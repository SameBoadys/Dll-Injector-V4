#pragma once

#include "framework.h"
#include <TlHelp32.h>
#include <string>
#include <strsafe.h>

class DllHook
{
public:
	static void WINAPI SetHook();
	static void WINAPI UnSetHook();
	static BOOL WINAPI LoadLib(DWORD dwProcessId, LPWSTR lpszLibName);
	static BOOL WINAPI FreeLib(DWORD dwProcessId, LPTSTR lpszLibName);
	static DWORD GetProcessId(LPCWSTR lpName, std::wstring& errMsg);
	static void ShowError(DWORD dwErrNo, LPTSTR lpszFunction);
private:
};

