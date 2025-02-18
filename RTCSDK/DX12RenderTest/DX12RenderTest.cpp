﻿// DX12RenderTest.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "DX12RenderTest.h"
#include "D3D12HelloTexture.h"

#include <unordered_map>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

std::unordered_map<HWND,D3D12HelloTexture*> dxins;
std::thread *renderthread;
bool running = true;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
#ifdef _DEBUG
    AllocConsole();
    FILE* a;
    freopen_s(&a, "CONOUT$", "w", stdout);
    freopen_s(&a, "CONOUT$", "w", stderr);
#endif

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DX12RENDERTEST, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DX12RENDERTEST));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DX12RENDERTEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DX12RENDERTEST);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    //WNDCLASSEXW wcexc;

    WNDCLASSEX wcexc = { 0 };
    wcexc.cbSize = sizeof(WNDCLASSEX);
    wcexc.style = CS_HREDRAW | CS_VREDRAW;
    wcexc.lpfnWndProc = 
        [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT {
        switch (message)
        {
        case WM_PAINT:
        {
            RECT rect;
            rect.left = 0;
            rect.right = 960;
            rect.top = 0;
            rect.bottom = 540;
            printf("childwindow hwnd:%lld DXrender:%p\n", (__int64)hWnd, dxins[hWnd]);
            //if (dxins[hWnd])
            //{
            //    dxins[hWnd]->OnUpdate();
            //    dxins[hWnd]->OnRender();
            //}
            ValidateRect(hWnd, &rect);
            break;
        }
        break;
        case WM_DESTROY:
        {
            if (dxins[hWnd])
            {
                dxins[hWnd]->OnDestroy();
            }
            dxins.erase(hWnd);
            PostQuitMessage(0);
            break;
        }
        case WM_KEYDOWN:
        {
            if (dxins[hWnd])
            {
                dxins[hWnd]->OnKeyDown(static_cast<UINT8>(wParam));
                break;
            }
        }
        case WM_KEYUP:
        {
            if (dxins[hWnd])
            {
                dxins[hWnd]->OnKeyUp(static_cast<UINT8>(wParam));
            }
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    };
    wcexc.hInstance = hInstance;
    wcexc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcexc.lpszClassName = L"RenderWindow";

    WORD res = RegisterClassExW(&wcexc);

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, 1920, 1080, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   const int width = 240;
   const int height = 320;
   for (int i = 0; i != 1; i++)
   {
       HWND hWndc = CreateWindow(L"RenderWindow", NULL, WS_CHILD | WS_VISIBLE,
           (width+10) * (i%(1920/(width+10))), (height+10) * (i/ (1080 / (height + 10))), width, height, hWnd, nullptr, hInstance, nullptr);
       D3D12HelloTexture* d3d12tex = new D3D12HelloTexture(width, height, L"", hWndc);
       dxins[hWndc] = d3d12tex;
       dxins[hWndc]->OnInit();
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   renderthread = new std::thread([]() {
       while (running)
       {
           for (auto& item : dxins)
           {
               item.second->OnUpdate();
               item.second->OnRender();
           }
           Sleep(10);
       }
       });

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        RECT rect;
        rect.left = 0;
        rect.right = 1920;
        rect.top = 0;
        rect.bottom = 1080;
        ValidateRect(hWnd, &rect);
        //for (auto item : dxins)
        //{
        //    if (item.second)
        //    {
        //        item.second->OnUpdate();
        //        item.second->OnRender();
        //    }
        //}
        break;
    case WM_DESTROY: 
    {
        running = false;
        renderthread->join();
        PostQuitMessage(0);
        break;
    }
    case WM_KEYDOWN:
    {
        break;
    }
    case WM_KEYUP:
    {
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
