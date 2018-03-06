#include "find_window.h"

// This code is inspired by https://stefansundin.github.io/altdrag/

#define SET_WINDOW_TO_NOTIFY(WND, V) SetWindowLongPtr(WND, 0, (LONG_PTR)V)
#define GET_WINDOW_TO_NOTIFY(WND) (HWND)GetWindowLongPtr(WND, 0)
#define SET_WINDOW_MSG_CODE(WND, V) SetWindowLong(WND, 8, V)
#define GET_WINDOW_MSG_CODE(WND) GetWindowLong(WND, 8)
#define SET_WINDOW_WPARAM(WND, V) SetWindowLongPtr(WND, 12, V)
#define GET_WINDOW_WPARAM(WND) GetWindowLongPtr(WND, 12)
// wastes 8 bytes on 32bit system, and breaks on 128bit system.
#define EXTRA_BYTES_FOR_WINDOW (8 + 4 + 8)

static LRESULT CALLBACK FindWindowByClickHandle_WindowProc(
  HWND findhwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

  if (msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN) {
    ShowWindow(findhwnd, SW_HIDE);

    if (msg == WM_LBUTTONDOWN) {
      POINT pt;
      GetCursorPos(&pt);
      HWND window = WindowFromPoint(pt);
      window = GetAncestor(window, GA_ROOT);

      HWND WindowToNotify = GET_WINDOW_TO_NOTIFY(findhwnd);
      UINT MsgCodeToSend = GET_WINDOW_MSG_CODE(findhwnd);
      WPARAM wParamToSend = GET_WINDOW_WPARAM(findhwnd);

      PostMessage(WindowToNotify, MsgCodeToSend, wParamToSend, (LPARAM)window);
    }
    DestroyWindow(findhwnd);
  }
  return DefWindowProc(findhwnd, msg, wParam, lParam);
}

ATOM findWindowClass = 0;

BOOL FindWindowByClick(HWND hWindowToNotify, UINT msg, WPARAM wParam) {
  int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
  int top = GetSystemMetrics(SM_YVIRTUALSCREEN);
  int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

  if (findWindowClass == 0) {
    WNDCLASSEX wnd = {0};
    wnd.cbSize = sizeof(WNDCLASSEX);
    wnd.lpfnWndProc = FindWindowByClickHandle_WindowProc;

    wnd.cbWndExtra = EXTRA_BYTES_FOR_WINDOW;
    wnd.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wnd.lpszClassName = TEXT("windowsnatch-window-finder");
    wnd.hCursor = LoadImage(
                    NULL, MAKEINTRESOURCE(OCR_CROSS),
                    IMAGE_CURSOR, 0, 0, LR_SHARED);

    if (wnd.hCursor == NULL) {
      return FALSE;
    }
    findWindowClass = RegisterClassEx(&wnd);
  }

  HWND findhwnd = CreateWindowEx(
                    WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED,
                    MAKEINTATOM(findWindowClass),
                    NULL, // window name
                    WS_POPUP,
                    left, top, width, height,
                    NULL, // Parent window
                    NULL,
                    NULL, // hInstance
                    NULL); // lpParam

  SET_WINDOW_TO_NOTIFY(findhwnd, hWindowToNotify);
  SET_WINDOW_MSG_CODE(findhwnd, msg);
  SET_WINDOW_WPARAM(findhwnd, wParam);

  SetLayeredWindowAttributes(findhwnd, 0, 1, LWA_ALPHA); // Almost transparent
  ShowWindowAsync(findhwnd, SW_SHOWNA);

  return TRUE;
}
