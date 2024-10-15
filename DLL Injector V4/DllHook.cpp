#include "DllHook.h"
void WINAPI DllHook::SetHook()
{
	std::wstring szPath;
	LPSETHOOK lproc;
	HINSTANCE hDll;
	BOOL bRet;
	PROCESS_INFORMATION info;
	STARTUPINFO start;
	memset(&start, 0, sizeof(start));
	szPath = _T("C://Users//YOURNAME//source//repos//Ldll//x64//Debug//Ldll.dll");
	hDll = LoadLibraryW(szPath.c_str());
	if (hDll != NULL)
	{
		lproc = (LPSETHOOK)GetProcAddress(hDll, "SetHook");
		if (lproc != NULL)
		{
			std::wstring str = _T("C://Windows//System32//notepad.exe");
			bRet = CreateProcess(NULL,
				const_cast<wchar_t*>(str.c_str()),
				NULL,
				NULL,
				TRUE,
				0,
				NULL,
				NULL,
				&start,
				&info);
			if (bRet != 0)
			{
				if (((*lproc)(info.dwThreadId)) == false)
				{
					std::wstring message = L"Sethook";
					DllHook::ShowError(GetLastError(), const_cast<LPTSTR>(message.c_str()));
				}
			}
			else
			{
				std::wstring message = L"CreateProcess";
				DllHook::ShowError(GetLastError(), const_cast<LPTSTR>(message.c_str()));
			}
		}
	}
}
void WINAPI DllHook::UnSetHook()
{
	 std::wstring szPath;
	 LPSETHOOK lproc;
	 HINSTANCE hDll;

	 szPath = _T("C://Users//YOURNAME//source//repos//Ldll//x64//Debug//Ldll.dll");
	 hDll = LoadLibrary(szPath.c_str());
	 if (hDll != NULL)
	 {
		 lproc = (LPSETHOOK)GetProcAddress(hDll, "SetHook");
		 if (lproc != NULL)
			 (*lproc)(0);
	 }
}
BOOL WINAPI DllHook::LoadLib(DWORD dwProcessId, LPWSTR lpszLibName)
{
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	LPWSTR lpszRemoteFile = NULL;
	hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, dwProcessId);
	if (hProcess == NULL)
	{
		ShowError(GetLastError(), const_cast<LPTSTR>(L"OpenProcess"));
		return FALSE;
	}

	lpszRemoteFile = (LPWSTR)VirtualAllocEx(hProcess, NULL, sizeof(WCHAR) * lstrlenW(lpszLibName) + 1, MEM_COMMIT, PAGE_READWRITE);
	if (lpszRemoteFile == NULL)
	{
		ShowError(GetLastError(), const_cast<LPTSTR>(L"VirtualAllocEx"));
		return FALSE;
	}
	if (!WriteProcessMemory(hProcess, lpszRemoteFile, (PVOID)lpszLibName, sizeof(WCHAR) * lstrlenW(lpszLibName) + 1, NULL))
	{
		ShowError(GetLastError(), const_cast<LPTSTR>(L"WriteProcessMemory"));
		return FALSE;
	}
	PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "LoadLibraryW");
	if (pfnThreadRtn == NULL)
	{
		ShowError(GetLastError(), const_cast<LPTSTR>(L"GetProcAddress"));
		return FALSE;
	}
	hThread = CreateRemoteThread(hProcess,
		NULL,
		0,
		pfnThreadRtn, 
		lpszRemoteFile,
		0,
		NULL);
	if (hThread == NULL)
	{
		ShowError(GetLastError(), const_cast<LPTSTR>(L"CreateRemoteThread"));
		return FALSE;
	}
	WaitForSingleObject(hThread, INFINITE);
	VirtualFreeEx(hProcess, lpszRemoteFile, 0, MEM_RELEASE);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return TRUE;
}
BOOL WINAPI DllHook::FreeLib(DWORD dwProcessId, LPTSTR lpszLibName)
{
	HANDLE hProcess = NULL,
	       hThread = NULL,
	       hthSnapshot = NULL;
	MODULEENTRY32 hMod = {sizeof(hMod)};
	BOOL bFound = FALSE;
	hthSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,
	                                       dwProcessId);
	if (hthSnapshot == NULL)
	{
		ShowError(GetLastError(), const_cast<LPTSTR>(L"CreateToolhelp32Snapshot"));
		return FALSE;
	}
	BOOL bMoreMods = Module32First(hthSnapshot, &hMod);
	if (bMoreMods == FALSE)
	{
		ShowError(GetLastError(), const_cast<LPTSTR>(L"Module32First"));
		return FALSE;
	}
	for (; bMoreMods; bMoreMods = Module32Next(hthSnapshot, &hMod))
	{
		if ((wcscmp(hMod.szExePath, lpszLibName) == 0) ||
			(wcscmp(hMod.szModule, lpszLibName) == 0))
		{
			bFound = TRUE;
			break;
		}
	}
	if (!bFound)
	{
		MessageBox(nullptr, L"Ä£¿é²»´æÔÚ", L"", MB_OK);
		CloseHandle(hthSnapshot);
		return FALSE;
	}
	hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE,
	                       FALSE, dwProcessId);
	if (hProcess == NULL)
	{
		ShowError(GetLastError(), const_cast<LPTSTR>(L"OpenProcess"));
		return FALSE;
	}
	PTHREAD_START_ROUTINE pfnThreadRtn =
		(PTHREAD_START_ROUTINE)GetProcAddress(
			GetModuleHandle(L"Kernel32.dll"), "FreeLibrary");
	if (pfnThreadRtn == NULL)
	{
		ShowError(GetLastError(), const_cast<LPTSTR>(L"GetProcAddress"));
		return FALSE;
	}
	hThread = CreateRemoteThread(hProcess,
	                             NULL,
	                             0,
	                             pfnThreadRtn,
	                             hMod.modBaseAddr,
	                             0,
	                             NULL);
	if (hThread == NULL)
	{
		ShowError(GetLastError(), const_cast<LPTSTR>(L"CreateRemoteThread"));
		return FALSE;
	}
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hthSnapshot);
	CloseHandle(hProcess);
	return TRUE;
}
DWORD DllHook::GetProcessId(LPCWSTR lpName, std::wstring& errMsg)
{
	DWORD dwPid = 0;
	HANDLE hProcess = NULL;
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		errMsg = L"Error: CreateToolhelp32Snapshot (of processes)";
		return 0;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hProcessSnap, &pe32))
	{
		errMsg = L"Error: Process32First";
		CloseHandle(hProcessSnap);
		return 0;
	}
	int namelen = 200;
	char name[201] = { 0 };
	do
	{
		if (!wcscmp(pe32.szExeFile, lpName))
		{
			dwPid = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hProcessSnap, &pe32));
	CloseHandle(hProcessSnap);
	return dwPid;
}
void DllHook::ShowError(DWORD dwErrNo, LPTSTR lpszFunction)
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErrNo,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);


	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(
			TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dwErrNo, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}