// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "WeChatHook.h"
#include "resource.h"
#include "socketHelper.h"
#include "atlstr.h"
#include <atlbase.h>
#include <fstream>
#include <cassert>
#include <string>
#include <iostream>

#define offset 0x37CD13
using namespace std;

VOID ShowUI(HMODULE hModule);
INT_PTR CALLBACK DialogProc(_In_ HWND   hwndDlg, _In_ UINT   uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
HMODULE hModulea;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		hModulea = hModule;
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ShowUI, hModule, NULL, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

VOID ShowUI(HMODULE hModule)
{
	DialogBox(hModule, MAKEINTRESOURCE(MAIN), NULL, &DialogProc);
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
			HookMessageCall(hwndDlg, offset, MsgProcess);
			int port = 0;
			{
				char szPath[MAX_PATH];
				GetModuleFileNameA(hModulea, szPath, MAX_PATH); 
				char* lpStr1;
				lpStr1 = szPath;
				PathRemoveFileSpecA(lpStr1);
				//MessageBox(hwndDlg, CA2W(lpStr1), L"路径", 0);
				string filename = lpStr1;
				filename = filename + "\\port";
				ifstream infile;
				infile.open(filename.data());   //将文件流对象与文件连接起来 
				assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行 

				string s;
				getline(infile, s);
				infile.close();
				port = stol(s);
			}
			if(!Socket_startup(port)) MessageBox(hwndDlg, L"启动失败", L"提示", 0);
			//MessageBox(hwndDlg, L"已HOOK", L"提示", 0);
		}
		break;
	default:
		break;
	}
	return FALSE;
}
