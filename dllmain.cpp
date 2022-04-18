// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "WeChatHook.h"
#include "resource.h"
#include "atlstr.h"
#include <atlbase.h>
#include <fstream>
#include <cassert>
#include <string>
#include <iostream>

#define offset 0x37CD13
using namespace std;

VOID Hook(HMODULE hModule);
INT_PTR CALLBACK DialogProc(_In_ HWND   hwndDlg, _In_ UINT   uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		//hModulea = hModule;
		Hook(hModule);
        //CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Hook, hModule, NULL, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

VOID Hook(HMODULE hModule)
{
	HookMessageCall(offset, MsgProcess);
	//MessageBox(NULL, L"已HOOK", L"提示", 0);
}

INT_PTR CALLBACK DialogProc(
	_In_ HWND   hwndDlg,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		break;
	case WM_COMMAND:
		if (wParam == HOOK) {
			HookMessageCall(offset, MsgProcess);
			//MessageBox(hwndDlg, L"已HOOK", L"提示", 0);
		}
		break;
	default:
		break;
	}
	return FALSE;
}
