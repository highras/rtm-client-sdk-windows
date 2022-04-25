#ifndef voiceConverter_h
#define voiceConverter_h

extern "C"
{
    char* convert_wav_to_amrwb(char* wavSrc, int wavSrcSize, int& status, int& amrSize);
    char* convert_amrwb_to_wav(char* amrSrc, int amrSrcSize, int& status, int& wavSize);
    void free_memory(char* ptr);
}

#endif
