#pragma once
#include "CLIRTCEventHandler.h"
#include <memory>

using namespace System;

class RTCProxy;

namespace RTCCLISDK
{
	public ref class CLIRTCProxy
	{
		RTCProxy* implement;
		shared_ptr<BridgeRTCEventHandler>* internalHandler;
	public:
		CLIRTCProxy(String^ rtmhost, unsigned short rtmport, CLIRTCEventHandler^ handler, String^ rtchost, unsigned short rtcport);
		~CLIRTCProxy();
	};
}