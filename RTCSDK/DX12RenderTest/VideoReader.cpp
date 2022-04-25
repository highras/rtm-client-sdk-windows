#include "VideoReader.h"

#include <stdio.h>

VideoReader::VideoReader(const char* path)
{
    fd = CreateFileA(path, GENERIC_READ, FILE_SHARE_DELETE,NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
}

VideoReader::~VideoReader()
{
    CloseHandle(fd);
}

void VideoReader::Read(char* data[3], size_t length[3], int32_t &captureLevel, int32_t &rotation, int32_t &cameraFacing, int64_t &timestamp, int32_t &spsLength, int32_t &ppsLength, int32_t &dataLength)
{
    rotation = -1;
    cameraFacing = -1;
    timestamp = -1;
    spsLength = 0;
    ppsLength = 0;
    dataLength = 0;
    if (fd == NULL)
        return;

    VideoHeader header;
    DWORD len; 
    if (!ReadFile(fd, (char*)&header, sizeof(VideoHeader), &len, NULL))
        return;
    if (len != sizeof(VideoHeader))
        return;

    captureLevel = header.captureLevel;
    rotation = header.rotation;
    cameraFacing = header.cameraFacing;
    timestamp = header.timestamp;
    spsLength = header.spsLength;
    ppsLength = header.ppsLength;
    dataLength = header.dataLength;

    if (header.spsLength > 0){
        if (!ReadFile(fd, data[0], header.spsLength < length[0] ? header.spsLength : length[0], &len, NULL))
            ; printf("sps %d readed\n", len);
    }
    if (header.ppsLength > 0){
        if (!ReadFile(fd, data[1], header.ppsLength < length[1] ? header.ppsLength : length[1], &len, NULL))
            ; printf("pps %d readed\n", len);
    }
    if (header.dataLength > 0){
        if (!ReadFile(fd, data[2], header.dataLength < length[2] ? header.dataLength : length[2], &len, NULL))
            ; printf("data %d readed\n", len);
    }
    
}