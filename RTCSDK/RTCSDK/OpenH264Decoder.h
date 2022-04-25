#pragma once
#include <codec_api.h>
#include <stdio.h>
#include <vector>

using namespace std;

class OpenH264Decoder
{
	ISVCDecoder *decoder;
	int width;
	int height;
	bool inited;
public:
	OpenH264Decoder(int w,int h);
	~OpenH264Decoder();
	vector<unsigned char> Decode(unsigned char* srcData, int srcLen);

	bool IsInited();
	void SetInited(bool t);
};

