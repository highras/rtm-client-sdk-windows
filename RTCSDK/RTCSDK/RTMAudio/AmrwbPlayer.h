#pragma once
#include "WavePlayer.h"
class AmrwbPlayer : public WavePlayer
{
	function<void()> completeCallback;
	function<void()> startedCallback;

	char* wavdata=nullptr;
	int wavsize;
public:
	AmrwbPlayer();
	~AmrwbPlayer();
	bool Start(void* data, size_t len) override;
	void SetCompleteCallback(function<void()> callback) override;
	void SetStartedCallback(function<void()> callback) override;
};

