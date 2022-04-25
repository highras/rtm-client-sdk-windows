#include "RTMAudio.h"
#include "../RTMClient.h"


bool RTMClient::StartRecordAudio()
{
	return rtmWaveRecorder.Start();
}

void RTMClient::StopRecordAudio(RTMAudio& recordata)
{
	rtmWaveRecorder.Stop(recordata.audioData);
	recordata.audioDur = rtmWaveRecorder.GetDuration();
	recordata.audioSampleRate = rtmWaveRecorder.GetSampleRate();
}
	 

bool RTMClient::StartPlayAudio(RTMAudio recordata)
{
	return rtmWavePlayer.Start((void*)recordata.audioData.c_str(), recordata.audioData.length());
}

bool RTMClient::StopPlayAudio()
{
	return rtmWavePlayer.Stop();
}