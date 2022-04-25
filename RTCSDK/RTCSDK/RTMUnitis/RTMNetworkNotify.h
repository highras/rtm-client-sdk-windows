#pragma once
#include <wrl/client.h>
#include <netlistmgr.h>
#include <functional>
#include <mutex>
#include <atomic>
#include <atlbase.h>

using namespace Microsoft::WRL;
class RTMNetworkNotify : public INetworkListManagerEvents
{
public:
	RTMNetworkNotify(void);
	virtual ~RTMNetworkNotify(void);

public:
	static void initNetwork(RTMNetworkNotify* pNetNotify);
	static void unInitNetwork();

public:
	void setNetworkStatusCallback(std::function<void(bool status)> cb);
	bool getNetworkStatus()
	{
		return networkStatus.load(std::memory_order_relaxed);
	}

private:
	STDMETHODIMP QueryInterface(REFIID riid, void** pIFace);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP ConnectivityChanged(NLM_CONNECTIVITY NewConnectivity);

private:
	ULONG lRefCnt = 0;
	std::atomic<bool> networkStatus = { true };
	std::function<void(bool status)> networkStatusCallback;

	static DWORD dwCookie;
	static IConnectionPoint* pCp;
};


