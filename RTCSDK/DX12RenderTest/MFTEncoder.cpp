#include "MFTEncoder.h"

#include "DXSampleHelper.h"
#include <codecapi.h>
#include <wmsdk.h>

HRESULT GetVideoSourceFromDevice(UINT nDevice, IMFMediaSource** ppVideoSource, IMFSourceReader** ppVideoReader)
{
	UINT32 videoDeviceCount = 0;
	IMFAttributes* videoConfig = NULL;
	IMFActivate** videoDevices = NULL;
	WCHAR* webcamFriendlyName;
	UINT nameLength = 0;
	IMFAttributes* pAttributes = NULL;

	HRESULT hr = S_OK;

	// Get the first available webcam.
	hr = MFCreateAttributes(&videoConfig, 1);
	ThrowIfFailed(hr);

	// Request video capture devices.
	hr = videoConfig->SetGUID(
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
	ThrowIfFailed(hr);

	hr = MFEnumDeviceSources(videoConfig, &videoDevices, &videoDeviceCount);
	ThrowIfFailed(hr);

	if (nDevice >= videoDeviceCount) {
		printf("The device index of %d was invalid for available device count of %d.\n", nDevice, videoDeviceCount);
		hr = E_INVALIDARG;
	}
	else {
		hr = videoDevices[nDevice]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &webcamFriendlyName, &nameLength);
		ThrowIfFailed(hr);

		wprintf(L"Using webcam: %s\n", webcamFriendlyName);

		hr = videoDevices[nDevice]->ActivateObject(IID_PPV_ARGS(ppVideoSource));
		ThrowIfFailed(hr);

		ThrowIfFailed(MFCreateAttributes(&pAttributes, 1));

		if (ppVideoReader != nullptr) {
			// Adding this attribute creates a video source reader that will handle
			// colour conversion and avoid the need to manually convert between RGB24 and RGB32 etc.
			ThrowIfFailed(pAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, 1),
				"Failed to set enable video processing attribute.");

			// Create a source reader.
			hr = MFCreateSourceReaderFromMediaSource(
				*ppVideoSource,
				pAttributes,
				ppVideoReader);
			ThrowIfFailed(hr, "Error creating video source reader.");
		}
	}

done:

	SAFE_RELEASE(videoConfig);
	SAFE_RELEASE(*videoDevices);
	SAFE_RELEASE(pAttributes);

	return hr;
}


void MFEncoder::Encode()
{
	ComPtr<IMFMediaSource> pVideoSource = NULL;
	ComPtr<IMFSourceReader> pVideoReader = NULL;
	ComPtr<IMFMediaType> pSrcOutMediaType = NULL;

	ThrowIfFailed(GetVideoSourceFromDevice(0, &pVideoSource, &pVideoReader));

	// Note the webcam needs to support this media type. 
	MFCreateMediaType(&pSrcOutMediaType);
	ThrowIfFailed(pSrcOutMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
	ThrowIfFailed(pSrcOutMediaType->SetGUID(MF_MT_SUBTYPE, WMMEDIASUBTYPE_I420));
	//CHECK_HR(pSrcOutMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB24));
	ThrowIfFailed(MFSetAttributeRatio(pSrcOutMediaType.Get(), MF_MT_FRAME_RATE, 30, 1));
	ThrowIfFailed(MFSetAttributeSize(pSrcOutMediaType.Get(), MF_MT_FRAME_SIZE, 640, 360));

	ThrowIfFailed(pVideoReader->SetCurrentMediaType(0, NULL, pSrcOutMediaType.Get()),
		"Failed to set media type on source reader.");

	ComPtr<IUnknown> spEncTransformUnk = NULL;
	ThrowIfFailed(CoCreateInstance(CLSID_CMSH264EncoderMFT, NULL, CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&spEncTransformUnk)));

	ComPtr<IMFTransform> pEncoderTransform = NULL;
	ThrowIfFailed(spEncTransformUnk->QueryInterface(IID_PPV_ARGS(&pEncoderTransform)));

	ComPtr<IMFMediaType> pMFTInputMediaType = NULL;
	MFCreateMediaType(&pMFTInputMediaType);
	ThrowIfFailed(pSrcOutMediaType->CopyAllItems(pMFTInputMediaType.Get()));
	ThrowIfFailed(pMFTInputMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12));

	ComPtr<IMFMediaType> pMFTOutputMediaType = NULL;
	MFCreateMediaType(&pMFTOutputMediaType);
	ThrowIfFailed(pMFTInputMediaType->CopyAllItems(pMFTOutputMediaType.Get()));
	ThrowIfFailed(pMFTOutputMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264));
	ThrowIfFailed(pMFTOutputMediaType->SetUINT32(MF_MT_AVG_BITRATE, 240000));
	ThrowIfFailed(pMFTOutputMediaType->SetUINT32(MF_MT_INTERLACE_MODE, 2));
	ThrowIfFailed(pMFTOutputMediaType->SetUINT32(MF_MT_VIDEO_PROFILE, eAVEncH264VProfile_ConstrainedBase));
	//ThrowIfFailed(MFSetAttributeRatio(pMFTOutputMediaType.Get(), MF_MT_VIDEO_PROFILE, eAVEncH264VProfile_ConstrainedBase, 1));
	//ThrowIfFailed(pMFTOutputMediaType->SetDouble(MF_MT_MPEG2_LEVEL, 3.1));
	//ThrowIfFailed(pMFTOutputMediaType->SetUINT32(MF_MT_MAX_KEYFRAME_SPACING, 10));
	//ThrowIfFailed(pMFTOutputMediaType->SetUINT32(CODECAPI_AVEncCommonQuality, 100));


	ThrowIfFailed(pEncoderTransform->SetOutputType(0, pMFTOutputMediaType.Get(), 0));

	ThrowIfFailed(pEncoderTransform->SetInputType(0, pMFTInputMediaType.Get(), 0));

	//ThrowIfFailed(pEncoderTransform->GetInputStatus(0, &mftStatus));
}
