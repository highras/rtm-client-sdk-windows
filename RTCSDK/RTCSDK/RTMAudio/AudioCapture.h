// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
#pragma once
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include <avrt.h>
#include <thread>
#include <mutex>

#include "AudioRingBuffer.h"

using namespace std;

//
//  WASAPI Capture class.
class AudioCapture : public IAudioSessionEvents, IMMNotificationClient
{
public:
    //  Public interface to AudioCapture.
    AudioCapture(IMMDevice *Endpoint, bool EnableStreamSwitch, ERole EndpointRole);
    ~AudioCapture(void);
    bool Initialize(UINT32 EngineLatency);
    void Shutdown();
    bool Start();
    void Stop();
    size_t GetAudioData(BYTE* data, size_t count);

    WORD ChannelCount() { return _MixFormat->nChannels; }
    UINT32 SamplesPerSecond() { return _MixFormat->nSamplesPerSec; }
    UINT32 BytesPerSample() { return _MixFormat->wBitsPerSample / 8; }
    size_t FrameSize() { return _FrameSize; }
    WAVEFORMATEX *MixFormat() { return _MixFormat; }

    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

private:
    LONG                _RefCount;
    //
    //  Core Audio Capture member variables.
    //
    IMMDevice *         _Endpoint;
    IAudioClient *      _AudioClient;
    IAudioCaptureClient *_CaptureClient;

    thread*              _CaptureThread;
    HANDLE              _ShutdownEvent;
    HANDLE              _AudioSamplesReadyEvent;
    WAVEFORMATEX *      _MixFormat;
    size_t              _FrameSize;

    //
    //  Capture buffer management.
    //
    mutex _BufferMutex;
    AudioRingBuffer<BYTE> _CaptureBuffer;

    DWORD DoCaptureThread();
    //
    //  Stream switch related members and methods.
    //
    bool                    _EnableStreamSwitch;
    ERole                   _EndpointRole;
    HANDLE                  _StreamSwitchEvent;          // Set when the current session is disconnected or the default device changes.
    HANDLE                  _StreamSwitchCompleteEvent;  // Set when the default device changed.
    IAudioSessionControl *  _AudioSessionControl;
    IMMDeviceEnumerator *   _DeviceEnumerator;
    LONG                    _EngineLatencyInMS;
    bool                    _InStreamSwitch;

    bool InitializeStreamSwitch();
    void TerminateStreamSwitch();
    bool HandleStreamSwitchEvent();

    STDMETHOD(OnDisplayNameChanged) (LPCWSTR /*NewDisplayName*/, LPCGUID /*EventContext*/) { return S_OK; };
    STDMETHOD(OnIconPathChanged) (LPCWSTR /*NewIconPath*/, LPCGUID /*EventContext*/) { return S_OK; };
    STDMETHOD(OnSimpleVolumeChanged) (float /*NewSimpleVolume*/, BOOL /*NewMute*/, LPCGUID /*EventContext*/) { return S_OK; }
    STDMETHOD(OnChannelVolumeChanged) (DWORD /*ChannelCount*/, float /*NewChannelVolumes*/[], DWORD /*ChangedChannel*/, LPCGUID /*EventContext*/) { return S_OK; };
    STDMETHOD(OnGroupingParamChanged) (LPCGUID /*NewGroupingParam*/, LPCGUID /*EventContext*/) {return S_OK; };
    STDMETHOD(OnStateChanged) (AudioSessionState /*NewState*/) { return S_OK; };
    STDMETHOD(OnSessionDisconnected) (AudioSessionDisconnectReason DisconnectReason);
    STDMETHOD(OnDeviceStateChanged) (LPCWSTR /*DeviceId*/, DWORD /*NewState*/) { return S_OK; }
    STDMETHOD(OnDeviceAdded) (LPCWSTR /*DeviceId*/) { return S_OK; };
    STDMETHOD(OnDeviceRemoved) (LPCWSTR /*DeviceId(*/) { return S_OK; };
    STDMETHOD(OnDefaultDeviceChanged) (EDataFlow Flow, ERole Role, LPCWSTR NewDefaultDeviceId);
    STDMETHOD(OnPropertyValueChanged) (LPCWSTR /*DeviceId*/, const PROPERTYKEY /*Key*/){return S_OK; };
    //
    //  IUnknown
    //
    STDMETHOD(QueryInterface)(REFIID iid, void **pvObject);

    //
    //  Utility functions.
    //
    bool InitializeAudioEngine();
    bool LoadFormat();
};
