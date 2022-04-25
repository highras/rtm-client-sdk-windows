#include "OpenH264Decoder.h"
#include <libyuv.h>

OpenH264Decoder::OpenH264Decoder(int w,int h):
	width(w),
	height(h)
{
	WelsCreateDecoder(&decoder);
	SDecodingParam param = {};
	param.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_AVC;
	//param.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_SVC;
	param.uiTargetDqLayer = 255;
	param.eEcActiveIdc = ERROR_CON_SLICE_COPY;
	auto ret = decoder->Initialize(&param);
	if (ret != 0)
	{
		printf("error initialize open h264 decoder!\n");
	}
}

OpenH264Decoder::~OpenH264Decoder()
{
	WelsDestroyDecoder(decoder);
	decoder = nullptr;
}

vector<unsigned char> OpenH264Decoder::Decode(unsigned char* srcData, int srcLen)
{
	static unsigned char* dstData[3] = {};
	SBufferInfo dstBufferInfo = {};
	int ret = decoder->DecodeFrameNoDelay(srcData, srcLen, dstData, &dstBufferInfo);
	vector<unsigned char> result;

	if (ret != 0)
	{
		return result;
	}
	if (dstBufferInfo.iBufferStatus == 1)
	{
		result.resize(width * height * 3 / 2);
		if (width == dstBufferInfo.UsrData.sSystemBuffer.iWidth)
		{
			for (int i = 0; i != dstBufferInfo.UsrData.sSystemBuffer.iHeight; i++)
			{
				memcpy_s(result.data() + i * dstBufferInfo.UsrData.sSystemBuffer.iWidth,
					dstBufferInfo.UsrData.sSystemBuffer.iWidth,
					dstBufferInfo.pDst[0] + i * dstBufferInfo.UsrData.sSystemBuffer.iStride[0],
					dstBufferInfo.UsrData.sSystemBuffer.iWidth);
			}
			for (int i = 0; i != dstBufferInfo.UsrData.sSystemBuffer.iHeight / 2; i++)
			{
				memcpy_s(result.data() + dstBufferInfo.UsrData.sSystemBuffer.iWidth * dstBufferInfo.UsrData.sSystemBuffer.iHeight + i * dstBufferInfo.UsrData.sSystemBuffer.iWidth / 2,
					dstBufferInfo.UsrData.sSystemBuffer.iWidth / 2,
					dstBufferInfo.pDst[1] + i * dstBufferInfo.UsrData.sSystemBuffer.iStride[1],
					dstBufferInfo.UsrData.sSystemBuffer.iWidth / 2);

				memcpy_s(result.data() + dstBufferInfo.UsrData.sSystemBuffer.iWidth * dstBufferInfo.UsrData.sSystemBuffer.iHeight * 5 / 4 + i * dstBufferInfo.UsrData.sSystemBuffer.iWidth / 2,
					dstBufferInfo.UsrData.sSystemBuffer.iWidth / 2,
					dstBufferInfo.pDst[2] + i * dstBufferInfo.UsrData.sSystemBuffer.iStride[1],
					dstBufferInfo.UsrData.sSystemBuffer.iWidth / 2);
			}
		}
		else 
		{
			libyuv::I420Scale(dstBufferInfo.pDst[0], dstBufferInfo.UsrData.sSystemBuffer.iStride[0], 
				dstBufferInfo.pDst[1], dstBufferInfo.UsrData.sSystemBuffer.iStride[1],
				dstBufferInfo.pDst[2], dstBufferInfo.UsrData.sSystemBuffer.iStride[1],
				dstBufferInfo.UsrData.sSystemBuffer.iWidth, dstBufferInfo.UsrData.sSystemBuffer.iHeight,
				result.data(), width, result.data() + width * height, width / 2, result.data() + width * height * 5 / 4, width / 2,
				width,height, libyuv::kFilterLinear);
		}
	}

	return result;
}
