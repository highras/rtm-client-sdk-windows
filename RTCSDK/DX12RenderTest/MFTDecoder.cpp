#include "MFTDecoder.h"
#include "DXSampleHelper.h"
#include <libyuv.h>


#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")

#define CHECKHR_GOTO(hr, done) if (hr != S_OK) goto done
#define IF_EQUAL_RETURN(param, val) if(val == param) return #val
#define OUTPUT_COLOR_FORMAT MFVideoFormat_I420

LPCSTR GetGUIDNameConst(const GUID& guid)
{
    IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_SUBTYPE);
    IF_EQUAL_RETURN(guid, MF_MT_ALL_SAMPLES_INDEPENDENT);
    IF_EQUAL_RETURN(guid, MF_MT_FIXED_SIZE_SAMPLES);
    IF_EQUAL_RETURN(guid, MF_MT_COMPRESSED);
    IF_EQUAL_RETURN(guid, MF_MT_SAMPLE_SIZE);
    IF_EQUAL_RETURN(guid, MF_MT_WRAPPED_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_NUM_CHANNELS);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FLOAT_SAMPLES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_AVG_BYTES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BLOCK_ALIGNMENT);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BITS_PER_SAMPLE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_VALID_BITS_PER_SAMPLE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_BLOCK);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_CHANNEL_MASK);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FOLDDOWN_MATRIX);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKREF);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKTARGET);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGREF);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGTARGET);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_PREFER_WAVEFORMATEX);
    IF_EQUAL_RETURN(guid, MF_MT_AAC_PAYLOAD_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_SIZE);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MAX);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MIN);
    IF_EQUAL_RETURN(guid, MF_MT_PIXEL_ASPECT_RATIO);
    IF_EQUAL_RETURN(guid, MF_MT_DRM_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_PAD_CONTROL_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_SOURCE_CONTENT_HINT);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_CHROMA_SITING);
    IF_EQUAL_RETURN(guid, MF_MT_INTERLACE_MODE);
    IF_EQUAL_RETURN(guid, MF_MT_TRANSFER_FUNCTION);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_PRIMARIES);
    IF_EQUAL_RETURN(guid, MF_MT_CUSTOM_VIDEO_PRIMARIES);
    IF_EQUAL_RETURN(guid, MF_MT_YUV_MATRIX);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_LIGHTING);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_NOMINAL_RANGE);
    IF_EQUAL_RETURN(guid, MF_MT_GEOMETRIC_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_MINIMUM_DISPLAY_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_ENABLED);
    IF_EQUAL_RETURN(guid, MF_MT_AVG_BITRATE);
    IF_EQUAL_RETURN(guid, MF_MT_AVG_BIT_ERROR_RATE);
    IF_EQUAL_RETURN(guid, MF_MT_MAX_KEYFRAME_SPACING);
    IF_EQUAL_RETURN(guid, MF_MT_DEFAULT_STRIDE);
    IF_EQUAL_RETURN(guid, MF_MT_PALETTE);
    IF_EQUAL_RETURN(guid, MF_MT_USER_DATA);
    IF_EQUAL_RETURN(guid, MF_MT_AM_FORMAT_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG_START_TIME_CODE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_PROFILE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_LEVEL);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG_SEQUENCE_HEADER);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_0);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_0);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_1);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_1);
    IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_SRC_PACK);
    IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_CTRL_PACK);
    IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_HEADER);
    IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_FORMAT);
    IF_EQUAL_RETURN(guid, MF_MT_IMAGE_LOSS_TOLERANT);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG4_SAMPLE_DESCRIPTION);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG4_CURRENT_SAMPLE_ENTRY);
    IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_4CC);
    IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_WAVE_FORMAT_TAG);

    // Media types

    IF_EQUAL_RETURN(guid, MFMediaType_Audio);
    IF_EQUAL_RETURN(guid, MFMediaType_Video);
    IF_EQUAL_RETURN(guid, MFMediaType_Protected);
    IF_EQUAL_RETURN(guid, MFMediaType_SAMI);
    IF_EQUAL_RETURN(guid, MFMediaType_Script);
    IF_EQUAL_RETURN(guid, MFMediaType_Image);
    IF_EQUAL_RETURN(guid, MFMediaType_HTML);
    IF_EQUAL_RETURN(guid, MFMediaType_Binary);
    IF_EQUAL_RETURN(guid, MFMediaType_FileTransfer);

    IF_EQUAL_RETURN(guid, MFVideoFormat_AI44); //     FCC('AI44')
    IF_EQUAL_RETURN(guid, MFVideoFormat_ARGB32); //   D3DFMT_A8R8G8B8 
    IF_EQUAL_RETURN(guid, MFVideoFormat_AYUV); //     FCC('AYUV')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DV25); //     FCC('dv25')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DV50); //     FCC('dv50')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVH1); //     FCC('dvh1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVSD); //     FCC('dvsd')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVSL); //     FCC('dvsl')
    IF_EQUAL_RETURN(guid, MFVideoFormat_H264); //     FCC('H264')
    IF_EQUAL_RETURN(guid, MFVideoFormat_I420); //     FCC('I420')
    IF_EQUAL_RETURN(guid, MFVideoFormat_IYUV); //     FCC('IYUV')
    IF_EQUAL_RETURN(guid, MFVideoFormat_M4S2); //     FCC('M4S2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MJPG);
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP43); //     FCC('MP43')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP4S); //     FCC('MP4S')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP4V); //     FCC('MP4V')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MPG1); //     FCC('MPG1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MSS1); //     FCC('MSS1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MSS2); //     FCC('MSS2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_NV11); //     FCC('NV11')
    IF_EQUAL_RETURN(guid, MFVideoFormat_NV12); //     FCC('NV12')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P010); //     FCC('P010')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P016); //     FCC('P016')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P210); //     FCC('P210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P216); //     FCC('P216')
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB24); //    D3DFMT_R8G8B8 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB32); //    D3DFMT_X8R8G8B8 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB555); //   D3DFMT_X1R5G5B5 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB565); //   D3DFMT_R5G6B5 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB8);
    IF_EQUAL_RETURN(guid, MFVideoFormat_UYVY); //     FCC('UYVY')
    IF_EQUAL_RETURN(guid, MFVideoFormat_v210); //     FCC('v210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_v410); //     FCC('v410')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV1); //     FCC('WMV1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV2); //     FCC('WMV2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV3); //     FCC('WMV3')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WVC1); //     FCC('WVC1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y210); //     FCC('Y210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y216); //     FCC('Y216')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y410); //     FCC('Y410')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y416); //     FCC('Y416')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y41P);
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y41T);
    IF_EQUAL_RETURN(guid, MFVideoFormat_YUY2); //     FCC('YUY2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_YV12); //     FCC('YV12')
    IF_EQUAL_RETURN(guid, MFVideoFormat_YVYU);

    IF_EQUAL_RETURN(guid, MFAudioFormat_PCM); //              WAVE_FORMAT_PCM 
    IF_EQUAL_RETURN(guid, MFAudioFormat_Float); //            WAVE_FORMAT_IEEE_FLOAT 
    IF_EQUAL_RETURN(guid, MFAudioFormat_DTS); //              WAVE_FORMAT_DTS 
    IF_EQUAL_RETURN(guid, MFAudioFormat_Dolby_AC3_SPDIF); //  WAVE_FORMAT_DOLBY_AC3_SPDIF 
    IF_EQUAL_RETURN(guid, MFAudioFormat_DRM); //              WAVE_FORMAT_DRM 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV8); //        WAVE_FORMAT_WMAUDIO2 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV9); //        WAVE_FORMAT_WMAUDIO3 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudio_Lossless); // WAVE_FORMAT_WMAUDIO_LOSSLESS 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMASPDIF); //         WAVE_FORMAT_WMASPDIF 
    IF_EQUAL_RETURN(guid, MFAudioFormat_MSP1); //             WAVE_FORMAT_WMAVOICE9 
    IF_EQUAL_RETURN(guid, MFAudioFormat_MP3); //              WAVE_FORMAT_MPEGLAYER3 
    IF_EQUAL_RETURN(guid, MFAudioFormat_MPEG); //             WAVE_FORMAT_MPEG 
    IF_EQUAL_RETURN(guid, MFAudioFormat_AAC); //              WAVE_FORMAT_MPEG_HEAAC 
    IF_EQUAL_RETURN(guid, MFAudioFormat_ADTS); //             WAVE_FORMAT_MPEG_ADTS_AAC 

    return NULL;
}

std::string GetMediaTypeDescription(IMFMediaType* pMediaType)
{
    HRESULT hr = S_OK;
    GUID MajorType;
    UINT32 cAttrCount;
    LPCSTR pszGuidStr;
    std::string description;
    WCHAR TempBuf[200];

    if (pMediaType == NULL)
    {
        description = "<NULL>";
        goto done;
    }

    hr = pMediaType->GetMajorType(&MajorType);
    CHECKHR_GOTO(hr, done);

    //pszGuidStr = STRING_FROM_GUID(MajorType);
    pszGuidStr = GetGUIDNameConst(MajorType);
    if (pszGuidStr != NULL)
    {
        description += pszGuidStr;
        description += ": ";
    }
    else
    {
        description += "Other: ";
    }

    hr = pMediaType->GetCount(&cAttrCount);
    CHECKHR_GOTO(hr, done);

    for (UINT32 i = 0; i < cAttrCount; i++)
    {
        GUID guidId;
        MF_ATTRIBUTE_TYPE attrType;

        hr = pMediaType->GetItemByIndex(i, &guidId, NULL);
        CHECKHR_GOTO(hr, done);

        hr = pMediaType->GetItemType(guidId, &attrType);
        CHECKHR_GOTO(hr, done);

        //pszGuidStr = STRING_FROM_GUID(guidId);
        pszGuidStr = GetGUIDNameConst(guidId);
        if (pszGuidStr != NULL)
        {
            description += pszGuidStr;
        }
        else
        {
            LPOLESTR guidStr = NULL;

            CHECKHR_GOTO(StringFromCLSID(guidId, &guidStr), done);
            auto wGuidStr = std::wstring(guidStr);
            description += std::string(wGuidStr.begin(), wGuidStr.end()); // GUID's won't have wide chars.

            CoTaskMemFree(guidStr);
        }

        description += "=";

        switch (attrType)
        {
        case MF_ATTRIBUTE_UINT32:
        {
            UINT32 Val;
            hr = pMediaType->GetUINT32(guidId, &Val);
            CHECKHR_GOTO(hr, done);

            description += std::to_string(Val);
            break;
        }
        case MF_ATTRIBUTE_UINT64:
        {
            UINT64 Val;
            hr = pMediaType->GetUINT64(guidId, &Val);
            CHECKHR_GOTO(hr, done);

            if (guidId == MF_MT_FRAME_SIZE)
            {
                description += "W:" + std::to_string(HI32(Val)) + " H: " + std::to_string(LO32(Val));
            }
            else if (guidId == MF_MT_FRAME_RATE)
            {
                // Frame rate is numerator/denominator.
                description += std::to_string(HI32(Val)) + "/" + std::to_string(LO32(Val));
            }
            else if (guidId == MF_MT_PIXEL_ASPECT_RATIO)
            {
                description += std::to_string(HI32(Val)) + ":" + std::to_string(LO32(Val));
            }
            else
            {
                //tempStr.Format("%ld", Val);
                description += std::to_string(Val);
            }

            //description += tempStr;

            break;
        }
        case MF_ATTRIBUTE_DOUBLE:
        {
            DOUBLE Val;
            hr = pMediaType->GetDouble(guidId, &Val);
            CHECKHR_GOTO(hr, done);

            //tempStr.Format("%f", Val);
            description += std::to_string(Val);
            break;
        }
        case MF_ATTRIBUTE_GUID:
        {
            GUID Val;
            const char* pValStr;

            hr = pMediaType->GetGUID(guidId, &Val);
            CHECKHR_GOTO(hr, done);

            //pValStr = STRING_FROM_GUID(Val);
            pValStr = GetGUIDNameConst(Val);
            if (pValStr != NULL)
            {
                description += pValStr;
            }
            else
            {
                LPOLESTR guidStr = NULL;
                CHECKHR_GOTO(StringFromCLSID(Val, &guidStr), done);
                auto wGuidStr = std::wstring(guidStr);
                description += std::string(wGuidStr.begin(), wGuidStr.end()); // GUID's won't have wide chars.

                CoTaskMemFree(guidStr);
            }

            break;
        }
        case MF_ATTRIBUTE_STRING:
        {
            hr = pMediaType->GetString(guidId, TempBuf, sizeof(TempBuf) / sizeof(TempBuf[0]), NULL);
            if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
            {
                description += "<Too Long>";
                break;
            }
            CHECKHR_GOTO(hr, done);
            auto wstr = std::wstring(TempBuf);
            description += std::string(wstr.begin(), wstr.end()); // It's unlikely the attribute descriptions will contain multi byte chars.

            break;
        }
        case MF_ATTRIBUTE_BLOB:
        {
            description += "<BLOB>";
            break;
        }
        case MF_ATTRIBUTE_IUNKNOWN:
        {
            description += "<UNK>";
            break;
        }
        }

        description += ", ";
    }

done:

    return description;
}

HRESULT CreateSingleBufferIMFSample(DWORD bufferSize, IMFSample** pSample)
{
    IMFMediaBuffer* pBuffer = NULL;

    HRESULT hr = S_OK;

    hr = MFCreateSample(pSample);
    ThrowIfFailed(hr);

    // Adds a ref count to the pBuffer object.
    hr = MFCreateMemoryBuffer(bufferSize, &pBuffer);
    ThrowIfFailed(hr);

    // Adds another ref count to the pBuffer object.
    hr = (*pSample)->AddBuffer(pBuffer);
    ThrowIfFailed(hr);

done:
    // Leave the single ref count that will be removed when the pSample is released.
    SAFE_RELEASE(pBuffer);
    return hr;
}

HRESULT CreateAndCopySingleBufferIMFSample(IMFSample* pSrcSample, IMFSample** pDstSample)
{
    IMFMediaBuffer* pDstBuffer = NULL;
    DWORD srcBufLength;

    HRESULT hr = S_OK;

    // Gets total length of ALL media buffer samples. We can use here because it's only a
    // single buffer sample copy.
    hr = pSrcSample->GetTotalLength(&srcBufLength);
    ThrowIfFailed(hr);

    hr = CreateSingleBufferIMFSample(srcBufLength, pDstSample);
    ThrowIfFailed(hr);

    hr = pSrcSample->CopyAllItems(*pDstSample);
    ThrowIfFailed(hr);

    hr = (*pDstSample)->GetBufferByIndex(0, &pDstBuffer);
    ThrowIfFailed(hr);

    hr = pSrcSample->CopyToBuffer(pDstBuffer);
    ThrowIfFailed(hr);

done:
    SAFE_RELEASE(pDstBuffer);
    return hr;
}

HRESULT GetTransformOutput(IMFTransform* pTransform, IMFSample** pOutSample, bool* transformFlushed)
{
    MFT_OUTPUT_STREAM_INFO StreamInfo = { 0 };
    MFT_OUTPUT_DATA_BUFFER outputDataBuffer = { 0 };
    DWORD processOutputStatus = 0;
    IMFMediaType* pChangedOutMediaType = NULL;

    HRESULT hr = S_OK;
    *transformFlushed = FALSE;

    hr = pTransform->GetOutputStreamInfo(0, &StreamInfo);
    ThrowIfFailed(hr);

    outputDataBuffer.dwStreamID = 0;
    outputDataBuffer.dwStatus = 0;
    outputDataBuffer.pEvents = NULL;

    if ((StreamInfo.dwFlags & MFT_OUTPUT_STREAM_PROVIDES_SAMPLES) == 0) {
        hr = CreateSingleBufferIMFSample(StreamInfo.cbSize, pOutSample);
        ThrowIfFailed(hr);
        outputDataBuffer.pSample = *pOutSample;
    }

    auto mftProcessOutput = pTransform->ProcessOutput(0, 1, &outputDataBuffer, &processOutputStatus);

    //printf("Process output result %.2X, MFT status %.2X.\n", mftProcessOutput, processOutputStatus);

    if (mftProcessOutput == S_OK) {
        // Sample is ready and allocated on the transform output buffer.
        *pOutSample = outputDataBuffer.pSample;
    }
    else if (mftProcessOutput == MF_E_TRANSFORM_STREAM_CHANGE) {
        // Format of the input stream has changed. https://docs.microsoft.com/en-us/windows/win32/medfound/handling-stream-changes
        if (outputDataBuffer.dwStatus == MFT_OUTPUT_DATA_BUFFER_FORMAT_CHANGE) {
            printf("MFT stream changed.\n");

            hr = pTransform->GetOutputAvailableType(0, 0, &pChangedOutMediaType);
            ThrowIfFailed(hr);

            std::cout << "MFT output media type: " << GetMediaTypeDescription(pChangedOutMediaType) << std::endl << std::endl;

            hr = pChangedOutMediaType->SetGUID(MF_MT_SUBTYPE, OUTPUT_COLOR_FORMAT);
            ThrowIfFailed(hr);

            hr = pTransform->SetOutputType(0, pChangedOutMediaType, 0);
            ThrowIfFailed(hr);

            hr = pTransform->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, NULL);
            ThrowIfFailed(hr);

            *transformFlushed = TRUE;
        }
        else {
            printf("MFT stream changed but didn't have the data format change flag set. Don't know what to do.\n");
            hr = E_NOTIMPL;
        }

        SAFE_RELEASE(*pOutSample);
        *pOutSample = NULL;
    }
    else if (mftProcessOutput == MF_E_TRANSFORM_NEED_MORE_INPUT) {
        // More input is not an error condition but it means the allocated output sample is empty.
        SAFE_RELEASE(*pOutSample);
        *pOutSample = NULL;
        hr = MF_E_TRANSFORM_NEED_MORE_INPUT;
    }
    else {
        printf("MFT ProcessOutput error result %.2X, MFT status %.2X.\n", mftProcessOutput, processOutputStatus);
        hr = mftProcessOutput;
        SAFE_RELEASE(*pOutSample);
        *pOutSample = NULL;
    }

done:

    SAFE_RELEASE(pChangedOutMediaType);

    return hr;
}

MFTDecoder::MFTDecoder():
    reader("video.livedatavideo"),
    openh264(640,480)
{
}

MFTDecoder::~MFTDecoder()
{
    SAFE_RELEASE(pSourceResolver);
    SAFE_RELEASE(uSource);
    SAFE_RELEASE(mediaFileSource);
    SAFE_RELEASE(pVideoReaderAttributes);
    SAFE_RELEASE(pSourceReader);
    SAFE_RELEASE(pFileVideoMediaType);
    SAFE_RELEASE(spDecTransformUnk);
    SAFE_RELEASE(pDecoderTransform);
    SAFE_RELEASE(pDecInputMediaType);
    SAFE_RELEASE(pDecOutputMediaType);
}

void MFTDecoder::Init(const WCHAR* FilePath)
{
    ThrowIfFailed(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE));

    ThrowIfFailed(MFStartup(MF_VERSION));

    // Set up the reader for the file.
    ThrowIfFailed(MFCreateSourceResolver(&pSourceResolver));

    ThrowIfFailed(pSourceResolver->CreateObjectFromURL(
        FilePath,		        // URL of the source.
        MF_RESOLUTION_MEDIASOURCE,  // Create a source object.
        NULL,                       // Optional property store.
        &ObjectType,				        // Receives the created object type. 
        &uSource					          // Receives a pointer to the media source.
    ));

    ThrowIfFailed(uSource->QueryInterface(IID_PPV_ARGS(&mediaFileSource)));

    ThrowIfFailed(MFCreateAttributes(&pVideoReaderAttributes, 2));

    ThrowIfFailed(pVideoReaderAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID));

    //ThrowIfFailed(pVideoReaderAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, 1));

    ThrowIfFailed(MFCreateSourceReaderFromMediaSource(mediaFileSource, pVideoReaderAttributes, &pSourceReader));

    ThrowIfFailed(pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pFileVideoMediaType));

    // Create H.264 decoder.
    ThrowIfFailed(CoCreateInstance(CLSID_CMSH264DecoderMFT, NULL, CLSCTX_INPROC_SERVER,
        IID_IUnknown, (void**)&spDecTransformUnk));

    ThrowIfFailed(spDecTransformUnk->QueryInterface(IID_PPV_ARGS(&pDecoderTransform)));

    MFCreateMediaType(&pDecInputMediaType);
    ThrowIfFailed(pFileVideoMediaType->CopyAllItems(pDecInputMediaType));
    ThrowIfFailed(pDecoderTransform->SetInputType(0, pDecInputMediaType, 0));

    MFCreateMediaType(&pDecOutputMediaType);
    ThrowIfFailed(pFileVideoMediaType->CopyAllItems(pDecOutputMediaType));
    ThrowIfFailed(pDecOutputMediaType->SetGUID(MF_MT_SUBTYPE, OUTPUT_COLOR_FORMAT));

    ThrowIfFailed(pDecoderTransform->SetOutputType(0, pDecOutputMediaType, 0));

    ThrowIfFailed(pDecoderTransform->GetInputStatus(0, &mftStatus));
    if (MFT_INPUT_STATUS_ACCEPT_DATA != mftStatus) {
        throw ("H.264 decoder MFT is not accepting data.\n");
    }

    std::cout << "H264 decoder input media type: " << GetMediaTypeDescription(pDecInputMediaType) << std::endl << std::endl;
    std::cout << "H264 decoder output media type: " << GetMediaTypeDescription(pDecOutputMediaType) << std::endl << std::endl;
    UINT64 size = 0;
    //pDecOutputMediaType->GetUINT64(MF_MT_FRAME_SIZE, &size);
    //width = HI32(size);
    //height = LO32(size);

    ThrowIfFailed(pDecoderTransform->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, NULL));
    ThrowIfFailed(pDecoderTransform->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL));
    ThrowIfFailed(pDecoderTransform->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, NULL));
    width = 640;
    height = 480;
}

std::vector<unsigned char> MFTDecoder::Decode()
{
    // Start processing frames.

    std::vector<unsigned char> data(width * height * 3 / 2);
    bool output = false;

    while (!output)
    {
        ThrowIfFailed(pSourceReader->ReadSample(
            MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0,                              // Flags.
            &streamIndex,                   // Receives the actual stream index. 
            &flags,                         // Receives status flags.
            &llVideoTimeStamp,              // Receives the time stamp.
            &pVideoSample                    // Receives the sample or NULL.
        ));

        if (flags & MF_SOURCE_READERF_STREAMTICK)
        {
            printf("\tStream tick.\n");
        }
        if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            printf("\tEnd of stream.\n");
            return data;
        }
        if (flags & MF_SOURCE_READERF_NEWSTREAM)
        {
            printf("\tNew stream.\n");
            return data;
        }
        if (flags & MF_SOURCE_READERF_NATIVEMEDIATYPECHANGED)
        {
            printf("\tNative type changed.\n");
            return data;
        }
        if (flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
        {
            printf("\tCurrent type changed.\n");
            return data;
        }

        if (pVideoSample)
        {
            //printf("Processing sample %i.\n", sampleCount);

            ThrowIfFailed(pVideoSample->SetSampleTime(llVideoTimeStamp));
            ThrowIfFailed(pVideoSample->GetSampleDuration(&llSampleDuration));
            ThrowIfFailed(pVideoSample->GetSampleFlags(&sampleFlags));
            //IMFMediaBuffer* mediabuffer;
            //pVideoSample->ConvertToContiguousBuffer(&mediabuffer);
            //BYTE* dat;
            //DWORD mlen;
            //DWORD clen;
            //mediabuffer->Lock(&dat, &mlen, &clen);

            //data = openh264.Decode(dat, clen);

            //mediabuffer->Unlock();

            //if (data.size() > 0)
            //    output = true;
            //printf("Sample count %d, Sample flags %d, sample duration %I64d, sample time %I64d\n", sampleCount, sampleFlags, llSampleDuration, llVideoTimeStamp);

            // Replicate transmitting the sample across the network and reconstructing.
            ThrowIfFailed(CreateAndCopySingleBufferIMFSample(pVideoSample, &pCopyVideoSample));
            // Apply the H264 decoder transform
            ThrowIfFailed(pDecoderTransform->ProcessInput(0, pCopyVideoSample, 0));

            HRESULT getOutputResult = S_OK;
            while (getOutputResult == S_OK) {

                getOutputResult = GetTransformOutput(pDecoderTransform, &pH264DecodeOutSample, &h264DecodeTransformFlushed);

                if (getOutputResult != S_OK && getOutputResult != MF_E_TRANSFORM_NEED_MORE_INPUT) {
                    throw ("Error getting H264 decoder transform output, error code %.2X.\n", getOutputResult);
                }

                if (h264DecodeTransformFlushed == TRUE) {
                    // H264 decoder format changed. Clear the capture file and start again.
                    ;
                }
                else if (pH264DecodeOutSample != NULL) {
                    // Write decoded sample to capture file.
                    IMFMediaBuffer* buf = NULL;
                    DWORD bufLength;

                    HRESULT hr = S_OK;

                    hr = pH264DecodeOutSample->ConvertToContiguousBuffer(&buf);
                    ThrowIfFailed(hr);

                    hr = buf->GetCurrentLength(&bufLength);
                    ThrowIfFailed(hr);

                    byte* byteBuffer = NULL;
                    DWORD buffMaxLen = 0, buffCurrLen = 0;
                    buf->Lock(&byteBuffer, &buffMaxLen, &buffCurrLen);


                    memcpy_s(data.data(), data.size(), byteBuffer, width * height);
                    byteBuffer += (width / 16 + ((width % 16) > 0)) * 16 * (height / 16 + ((height % 16) > 0)) * 16;
                    memcpy_s(data.data() + width * height, data.size() - width * height, byteBuffer , width * height / 4);
                    byteBuffer += (width / 16 + ((width % 16) > 0)) * 16 * (height / 16 + ((height % 16) > 0)) * 16 / 4;
                    memcpy_s(data.data() + width * height * 5 /4, data.size() - width * height * 5 / 4, byteBuffer, width * height / 4);

                    output = true;
                    SAFE_RELEASE(buf);
                }

                SAFE_RELEASE(pH264DecodeOutSample);
            }

            sampleCount++;
        }
        SAFE_RELEASE(pVideoSample);
        SAFE_RELEASE(pCopyVideoSample);
    }
    return data;
}

std::vector<unsigned char> MFTDecoder::DecodeOpenH264()
{
    std::vector<unsigned char> result;


    int32_t captureLevel = 0;
    int32_t rotation = 0;
    int32_t cameraFacing = 0;
    int64_t timestamp = 0;
    int32_t spsLength = 0;
    int32_t ppsLength = 0;
    int32_t dataLength = 0;

    static char* data[3] = { new char[512] ,new char[512],new char[512 * 1024] };
    static size_t length[3] = { 512,512,512 * 1024 };
    do
    {
        reader.Read(data, length, captureLevel, rotation, cameraFacing, timestamp, spsLength, ppsLength, dataLength);
        if (dataLength > 0)
        {
            result = openh264.Decode((BYTE*)data[2], dataLength);
            if (result.size() > 0)
            {
                return result;
            }

        }
    } while (dataLength > 0);

    return result;
}

std::vector<unsigned char> MFTDecoder::DecodeTest()
{
    std::vector<unsigned char> result;

    char* data[3];
    data[0] = new char[512];
    data[1] = new char[512];
    data[2] = new char[512 * 1024];

    size_t length[3] = { 512,512,512*1024 };
    int32_t captureLevel = 0;
    int32_t rotation = 0;
    int32_t cameraFacing = 0;
    int64_t timestamp = 0;
    int32_t spsLength = 0;
    int32_t ppsLength = 0;
    int32_t dataLength = 0;

    IMFTransform* l_pDecoderTransform = NULL; // This is H264 Decoder MFT.
    IMFMediaType* l_pDecInputMediaType = NULL, * l_pDecOutputMediaType = NULL;
    DWORD l_mftStatus = 0;

    ThrowIfFailed(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE));

    ThrowIfFailed(MFStartup(MF_VERSION));

    // Create H.264 decoder.
    ThrowIfFailed(CoCreateInstance(CLSID_CMSH264DecoderMFT, NULL, CLSCTX_INPROC_SERVER,
        IID_IUnknown, (void**)&spDecTransformUnk));

    ThrowIfFailed(spDecTransformUnk->QueryInterface(IID_PPV_ARGS(&l_pDecoderTransform)));

    MFCreateMediaType(&l_pDecInputMediaType);

    ThrowIfFailed(l_pDecInputMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
    ThrowIfFailed(l_pDecInputMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264));
    ThrowIfFailed(l_pDecInputMediaType->SetUINT32(MF_MT_COMPRESSED, 1));
    ThrowIfFailed(MFSetAttributeSize(l_pDecInputMediaType, MF_MT_FRAME_SIZE, 320, 240));
    ThrowIfFailed(MFSetAttributeRatio(l_pDecInputMediaType, MF_MT_FRAME_RATE, 15, 1));
    ThrowIfFailed(l_pDecoderTransform->SetInputType(0, l_pDecInputMediaType, 0));
    MFCreateMediaType(&l_pDecOutputMediaType);
    ThrowIfFailed(l_pDecInputMediaType->CopyAllItems(l_pDecOutputMediaType));
    ThrowIfFailed(l_pDecOutputMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12));
    ThrowIfFailed(l_pDecOutputMediaType->SetUINT32(MF_MT_COMPRESSED, 0));

    std::cout << "H264 decoder output media type: " << GetMediaTypeDescription(l_pDecOutputMediaType) << std::endl << std::endl;
    ThrowIfFailed(l_pDecoderTransform->SetOutputType(0, l_pDecOutputMediaType, 0));

    ThrowIfFailed(l_pDecoderTransform->GetInputStatus(0, &l_mftStatus));
    if (MFT_INPUT_STATUS_ACCEPT_DATA != l_mftStatus) {
        throw ("H.264 decoder MFT is not accepting data.\n");
    }

    
    //l_pDecOutputMediaType->GetUINT64(MF_MT_FRAME_SIZE, &size);
    //width = HI32(size);
    //height = LO32(size);

    ThrowIfFailed(l_pDecoderTransform->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, NULL));
    ThrowIfFailed(l_pDecoderTransform->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL));
    ThrowIfFailed(l_pDecoderTransform->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, NULL));

    do
    {
        reader.Read(data, length, captureLevel, rotation, cameraFacing, timestamp, spsLength, ppsLength, dataLength);
        if (dataLength > 0)
        { 
            IMFSample *sample;
            HRESULT hr = CreateSingleBufferIMFSample(dataLength, &sample);
            IMFMediaBuffer* buffer;
            sample->ConvertToContiguousBuffer(&buffer);
            BYTE* inputData;
            DWORD maxlen;
            DWORD len;
            buffer->Lock(&inputData, &maxlen, &len);
            //if (spsLength > 0)
            //{
            //    memcpy_s(inputData, maxlen, data[0], spsLength);
            //    inputData += spsLength;
            //    maxlen -= spsLength;
            //    memcpy_s(inputData, maxlen, data[1], ppsLength);
            //    inputData += ppsLength;
            //    maxlen -= ppsLength;
            //}
            memcpy_s(inputData, maxlen, data[2], dataLength);
            buffer->Unlock();
            sample->SetSampleTime(timestamp*1000000);
            sample->SetSampleDuration(1000*1000/30);
            hr = l_pDecoderTransform->ProcessInput(0, sample, 0);
            ThrowIfFailed(hr);


            HRESULT getOutputResult = S_OK;
            while (getOutputResult == S_OK) {

                getOutputResult = GetTransformOutput(l_pDecoderTransform, &pH264DecodeOutSample, &h264DecodeTransformFlushed);

                if (getOutputResult != S_OK && getOutputResult != MF_E_TRANSFORM_NEED_MORE_INPUT) {
                    throw ("Error getting H264 decoder transform output, error code %.2X.\n", getOutputResult);
                }

                if (h264DecodeTransformFlushed == TRUE) {
                    // H264 decoder format changed. Clear the capture file and start again.
                    printf("flushed!\n");
                }
                else if (pH264DecodeOutSample != NULL) {
                    // Write decoded sample to capture file.
                    IMFMediaBuffer* buf = NULL;
                    DWORD bufLength;

                    HRESULT hr = S_OK;

                    hr = pH264DecodeOutSample->ConvertToContiguousBuffer(&buf);
                    ThrowIfFailed(hr);

                    hr = buf->GetCurrentLength(&bufLength);
                    ThrowIfFailed(hr);

                    byte* byteBuffer = NULL;
                    DWORD buffMaxLen = 0, buffCurrLen = 0;
                    buf->Lock(&byteBuffer, &buffMaxLen, &buffCurrLen);
                    result.resize(width * height * 3 / 2);

                    memcpy_s(result.data(), result.size(), byteBuffer, width * height);
                    memcpy_s(result.data() + width * height, result.size() - width * height, byteBuffer + (width / 16 + ((width % 16) > 0)) * 16 * (height / 16 + ((height % 16) > 0)) * 16, width * height / 2);

                    SAFE_RELEASE(buf);
                }

                SAFE_RELEASE(pH264DecodeOutSample);
            }
        }
    } while (dataLength > 0);

    return result;
}
