// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
#include "AudioCapture.h"
#include <assert.h>
#include "framework.h"

//extern bool DisableMMCSS;
//
//  A simple WASAPI Capture client.
//

AudioCapture::AudioCapture(IMMDevice *Endpoint, bool EnableStreamSwitch, ERole EndpointRole) : 
    _RefCount(1),
    _Endpoint(Endpoint),
    _AudioClient(NULL),
    _CaptureClient(NULL),
    _CaptureThread(nullptr),
    _ShutdownEvent(NULL),
    _CaptureBuffer(960*4*2*10),
    _MixFormat(NULL),
    _AudioSamplesReadyEvent(NULL),
    _EnableStreamSwitch(EnableStreamSwitch),
    _EndpointRole(EndpointRole),
    _StreamSwitchEvent(NULL),
    _StreamSwitchCompleteEvent(NULL),
    _AudioSessionControl(NULL),
    _DeviceEnumerator(NULL),
    _InStreamSwitch(false),
    _EngineLatencyInMS(0),
    _FrameSize(0)
{
    _Endpoint->AddRef();    // Since we're holding a copy of the endpoint, take a reference to it.  It'll be released in Shutdown();
}

//
//  Empty destructor - everything should be released in the Shutdown() call.
//
AudioCapture::~AudioCapture(void) 
{
    CoUninitialize();
}


//
//  Initialize WASAPI in event driven mode, associate the audio client with our samples ready event handle, retrieve 
//  a capture client for the transport, create the capture thread and start the audio engine.
//
bool AudioCapture::InitializeAudioEngine()
{
    HRESULT hr = _AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST, _EngineLatencyInMS*10000, 0, _MixFormat, NULL);

    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to initialize audio client: %x.\n", hr);
        return false;
    }

    //
    //  Retrieve the buffer size for the audio client.
    //
    //hr = _AudioClient->GetBufferSize(&_BufferSize);
    //if(FAILED(hr))
    //{
    //    fprintf(stderr, "Unable to get audio client buffer: %x. \n", hr);
    //    return false;
    //}

    hr = _AudioClient->SetEventHandle(_AudioSamplesReadyEvent);
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to set ready event: %x.\n", hr);
        return false;
    }

    hr = _AudioClient->GetService(IID_PPV_ARGS(&_CaptureClient));
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to get new capture client: %x.\n", hr);
        return false;
    }

    return true;
}

//
//  Retrieve the format we'll use to capture samples.
//
//  We use the Mix format since we're capturing in shared mode.
//

uint32_t GetFormatTag(const WAVEFORMATEX* wfx)
{
    if (wfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
        if (wfx->cbSize < (sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX)))
            return 0;

        static const GUID s_wfexBase = { 0x00000000, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 };

        auto wfex = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(wfx);

        if (memcmp(reinterpret_cast<const BYTE*>(&wfex->SubFormat) + sizeof(DWORD),
            reinterpret_cast<const BYTE*>(&s_wfexBase) + sizeof(DWORD), sizeof(GUID) - sizeof(DWORD)) != 0)
        {
            return 0;
        }

        return wfex->SubFormat.Data1;
    }
    else
    {
        return wfx->wFormatTag;
    }
}

bool AudioCapture::LoadFormat()
{
    HRESULT hr = _AudioClient->GetMixFormat(&_MixFormat);
    auto wfex = GetFormatTag(_MixFormat);
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to get mix format on audio client: %x.\n", hr);
        return false;
    }

    _FrameSize = (_MixFormat->wBitsPerSample / 8) * _MixFormat->nChannels;
    return true;
}

//
//  Initialize the capturer.
//
bool AudioCapture::Initialize(UINT32 EngineLatency)
{
    //
    //  Create our shutdown and samples ready events- we want auto reset events that start in the not-signaled state.
    //
    _ShutdownEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
    if (_ShutdownEvent == NULL)
    {
        fprintf(stderr, "Unable to create shutdown event: %d.\n", GetLastError());
        return false;
    }

    _AudioSamplesReadyEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
    if (_AudioSamplesReadyEvent == NULL)
    {
        fprintf(stderr, "Unable to create samples ready event: %d.\n", GetLastError());
        return false;
    }

    //
    //  Create our stream switch event- we want auto reset events that start in the not-signaled state.
    //  Note that we create this event even if we're not going to stream switch - that's because the event is used
    //  in the main loop of the capturer and thus it has to be set.
    //
    _StreamSwitchEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
    if (_StreamSwitchEvent == NULL)
    {
        fprintf(stderr, "Unable to create stream switch event: %d.\n", GetLastError());
        return false;
    }
        
    //
    //  Now activate an IAudioClient object on our preferred endpoint and retrieve the mix format for that endpoint.
    //
    HRESULT hr = _Endpoint->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&_AudioClient));
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to activate audio client: %x.\n", hr);
        return false;
    }

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_DeviceEnumerator));
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to instantiate device enumerator: %x\n", hr);
        return false;
    }

    //
    // Load the MixFormat.  This may differ depending on the shared mode used
    //
    if (!LoadFormat())
    {
        fprintf(stderr, "Failed to load the mix format \n");
        return false;
    }

    //
    //  Remember our configured latency in case we'll need it for a stream switch later.
    //
    _EngineLatencyInMS = EngineLatency;

    if (!InitializeAudioEngine())
    {
        return false;
    }

    if (_EnableStreamSwitch)
    {
        if (!InitializeStreamSwitch())
        {
            return false;
        }
    }

    return true;
}

//
//  Shut down the capture code and free all the resources.
//
void AudioCapture::Shutdown()
{
    if (_CaptureThread)
    {
        SetEvent(_ShutdownEvent);
        _CaptureThread->join();
        delete _CaptureThread;
        _CaptureThread = nullptr;
    }

    if (_ShutdownEvent)
    {
        CloseHandle(_ShutdownEvent);
        _ShutdownEvent = NULL;
    }
    if (_AudioSamplesReadyEvent)
    {
        CloseHandle(_AudioSamplesReadyEvent);
        _AudioSamplesReadyEvent = NULL;
    }
    if (_StreamSwitchEvent)
    {
        CloseHandle(_StreamSwitchEvent);
        _StreamSwitchEvent = NULL;
    }

    SAFE_RELEASE(_Endpoint);
    SAFE_RELEASE(_AudioClient);
    SAFE_RELEASE(_CaptureClient);

    if (_MixFormat)
    {
        CoTaskMemFree(_MixFormat);
        _MixFormat = NULL;
    }

    if (_EnableStreamSwitch)
    {
        TerminateStreamSwitch();
    }
}


//
//  Start capturing...
//
bool AudioCapture::Start()
{
    HRESULT hr;


    //
    //  Now create the thread which is going to drive the capture.
    //
    _CaptureThread = new thread([this] () {
        DoCaptureThread();
        });

    //
    //  We're ready to go, start capturing!
    //
    hr = _AudioClient->Start();
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to start capture client: %x.\n", hr);
        return false;
    }

    return true;
}

//
//  Stop the capturer.
//
void AudioCapture::Stop()
{
    HRESULT hr;

    //
    //  Tell the capture thread to shut down, wait for the thread to complete then clean up all the stuff we 
    //  allocated in Start().
    //
    if (_ShutdownEvent)
    {
        SetEvent(_ShutdownEvent);
    }

    hr = _AudioClient->Stop();
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to stop audio client: %x\n", hr);
    }

    if (_CaptureThread)
    {
        _CaptureThread->join();
        delete _CaptureThread;
        _CaptureThread = nullptr;
    }

    unique_lock<mutex> lck(_BufferMutex);
    _CaptureBuffer.Reset();
}

size_t AudioCapture::GetAudioData(BYTE* data, size_t count)
{
    unique_lock<mutex> lck(_BufferMutex);
    if (_CaptureBuffer.AvailableRead() > count)
        return _CaptureBuffer.Read(data, count);
    return 0;
}

DWORD AudioCapture::DoCaptureThread()
{
    bool stillPlaying = true;
    HANDLE waitArray[3] = {_ShutdownEvent, _StreamSwitchEvent, _AudioSamplesReadyEvent };
    HANDLE mmcssHandle = NULL;
    DWORD mmcssTaskIndex = 0;

    /*if (!DisableMMCSS)
    {
        mmcssHandle = AvSetMmThreadCharacteristics(L"Audio", &mmcssTaskIndex);
        if (mmcssHandle == NULL)
        {
            fprintf(stderr, "Unable to enable MMCSS on capture thread: %d\n", GetLastError());
        }
        else
            AvRevertMmThreadCharacteristics(mmcssHandle);
    }*/
    while (stillPlaying)
    {
        HRESULT hr;
        DWORD waitResult = WaitForMultipleObjects(3, waitArray, FALSE, INFINITE);
        switch (waitResult)
        {
        case WAIT_OBJECT_0 + 0:     // _ShutdownEvent
            stillPlaying = false;       // We're done, exit the loop.
            break;
        case WAIT_OBJECT_0 + 1:     // _StreamSwitchEvent
            //
            //  We need to stop the capturer, tear down the _AudioClient and _CaptureClient objects and re-create them on the new.
            //  endpoint if possible.  If this fails, abort the thread.
            //
            if (!HandleStreamSwitchEvent())
            {
                stillPlaying = false;
            }
            break;
        case WAIT_OBJECT_0 + 2:     // _AudioSamplesReadyEvent
            //
            //  We need to retrieve the next buffer of samples from the audio capturer.
            //
            BYTE *pData;
            UINT32 framesAvailable;
            DWORD  flags;

            //
            //  Find out how much capture data is available.  We need to make sure we don't run over the length
            //  of our capture buffer.  We'll discard any samples that don't fit in the buffer.
            //
            hr = _CaptureClient->GetBuffer(&pData, &framesAvailable, &flags, NULL, NULL);
            if (SUCCEEDED(hr))
            {
                unique_lock<mutex> lck(_BufferMutex);
                //
                    //  The flags on capture tell us information about the data.
                    //
                    //  We only really care about the silent flag since we want to put frames of silence into the buffer
                    //  when we receive silence.  We rely on the fact that a logical bit 0 is silence for both float and int formats.
                    //
                if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
                {
                    //
                    //  Fill 0s from the capture buffer to the output buffer.
                    //
                    _CaptureBuffer.Write((BYTE)0, framesAvailable * _FrameSize);
                }
                else
                {
                    //
                    //  Copy data from the audio engine buffer to the output buffer.
                    //
                    _CaptureBuffer.Write(pData, framesAvailable * _FrameSize);
                }
                
             
                hr = _CaptureClient->ReleaseBuffer(framesAvailable);
                if (FAILED(hr))
                {
                    fprintf(stderr, "Unable to release capture buffer: %x!\n", hr);
                }
            }
            break;
        }
    }

    return 0;
}


//
//  Initialize the stream switch logic.
//
bool AudioCapture::InitializeStreamSwitch()
{
    HRESULT hr = _AudioClient->GetService(IID_PPV_ARGS(&_AudioSessionControl));
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to retrieve session control: %x\n", hr);
        return false;
    }

    //
    //  Create the stream switch complete event- we want a manual reset event that starts in the not-signaled state.
    //
    _StreamSwitchCompleteEvent = CreateEventEx(NULL, NULL, CREATE_EVENT_INITIAL_SET | CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE);
    if (_StreamSwitchCompleteEvent == NULL)
    {
        fprintf(stderr, "Unable to create stream switch event: %d.\n", GetLastError());
        return false;
    }
    //
    //  Register for session and endpoint change notifications.  
    //
    //  A stream switch is initiated when we receive a session disconnect notification or we receive a default device changed notification.
    //
    hr = _AudioSessionControl->RegisterAudioSessionNotification(this);
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to register for stream switch notifications: %x\n", hr);
        return false;
    }

    hr = _DeviceEnumerator->RegisterEndpointNotificationCallback(this);
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to register for stream switch notifications: %x\n", hr);
        return false;
    }

    return true;
}

void AudioCapture::TerminateStreamSwitch()
{
    HRESULT hr = _AudioSessionControl->UnregisterAudioSessionNotification(this);
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to unregister for session notifications: %x\n", hr);
    }

    _DeviceEnumerator->UnregisterEndpointNotificationCallback(this);
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to unregister for endpoint notifications: %x\n", hr);
    }

    if (_StreamSwitchCompleteEvent)
    {
        CloseHandle(_StreamSwitchCompleteEvent);
        _StreamSwitchCompleteEvent = NULL;
    }

    SAFE_RELEASE(_AudioSessionControl);
    SAFE_RELEASE(_DeviceEnumerator);
}

//
//  Handle the stream switch.
//
//  When a stream switch happens, we want to do several things in turn:
//
//  1) Stop the current capturer.
//  2) Release any resources we have allocated (the _AudioClient, _AudioSessionControl (after unregistering for notifications) and 
//        _CaptureClient).
//  3) Wait until the default device has changed (or 500ms has elapsed).  If we time out, we need to abort because the stream switch can't happen.
//  4) Retrieve the new default endpoint for our role.
//  5) Re-instantiate the audio client on that new endpoint.  
//  6) Retrieve the mix format for the new endpoint.  If the mix format doesn't match the old endpoint's mix format, we need to abort because the stream
//      switch can't happen.
//  7) Re-initialize the _AudioClient.
//  8) Re-register for session disconnect notifications and reset the stream switch complete event.
//
bool AudioCapture::HandleStreamSwitchEvent()
{
    HRESULT hr;
    DWORD waitResult;

    assert(_InStreamSwitch);
    //
    //  Step 1.  Stop capturing.
    //
    hr = _AudioClient->Stop();
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to stop audio client during stream switch: %x\n", hr);
        goto ErrorExit;
    }

    //
    //  Step 2.  Release our resources.  Note that we don't release the mix format, we need it for step 6.
    //
    hr = _AudioSessionControl->UnregisterAudioSessionNotification(this);
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to stop audio client during stream switch: %x\n", hr);
        goto ErrorExit;
    }

    SAFE_RELEASE(_AudioSessionControl);
    SAFE_RELEASE(_CaptureClient);
    SAFE_RELEASE(_AudioClient);
    SAFE_RELEASE(_Endpoint);

    //
    //  Step 3.  Wait for the default device to change.
    //
    //  There is a race between the session disconnect arriving and the new default device 
    //  arriving (if applicable).  Wait the shorter of 500 milliseconds or the arrival of the 
    //  new default device, then attempt to switch to the default device.  In the case of a 
    //  format change (i.e. the default device does not change), we artificially generate  a
    //  new default device notification so the code will not needlessly wait 500ms before 
    //  re-opening on the new format.  (However, note below in step 6 that in this SDK 
    //  sample, we are unlikely to actually successfully absorb a format change, but a 
    //  real audio application implementing stream switching would re-format their 
    //  pipeline to deliver the new format).  
    //
    waitResult = WaitForSingleObject(_StreamSwitchCompleteEvent, 500);
    if (waitResult == WAIT_TIMEOUT)
    {
        fprintf(stderr, "Stream switch timeout - aborting...\n");
        goto ErrorExit;
    }

    //
    //  Step 4.  If we can't get the new endpoint, we need to abort the stream switch.  If there IS a new device,
    //          we should be able to retrieve it.
    //
    hr = _DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, _EndpointRole, &_Endpoint);
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to retrieve new default device during stream switch: %x\n", hr);
        goto ErrorExit;
    }
    //
    //  Step 5 - Re-instantiate the audio client on the new endpoint.
    //
    hr = _Endpoint->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&_AudioClient));
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to activate audio client on the new endpoint: %x.\n", hr);
        goto ErrorExit;
    }
    //
    //  Step 6 - Retrieve the new mix format.
    //
    WAVEFORMATEX *wfxNew;
    hr = _AudioClient->GetMixFormat(&wfxNew);
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to retrieve mix format for new audio client: %x.\n", hr);
        goto ErrorExit;
    }

    //
    //  Note that this is an intentionally naive comparison.  A more sophisticated comparison would
    //  compare the sample rate, channel count and format and apply the appropriate conversions into the capture pipeline.
    //
    if (memcmp(_MixFormat, wfxNew, sizeof(WAVEFORMATEX) + wfxNew->cbSize) != 0)
    {
        fprintf(stderr, "New mix format doesn't match old mix format.  Aborting.\n");
        CoTaskMemFree(wfxNew);
        goto ErrorExit;
    }
    CoTaskMemFree(wfxNew);

    //
    //  Step 7:  Re-initialize the audio client.
    //
    if (!InitializeAudioEngine())
    {
        goto ErrorExit;
    }

    //
    //  Step 8: Re-register for session disconnect notifications.
    //
    hr = _AudioClient->GetService(IID_PPV_ARGS(&_AudioSessionControl));
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to retrieve session control on new audio client: %x\n", hr);
        goto ErrorExit;
    }
    hr = _AudioSessionControl->RegisterAudioSessionNotification(this);
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to retrieve session control on new audio client: %x\n", hr);
        goto ErrorExit;
    }

    //
    //  Reset the stream switch complete event because it's a manual reset event.
    //
    ResetEvent(_StreamSwitchCompleteEvent);
    //
    //  And we're done.  Start capturing again.
    //
    hr = _AudioClient->Start();
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to start the new audio client: %x\n", hr);
        goto ErrorExit;
    }

    _InStreamSwitch = false;
    return true;

ErrorExit:
    _InStreamSwitch = false;
    return false;
}

//
//  Called when an audio session is disconnected.  
//
//  When a session is disconnected because of a device removal or format change event, we just want 
//  to let the capture thread know that the session's gone away
//
HRESULT AudioCapture::OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason)
{
    if (DisconnectReason == DisconnectReasonDeviceRemoval)
    {
        //
        //  The stream was disconnected because the device we're capturing to was removed.
        //
        //  We want to reset the stream switch complete event (so we'll block when the HandleStreamSwitchEvent function
        //  waits until the default device changed event occurs).
        //
        //  Note that we don't set the _StreamSwitchCompleteEvent - that will be set when the OnDefaultDeviceChanged event occurs.
        //
        _InStreamSwitch = true;
        SetEvent(_StreamSwitchEvent);
    }
    if (DisconnectReason == DisconnectReasonFormatChanged)
    {
        //
        //  The stream was disconnected because the format changed on our capture device.
        //
        //  We want to flag that we're in a stream switch and then set the stream switch event (which breaks out of the capturer).  We also
        //  want to set the _StreamSwitchCompleteEvent because we're not going to see a default device changed event after this.
        //
        _InStreamSwitch = true;
        SetEvent(_StreamSwitchEvent);
        SetEvent(_StreamSwitchCompleteEvent);
    }
    return S_OK;
}
//
//  Called when the default capture device changed.  We just want to set an event which lets the stream switch logic know that it's ok to 
//  continue with the stream switch.
//
HRESULT AudioCapture::OnDefaultDeviceChanged(EDataFlow Flow, ERole Role, LPCWSTR /*NewDefaultDeviceId*/)
{
    if (Flow == eCapture && Role == _EndpointRole)
    {
        //
        //  The default capture device for our configuredf role was changed.  
        //
        //  If we're not in a stream switch already, we want to initiate a stream switch event.  
        //  We also we want to set the stream switch complete event.  That will signal the capture thread that it's ok to re-initialize the
        //  audio capturer.
        //
        if (!_InStreamSwitch)
        {
            _InStreamSwitch = true;
            SetEvent(_StreamSwitchEvent);
        }
        SetEvent(_StreamSwitchCompleteEvent);
    }
    return S_OK;
}

//
//  IUnknown
//
HRESULT AudioCapture::QueryInterface(REFIID Iid, void **Object)
{
    if (Object == NULL)
    {
        return E_POINTER;
    }
    *Object = NULL;

    if (Iid == IID_IUnknown)
    {
        *Object = static_cast<IUnknown *>(static_cast<IAudioSessionEvents *>(this));
        AddRef();
    }
    else if (Iid == __uuidof(IMMNotificationClient))
    {
        *Object = static_cast<IMMNotificationClient *>(this);
        AddRef();
    }
    else if (Iid == __uuidof(IAudioSessionEvents))
    {
        *Object = static_cast<IAudioSessionEvents *>(this);
        AddRef();
    }
    else
    {
        return E_NOINTERFACE;
    }
    return S_OK;
}

ULONG AudioCapture::AddRef()
{
    return InterlockedIncrement(&_RefCount);
}

ULONG AudioCapture::Release()
{
    ULONG returnValue = InterlockedDecrement(&_RefCount);
    if (returnValue == 0)
    {
        delete this;
    }
    return returnValue;
}