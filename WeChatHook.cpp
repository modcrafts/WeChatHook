#include "pch.h"
#include <windows.h>
#include <stdio.h>
#include <iostream> 
using namespace std;

#define HOOK_LEN 5

BYTE backupCode[HOOK_LEN] { 0 };

VOID MsgForward(DWORD msg)
{
    msg = *((DWORD*)*((DWORD*)msg));

    LPVOID msgFrom = (LPVOID)(msg + 0x48);
    LPVOID msgContent = (LPVOID)(msg + 0x70);
    if (msgContent == NULL) msgContent = msgFrom;
    LPVOID msgFrom2 = (LPVOID)(msg + 0x170);
    LPVOID msgKey = (LPVOID)(msg + 0x184);

    MessageBox(NULL, *(LPCWSTR*)msgFrom, L"来源", 0);
    MessageBox(NULL, *(LPCWSTR*)msgContent, L"内容", 0);
    MessageBox(NULL, *(LPCWSTR*)msgFrom2, L"来源2", 0);
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

    MsgForward(cEsp);

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
