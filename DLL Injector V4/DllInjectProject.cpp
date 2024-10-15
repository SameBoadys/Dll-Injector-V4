#include "framework.h"
#include "DllInjectProject.h"
#include <string>
#include <CommCtrl.h> //ListView_InsertItem用
#include <windowsx.h> // Edit_SetText用
#include "DllHook.h"
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


#define MAX_LOADSTRING 100

HINSTANCE hInst;
HWND hListView;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

void __fastcall Button1Click(HWND hWnd);
void __fastcall Button2Click(HWND hWnd);
void __fastcall btLookClick(HWND hWnd);
BOOL InitListViewColumns(HWND hWndListView);

bool SetPrivilege();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FALSE == SetPrivilege())
	{
		MessageBox(nullptr, L"提权失败", L"", MB_OK);
		return FALSE;
	}

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
	                          CW_USEDEFAULT, 0, 600, 500, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

void __fastcall Button1Click(HWND hWnd)
{
	std::wstring szPath =  _T("C://Users//lisongqian//source//repos//Ldll//x64//Debug//Ldll.dll");
	std::wstring errMsg = L"";
	WCHAR processName[50];
	SendMessage(GetDlgItem(hWnd, IDM_OUTBOX), WM_GETTEXT, sizeof(processName) / sizeof(wchar_t),
		(LPARAM)(LPCTSTR)processName);
	DWORD dwProcessId = DllHook::GetProcessId(processName, errMsg);
	DllHook::LoadLib(dwProcessId, const_cast<LPWSTR>(szPath.c_str()));
}

void __fastcall Button2Click(HWND hWnd)

{
	std::wstring szPath = _T("Ldll.dll");
	std::wstring errMsg = L"";
	WCHAR processName[50];
	SendMessage(GetDlgItem(hWnd, IDM_OUTBOX), WM_GETTEXT, sizeof(processName) / sizeof(wchar_t),
		(LPARAM)(LPCTSTR)processName);
	DWORD dwProcessId = DllHook::GetProcessId(processName, errMsg);
	DllHook::FreeLib(dwProcessId, const_cast<LPWSTR>(szPath.c_str()));
}

void __fastcall btLookClick(HWND hWnd)
{
	DWORD dwProcessId = 0;
	BOOL bRet;
	MODULEENTRY32 hMod = {sizeof(hMod)};
	HANDLE hthSnapshot = NULL;
	BOOL bMoreMods = FALSE;
	std::wstring errMsg = L"";
	WCHAR processName[50];
	SendMessage(GetDlgItem(hWnd, IDM_OUTBOX), WM_GETTEXT, sizeof(processName) / sizeof(wchar_t),
	            (LPARAM)(LPCTSTR)processName);
	dwProcessId = DllHook::GetProcessId(processName, errMsg);
	if (dwProcessId == 0)
	{
		DllHook::ShowError(GetLastError(), const_cast<LPTSTR>(errMsg.c_str()));
		return;
	}
	hthSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if (hthSnapshot == NULL)
	{
		DllHook::ShowError(GetLastError(), const_cast<LPTSTR>(errMsg.c_str()));
		return;
	}
	bMoreMods = Module32First(hthSnapshot, &hMod);
	if (bMoreMods == FALSE)
	{
		std::wstring message = L"Module32First";
		DllHook::ShowError(GetLastError(), const_cast<LPTSTR>(message.c_str()));
		return;
	}
	ListView_DeleteAllItems(hListView);
	LVITEM vitem = {0};
	vitem.mask = LVIF_TEXT;
	WCHAR threadIdStr[10];
	wcscpy_s<sizeof(threadIdStr) / sizeof(threadIdStr[0])>(threadIdStr, std::to_wstring(dwProcessId).c_str());
	for (int i = 0; bMoreMods; bMoreMods = Module32Next(hthSnapshot, &hMod), i++)
	{
		vitem.iItem = i;
		vitem.iSubItem = 0;
		vitem.pszText = threadIdStr;
		ListView_InsertItem(hListView, &vitem);

		vitem.iSubItem = 1;
		vitem.pszText = processName;
		ListView_SetItem(hListView, &vitem);
		vitem.iSubItem = 2;
		vitem.pszText = hMod.szExePath;
		ListView_SetItem(hListView, &vitem);
	}
	CloseHandle(hthSnapshot);
} 
BOOL InitListViewColumns(HWND hWndListView)
{
	RECT rcClient;
	GetClientRect(hWndListView, &rcClient);

	ListView_SetView(hWndListView, LV_VIEW_DETAILS);
	LVCOLUMN vcl;
	vcl.fmt = LVCFMT_LEFT;
	vcl.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	vcl.pszText = const_cast<LPWSTR>(L"ID");
	vcl.cx = 60; 
	vcl.iSubItem = 0;
	ListView_InsertColumn(hWndListView, 0, &vcl);
	vcl.pszText = const_cast<LPWSTR>(L"进程");
	vcl.pszText = const_cast<LPWSTR>(L"进程");
	vcl.cx = 90;
	vcl.iSubItem = 1;
	ListView_InsertColumn(hWndListView, 1, &vcl);
	vcl.pszText = const_cast<LPWSTR>(L"模块");
	vcl.cx = rcClient.right - rcClient.left - 170;
	vcl.iSubItem = 2;
	ListView_InsertColumn(hWndListView, 2, &vcl);
	ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_AUTOSIZECOLUMNS);
	return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		{
			RECT rcClient;
			GetClientRect(hWnd, &rcClient);
			hListView = CreateWindow(
				WC_LISTVIEW,
				L"",
				WS_CHILD | WS_VISIBLE | WS_BORDER/* LVS_REPORT*/,
				0,
				0,
				rcClient.right - rcClient.left,
				rcClient.bottom - rcClient.top - 50,
				hWnd,
				(HMENU)IDM_LISTVIEW,
				(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
				nullptr);
			InitListViewColumns(hListView);
			int buttonX = 5,
			    buttonY = rcClient.bottom - rcClient.top - 40,
			    buttonWeight = (rcClient.right - rcClient.left - 40) / 4,
			    buttonHeight = 30;
			HWND setHookButton = CreateWindow(
				L"BUTTON", // Predefined class; Unicode assumed 
				L"设置Hook", // Button text 
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // Styles 
				buttonX, // x position 
				buttonY, // y position 
				buttonWeight, // Button width
				buttonHeight, // Button height
				hWnd, // Parent window
				(HMENU)IDM_BUTTON1, // No menu.
				(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
				nullptr); // Pointer not needed.

			HWND unHookButton = CreateWindow(
				L"BUTTON",
				L"卸载Hook",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				buttonX + buttonWeight + 10,
				buttonY,
				buttonWeight,
				buttonHeight,
				hWnd,
				(HMENU)IDM_BUTTON2,
				(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
				nullptr);
			HWND hOutWnd = CreateWindow(_T("EDIT"),
			                            NULL,
			                            WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT,
			                            buttonX + (buttonWeight + 10) * 2,
			                            buttonY +1,
			                            buttonWeight,
			                            buttonHeight - 2,
			                            hWnd,
			                            (HMENU)IDM_OUTBOX,
			                            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			                            NULL);
			Edit_SetText(hOutWnd, L"spoolsv.exe");
			HWND button3 = CreateWindow(
				L"BUTTON",
				L"查看",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				buttonX + (buttonWeight + 10) * 3,
				buttonY,
				buttonWeight,
				buttonHeight,
				hWnd,
				(HMENU)IDM_BUTTON3,
				(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
				nullptr);
			break;
		}
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			switch (wmId)
			{
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			case IDM_BUTTON1:
				{
					Button1Click(hWnd);
					break;
				}
			case IDM_BUTTON2:
				{
					Button2Click(hWnd);
					break;
				}
			case IDM_BUTTON3:
				{
					btLookClick(hWnd);
					break;
				}
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_SIZING:
		{
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

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

bool SetPrivilege()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	TOKEN_PRIVILEGES tpPrevious;
	DWORD cbPrevious = sizeof(TOKEN_PRIVILEGES);


	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
		{
			return false;
		}
		// 1) Get current privilege setting
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = luid;
		tp.Privileges[0].Attributes = 0;

		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &tpPrevious, &cbPrevious);
		DWORD lastError = GetLastError();
		if (lastError != ERROR_SUCCESS)
		{
			return false;
		}

		// 2) Set privilege based on previous setting
		tpPrevious.PrivilegeCount = 1;
		tpPrevious.Privileges[0].Luid = luid;

		tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);

		AdjustTokenPrivileges(hToken, FALSE, &tpPrevious, cbPrevious, NULL, NULL);
		lastError = GetLastError();
		if (lastError != ERROR_SUCCESS)
		{
			return false;
		}
	}
	return TRUE;
}
