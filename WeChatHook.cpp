#include "pch.h"
#include <windows.h>
#include <stdio.h>
#include <iostream> 
#include "resource.h"
#include "json/json.h"
#include <atlconv.h>
#include <atlstr.h>
using namespace std;

#define HOOK_LEN 5
VOID SendCallbackMsg(LPVOID msg);

BYTE backupCode[HOOK_LEN] { 0 };
DWORD dlloffset = 0x0;
VOID MsgForward(DWORD msg)
{
    try {
        msg = *((DWORD*)msg);

        LPVOID msgFrom = (LPVOID)(msg + 0x48);
        LPVOID msgContent;
        LPVOID msgKey;
        LPVOID msgFrom2;
        /*if (*(LPCWSTR*)msgFrom == NULL) {
            msgFrom = (LPVOID)(msg - 0x240);
            msgContent = (LPVOID)(msg - 0x218);
            if (msgContent == NULL) msgContent = msgFrom;
            msgKey = (LPVOID)(msg - 0x104);
            msgFrom2 = msgFrom;
         }
        else {*/
            msgContent = (LPVOID)(msg + 0x70);
            if (msgContent == NULL) msgContent = msgFrom;
            msgFrom2 = (LPVOID)(msg + 0x170);
            msgKey = (LPVOID)(msg + 0x184);
        //}
        /*
        SetDlgItemText(hDlg, FROM, *(LPCWSTR*)msgFrom);
        SetDlgItemText(hDlg, CONTENT, *(LPCWSTR*)msgContent);
        SetDlgItemText(hDlg, SENDER, *(LPCWSTR*)msgFrom2);
        SetDlgItemText(hDlg, KEY, *(LPCWSTR*)msgKey);
        Json::Value content;*/
        /*
        Socket_sendstr((char*)(LPCWSTR)msgContent);
        return;
        content["from"] = (char*)strFrom.GetBuffer();
        content["content"] = (char*)strContent.GetBuffer();
        if(!Socket_send(content)) MessageBox(NULL, L"发送失败", L"错误", 0);;
        */
        SendCallbackMsg(msgContent);
    }
    catch (const std::exception&) {
        MessageBox(NULL, L"发生未预料的错误", L"错误", 0);
    }
}


DWORD GetWechatWinAdd()
{
    return (DWORD)LoadLibrary(L"WeChatWin.dll");
}

DWORD cEax = 0;
DWORD cEcx = 0;
DWORD cEdx = 0;
DWORD cEbx = 0;
DWORD cEsp = 0;
DWORD cEbp = 0;
DWORD cEsi = 0;
DWORD cEdi = 0;
DWORD retCallAdd = 0;
VOID __declspec(naked) MsgProcess()
{
    __asm {
        mov cEax, eax
        mov cEcx, ecx
        mov cEdx, edx
        mov cEbx, ebx
        mov cEsp, esp
        mov cEbp, ebp
        mov cEsi, esi
        mov cEdi, edi
    }

    MsgForward(cEbx);

    retCallAdd = GetWechatWinAdd() + 0x37CD18;

    __asm {
        mov  eax, cEax
        mov  ecx, cEcx
        mov  edx, cEdx
        mov  ebx, cEbx
        mov  esp, cEsp
        mov  ebp, cEbp
        mov  esi, cEsi
        mov  edi, cEdi
    }

    __asm {
        call eax
        lea ecx, dword ptr ss : [ebp - 0x18]
        jmp retCallAdd
    }
}

VOID HookMessageCall(DWORD offset, LPVOID func)
{
    dlloffset = offset;
    //0x37CD13
    DWORD hookPoint = GetWechatWinAdd() + offset;
    BYTE jmpCode[HOOK_LEN] = { 0 };
    jmpCode[0] = 0xE9;
    *(DWORD*)&jmpCode[1] = (DWORD)func - hookPoint - HOOK_LEN;
    
    HANDLE wx_handle = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
    if (ReadProcessMemory(wx_handle, (LPCVOID)hookPoint, backupCode, HOOK_LEN, NULL) == 0) {
        MessageBox(NULL, L"内存读取失败", L"错误", 0);
        return;
    }
    if (WriteProcessMemory(wx_handle, (LPVOID)hookPoint, jmpCode, HOOK_LEN, NULL) == 0) {
        MessageBox(NULL, L"内存写入失败", L"错误", 0);
        return;
    }
}

VOID SendCallbackMsg(LPVOID msg) {// 接收消息进程名
        CString strOtherWndTitle = L"支付监听回调";
        // 获取接收消息进程句柄
        HWND hOtherWnd = ::FindWindow(NULL, strOtherWndTitle.GetBuffer(0));
        if (hOtherWnd != NULL && ::IsWindow(hOtherWnd))
        {
            int nLen = WideCharToMultiByte(CP_ACP, 0, *(LPCWSTR*)msg, -1, NULL, 0, NULL, NULL);
            char* s = new char[nLen + 1];
            WideCharToMultiByte(CP_ACP, 0, *(LPCWSTR*)msg, -1, s, nLen, NULL, NULL);
            COPYDATASTRUCT CopyData;
            CopyData.dwData = 0;
            CopyData.cbData = strlen(s) + 1;
            CopyData.lpData = s;
            SendMessage(hOtherWnd, WM_COPYDATA, (WPARAM)GetCurrentProcess(), (LPARAM)&CopyData);
            delete s;
        }
        else {
            MessageBox(NULL, L"句柄无效,请确认主程序是否启动\n监听模块将自动卸载", L"错误", 0);
            DWORD hookPoint = GetWechatWinAdd() + dlloffset;
            HANDLE wx_handle = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
            if (WriteProcessMemory(wx_handle, (LPVOID)hookPoint, backupCode, HOOK_LEN, NULL) == 0) {
                MessageBox(NULL, L"内存写入失败", L"错误", 0);
                return;
            }
        }
}