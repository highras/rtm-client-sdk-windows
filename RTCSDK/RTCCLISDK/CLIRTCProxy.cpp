#include "CLIRTCProxy.h"
//#include <msclr\marshal_cppstd.h>
#include <RTCProxy.h>

namespace RTCCLISDK
{
    CLIRTCProxy::CLIRTCProxy(String^ rtmhost, unsigned short rtmport, CLIRTCEventHandler^ handler, String^ rtchost, unsigned short rtcport)
    {
        //internalHandler = new shared_ptr<BridgeRTCEventHandler>(new BridgeRTCEventHandler(handler));
        //implement = new RTCProxy(msclr::interop::marshal_as<std::string>(rtmhost), rtmport, *internalHandler, msclr::interop::marshal_as<std::string>(rtchost), rtcport);
    }

    CLIRTCProxy::~CLIRTCProxy()
    {
        //delete implement;
        //delete internalHandler;
    }
}