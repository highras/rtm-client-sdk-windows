#pragma once

#include <stdint.h>
#include <stddef.h>
#include <Windows.h>

class VideoReader
{
#pragma pack(4)
    struct VideoHeader{
        int32_t captureLevel;
        int32_t rotation;
        int32_t cameraFacing;
        int64_t timestamp;
        int32_t spsLength;
        int32_t ppsLength;
        int32_t dataLength;
    };
#pragma pack()
    HANDLE fd;
public:
    VideoReader(const char* path);
    ~VideoReader();
    void Read(char* data[3], size_t length[3], int32_t &captureLevel, int32_t &rotation, int32_t &cameraFacing, int64_t &timestamp, int32_t &spsLength, int32_t &ppsLength, int32_t &dataLength);
};