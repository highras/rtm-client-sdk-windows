// TestProject.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <netlistmgr.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;
using namespace std;

class NetWorkEvent : public INetworkConnectionEvents
{
private:
    LONG _ref;

public:
    NetWorkEvent();
    virtual HRESULT STDMETHODCALLTYPE NetworkConnectionConnectivityChanged(/* [in] */ GUID connectionId, /* [in] */ NLM_CONNECTIVITY newConnectivity);
    virtual HRESULT STDMETHODCALLTYPE NetworkConnectionPropertyChanged(/* [in] */ GUID connectionId, /* [in] */ NLM_CONNECTION_PROPERTY_CHANGE flags);
    STDMETHODIMP QueryInterface(REFIID refIID, void** pIFace);
    virtual ULONG __stdcall AddRef(void);
    virtual ULONG __stdcall Release(void);
};

NetWorkEvent::NetWorkEvent() {}
HRESULT NetWorkEvent::NetworkConnectionConnectivityChanged(GUID connectionId, NLM_CONNECTIVITY newConnectivity)
{
    cout << "event!" << endl;
    return S_OK;
}
HRESULT NetWorkEvent::NetworkConnectionPropertyChanged(GUID connectionId, NLM_CONNECTION_PROPERTY_CHANGE flags) 
{ 
    return S_OK; 
}

STDMETHODIMP NetWorkEvent::QueryInterface(REFIID refIID, void** pIFace)
{
    HRESULT hr = S_OK;
    *pIFace = NULL;
    if (IsEqualIID(refIID, IID_IUnknown))
    {
        *pIFace = (IUnknown*)this;
        ((IUnknown*)*pIFace)->AddRef();
    }
    else if (IsEqualIID(refIID, IID_INetworkConnectionEvents))
    {
        *pIFace = (INetworkConnectionEvents*)this;
        ((IUnknown*)*pIFace)->AddRef();
    }
    else
    {
        hr = E_NOINTERFACE;
    }
    return hr;
}

ULONG NetWorkEvent::AddRef(void) 
{ 
    return (ULONG)InterlockedIncrement(&_ref); 
}

ULONG NetWorkEvent::Release(void)
{
    LONG Result = InterlockedDecrement(&_ref);
    if (Result == 0)
        delete this;
    return (ULONG)Result;
}

class NetworkNotify : public /*INetworkConnectionEvents,*/ INetworkEvents
{
    LONG _ref;
public:
    //virtual HRESULT STDMETHODCALLTYPE NetworkConnectionConnectivityChanged(GUID connectionId, NLM_CONNECTIVITY newConnectivity) override
    //{
    //    printf("connectionid: {%x-%x-%x-%llx} NLM_CONNECTIVITY: 0x%x\r\n", connectionId.Data1, connectionId.Data2, connectionId.Data3, *(long long*)connectionId.Data4, newConnectivity);
    //    return S_OK;
    //}
    //virtual HRESULT STDMETHODCALLTYPE NetworkConnectionPropertyChanged(GUID connectionId, NLM_CONNECTION_PROPERTY_CHANGE flags) override
    //{
    //    return S_OK;
    //}
    
    virtual STDMETHOD_(ULONG, AddRef)() 
    { 
        return (ULONG)InterlockedIncrement(&_ref); 
    }

    virtual STDMETHOD_(ULONG, Release)() 
    {
        LONG Result = InterlockedDecrement(&_ref);
        if (Result == 0)
            delete this;
        return (ULONG)Result;
    }

    STDMETHODIMP QueryInterface(REFIID refIID, void** pIFace)
    {
        HRESULT hr = S_OK;
        *pIFace = NULL;
        if (IsEqualIID(refIID, IID_IUnknown))
        {
            *pIFace = (IUnknown*)this;
            ((IUnknown*)*pIFace)->AddRef();
        }
        else if (IsEqualIID(refIID, IID_INetworkEvents/*IID_INetworkConnectionEvents*/))
        {
            *pIFace = (/*INetworkConnectionEvents*/INetworkEvents*)this;
            ((IUnknown*)*pIFace)->AddRef();
        }
        else
        {
            hr = E_NOINTERFACE;
        }
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE NetworkAdded(
        /* [in] */ GUID networkId)
    {
        printf("network added! networkId: {%x-%x-%x-%llx} \r\n", networkId.Data1, networkId.Data2, networkId.Data3, *(long long*)networkId.Data4);
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE NetworkDeleted(
        /* [in] */ GUID networkId)
    {
        printf("network deleted! networkId: {%x-%x-%x-%llx} \r\n", networkId.Data1, networkId.Data2, networkId.Data3, *(long long*)networkId.Data4);
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE NetworkConnectivityChanged(
        /* [in] */ GUID networkId,
        /* [in] */ NLM_CONNECTIVITY newConnectivity)
    {
        printf("network connectivity changed! networkId: {%x-%x-%x-%llx} NLM_CONNECTIVITY: 0x%x\r\n", networkId.Data1, networkId.Data2, networkId.Data3, *(long long*)networkId.Data4, newConnectivity);
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE NetworkPropertyChanged(
        /* [in] */ GUID networkId,
        /* [in] */ NLM_NETWORK_PROPERTY_CHANGE flags)
    {
        printf("network property changed! networkId: {%x-%x-%x-%llx} NLM_NETWORK_PROPERTY_CHANGE: 0x%x\r\n", networkId.Data1, networkId.Data2, networkId.Data3, *(long long*)networkId.Data4, flags);
        return S_OK;
    }
};

int mainnetwork(void)
{
    ComPtr<INetworkListManager> pNLM;
    CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);
    HRESULT hr = CoCreateInstance(CLSID_NetworkListManager, NULL,
        CLSCTX_ALL, IID_PPV_ARGS(&pNLM));
    DWORD cookies = 0;
    NetworkNotify* eventcallback = new NetworkNotify();
    NetWorkEvent* ne = new NetWorkEvent();
    if (SUCCEEDED(hr))
    {
        ComPtr<IConnectionPointContainer> pCpc;
        hr = pNLM->QueryInterface(IID_PPV_ARGS(&pCpc));
        if (SUCCEEDED(hr))
        {
            ComPtr<IConnectionPoint> connectionPoint;
            hr = pCpc->FindConnectionPoint(IID_INetworkEvents, &connectionPoint);
            hr = connectionPoint->Advise(eventcallback, &cookies);
        }
    }
    std::cout << "Hello World!\n";
    while (true)
        Sleep(100);
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
