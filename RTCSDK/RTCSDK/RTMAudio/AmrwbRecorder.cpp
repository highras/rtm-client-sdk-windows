#include "AmrwbRecorder.h"
#include <voiceConverter.h>

void AmrwbRecorder::Stop(std::string& audioData)
{
	WaveRecorder::Stop(audioData);

	int status = 0;
	int amrsize = 0;
	char* amrdata = convert_wav_to_amrwb((char*)audioData.data(), audioData.length(), status, amrsize);
	if (amrdata)
		audioData.assign(amrdata, amrsize); 
}
