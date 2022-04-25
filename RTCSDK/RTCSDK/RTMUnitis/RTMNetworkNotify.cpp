#include "RTMNetworkNotify.h"
#include <iostream>


DWORD RTMNetworkNotify::dwCookie = 0;
IConnectionPoint* RTMNetworkNotify::pCp;
RTMNetworkNotify::RTMNetworkNotify(void) : lRefCnt(0)
{
}

RTMNetworkNotify::~RTMNetworkNotify(void)
{
}


void RTMNetworkNotify::setNetworkStatusCallback(std::function<void(bool status)> cb)
{
	networkStatusCallback = cb;
}

void RTMNetworkNotify::initNetwork(RTMNetworkNotify* pNetNotify)
{
	if (dwCookie != 0)
		return;

	ComPtr<INetworkListManager> pNLM;
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	hr = CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pNLM));
	if (SUCCEEDED(hr))
	{
		VARIANT_BOOL IsConnect = VARIANT_FALSE;
		hr = pNLM->get_IsConnectedToInternet(&IsConnect);
		if (SUCCEEDED(hr))
		{
			ComPtr<IConnectionPointContainer> pCpc;
			hr = pNLM->QueryInterface(IID_PPV_ARGS(&pCpc));
			if (SUCCEEDED(hr))
			{
				hr = pCpc->FindConnectionPoint(IID_INetworkListManagerEvents, &pCp);
				if (SUCCEEDED(hr))
				{
					hr = pCp->Advise((IUnknown*)pNetNotify, &dwCookie);
				}
				else
				{
#ifdef _DEBUG
					std::cout << "[initNetwork] Advise , error:" << hr << std::endl;
#endif // _DEBUG
				}
			}
			else
			{
#ifdef _DEBUG
				std::cout << "[initNetwork] QueryInterface , error:" << hr << std::endl;
#endif // _DEBUG
			}
		}
		else
		{
#ifdef _DEBUG
			std::cout << "[initNetwork] network is disconnected" << std::endl;
#endif // _DEBUG
		}
	}
	else
	{
#ifdef _DEBUG
		std::cout << "[initNetwork] CoCreateInstance fail, error:" << hr << std::endl;
#endif // _DEBUG
	}
}

void RTMNetworkNotify::unInitNetwork()
{
	if (dwCookie)
	{
		HRESULT hr = pCp->Unadvise(dwCookie);
		dwCookie = 0;
	}
	CoUninitialize();
}


STDMETHODIMP RTMNetworkNotify::QueryInterface(REFIID refIID, void** pIFace)
{
	*pIFace = NULL;
	if (refIID == IID_IUnknown || refIID == __uuidof(INetworkListManagerEvents))
	{
		*pIFace = static_cast<INetworkListManagerEvents*>
			(static_cast<IUnknown*>(this));
		((IUnknown*)*pIFace)->AddRef();
	}
	if (*pIFace == NULL)
	{
		return E_NOINTERFACE;
	}

	return S_OK;
}

STDMETHODIMP_(ULONG) RTMNetworkNotify::AddRef()
{
	return (ULONG)InterlockedIncrement(&lRefCnt);
}

STDMETHODIMP_(ULONG) RTMNetworkNotify::Release()
{
	LONG Result = InterlockedDecrement(&lRefCnt);
	if (Result == 0)
		delete this;
	return (ULONG)Result;
}

STDMETHODIMP  RTMNetworkNotify::ConnectivityChanged(NLM_CONNECTIVITY NewConnectivity)
{
	if ((NewConnectivity & NLM_CONNECTIVITY_IPV4_INTERNET) || (NewConnectivity & NLM_CONNECTIVITY_IPV6_INTERNET))
	{
		networkStatusCallback(true);
		networkStatus.store(true, std::memory_order_relaxed);
#ifdef _DEBUG
		std::cout << "[ConnectivityChanged] ok" << std::endl;
#endif 
	}
	else
	{
		networkStatusCallback(false);
		networkStatus.store(false, std::memory_order_relaxed);
#ifdef _DEBUG
		std::cout << "[ConnectivityChanged] false" << std::endl;
#endif
	}

	return S_OK;
}
