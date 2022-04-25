#pragma once

#include <list>
#include <vector>

#include <stdio.h>
#include <tchar.h>
#include <evr.h>
#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <wmcodecdsp.h>
#include <iostream>
#include "OpenH264Decoder.h"
#include "VideoReader.h"

class MFTDecoder
{
    IMFSourceResolver* pSourceResolver = NULL;
    IUnknown* uSource = NULL;
    IMFMediaSource* mediaFileSource = NULL;
    IMFAttributes* pVideoReaderAttributes = NULL;
    IMFSourceReader* pSourceReader = NULL;
    IMFMediaType* pFileVideoMediaType = NULL;
    IUnknown* spDecTransformUnk = NULL;
    IMFTransform* pDecoderTransform = NULL; // This is H264 Decoder MFT.
    IMFMediaType* pDecInputMediaType = NULL, * pDecOutputMediaType = NULL;
    MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;
    DWORD mftStatus = 0;

    IMFSample* pVideoSample = NULL, * pCopyVideoSample = NULL, * pH264DecodeOutSample = NULL;
    DWORD streamIndex, flags;
    LONGLONG llVideoTimeStamp = 0, llSampleDuration = 0;
    int sampleCount = 0;
    DWORD sampleFlags = 0;
    bool h264DecodeTransformFlushed = false;

    OpenH264Decoder openh264;
    VideoReader reader;

public:
    int width;
    int height;
	MFTDecoder();
	~MFTDecoder();
    void Init(const WCHAR* FilePath);
    std::vector<unsigned char> Decode();
    std::vector<unsigned char> DecodeOpenH264();
    std::vector<unsigned char> DecodeTest();
};

