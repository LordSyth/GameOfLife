#include "stdafx.h"
#include "GameOfLife.h"
#include <vector>
#include <ObjIdl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")
#define MAX_LOADSTRING 100
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
ATOM MyRegisterClass(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	Gdiplus::GdiplusStartupInput input;
	ULONG_PTR token;
	Gdiplus::GdiplusStartup(&token, &input, nullptr);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GAMEOFLIFE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow))
        return FALSE;
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAMEOFLIFE));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
	Gdiplus::GdiplusShutdown(token);
    return (int) msg.wParam;
}
ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAMEOFLIFE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GAMEOFLIFE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

Gdiplus::Bitmap* Bitmap;
std::vector<std::vector<bool>> Map;
std::vector<std::vector<bool>> Buffer;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
	case WM_CREATE:
		{
			Bitmap = new Gdiplus::Bitmap(512, 512);
			Map = Buffer = std::vector<std::vector<bool>>(128, std::vector<bool>(128, false));
			
			HMENU hMenuMain = GetMenu(hWnd);
			DeleteMenu(hMenuMain, 1, MF_BYPOSITION);

			SetTimer(hWnd, 0, 20, TIMERPROC(NULL));
		}
		break;
	case WM_TIMER:
		{
			int x, y, dx, dy, count; Gdiplus::Color c;
			for (x = 0; x < 128; ++x) for (y = 0; y < 128; ++y) {
				count = 0;
				for (dx = -1; dx < 2; ++dx)
					for (dy = -1; dy < 2; ++dy)
						if (dx != 0 || dy != 0)
							if (Map[x + dx][y + dy])
								++count;
				c = Gdiplus::Color::White;
				if (Buffer[x][y] = (count == 3 || (Map[x][y] = true && count == 2)))
					c = Gdiplus::Color::Black;
				for (dx = 0; dx < 4; ++dx) for (dy = 0; dy < 4; ++dy)
					Bitmap->SetPixel(x * 4 + dx, y * 4 + dy, c);
			}
			Map = Buffer;
			InvalidateRect(hWnd, nullptr, false);
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			Gdiplus::Graphics g(hdc);
			g.DrawImage(Bitmap, 0, 0);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
