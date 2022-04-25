#pragma once
#include "WaveRecorder.h"
class AmrwbRecorder : public WaveRecorder
{
public:
	void Stop(std::string& audioData) override;
};

