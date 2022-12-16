#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "wininet")

#include <windows.h>
#include <wininet.h>

TCHAR szClassName[] = TEXT("Window");

LPWSTR GetAllHeaders(HINTERNET hHttp)
{
	LPWSTR lpOutBuffer = NULL;
	DWORD dwSize = 0;
retry:
	if (!HttpQueryInfo(hHttp, HTTP_QUERY_RAW_HEADERS_CRLF,
		(LPVOID)lpOutBuffer, &dwSize, NULL))
	{
		if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND)
		{
			return NULL;
		}
		else
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				lpOutBuffer = (LPWSTR)GlobalAlloc(0, dwSize);
				goto retry;
			}
			else
			{
				if (lpOutBuffer)
				{
					GlobalFree(lpOutBuffer);
				}
				return NULL;
			}
		}
	}
	return lpOutBuffer;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton;
	static HWND hEdit1;
	static HWND hEdit2;
	switch (msg)
	{
	case WM_CREATE:
		hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"https://hack.jp/", WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 10, 10, 256, 32, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton = CreateWindow(L"BUTTON", L"取得", WS_VISIBLE | WS_CHILD | WS_TABSTOP, 276, 10, 256, 32, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit2 = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", 0, WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_TABSTOP | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 10, 50, 522, 256, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SIZE:
		MoveWindow(hEdit1, 10, 10, LOWORD(lParam) - 30 - 256, 32, TRUE);
		MoveWindow(hButton, LOWORD(lParam) - 10 - 256, 10, 256, 32, TRUE);
		MoveWindow(hEdit2, 10, 50, LOWORD(lParam) - 20, HIWORD(lParam) - 60, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			int nSize = GetWindowTextLength(hEdit1);
			if (nSize > 0) {
				LPWSTR lpszURL = (LPWSTR)GlobalAlloc(0, sizeof(WCHAR) * (nSize + 1));
				if (lpszURL) {
					HINTERNET hInternet = InternetOpen(
						NULL,
						INTERNET_OPEN_TYPE_PRECONFIG,
						NULL,
						NULL,
						0);
					if (hInternet) {
						GetWindowText(hEdit1, lpszURL, nSize + 1);
						HINTERNET hFile = InternetOpenUrl(
							hInternet,
							lpszURL,
							NULL,
							0,
							INTERNET_FLAG_RELOAD,
							0);
						GlobalFree(lpszURL);
						if (hFile) {
							LPWSTR lpszAllHeaders = GetAllHeaders(hFile);
							if (lpszAllHeaders) {
								SetWindowText(hEdit2, lpszAllHeaders);
								GlobalFree(lpszAllHeaders);
							}
							InternetCloseHandle(hFile);
						}
						InternetCloseHandle(hInternet);
					}
				}
			}
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefDlgProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		DLGWINDOWEXTRA,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		0,
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		L"Get Http Header",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!IsDialogMessage(hWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
