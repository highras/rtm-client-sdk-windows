#pragma once

#include <xstring>
struct RTMAudio
{
    std::string audioCodec = "amr-wb";
    std::string audioLang = "zh-CN";
    std::string audioData;  // Current is AMR-WB
    int64_t audioDur = 0;   // Duration in ms
    int32_t audioSampleRate = 16000;
};