#undef UNICODE
#define UNICODE
#include <windows.h>
#include <stdio.h>

#include "trayicon.h"

#define LEN_BUFF_SHORT (10)
#define LEN_BUFF_LONG (50)

TCHAR buff[LEN_BUFF_LONG];
TCHAR *targetClass = TEXT("PuTTY");
TCHAR *targetTitle = TEXT("target");

HWND targetWindow = NULL;

void showMagic();
void findMyPutty();
void HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                    LONG idObject, LONG idChild,
                    DWORD dwEventThread, DWORD dwmsEventTime);


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE prev, LPSTR cmdline, int show)
{
  HWND hWnd;
  MSG msg;
  BOOL bRet;
  HWINEVENTHOOK g_hook;
  DWORD hProc = 0; //putty proc

  findMyPutty();
  if (targetWindow == NULL) {
    printf("didn't find the target window");
    return 1;
  }
  showMagic();

  GetWindowThreadProcessId(targetWindow, &hProc);
  if (hProc == 0) {
    printf("no process to call\n");
    return 3;
  }

  RegisterApplicationClass(hInst);

  hWnd = CreateWindow(
           THIS_CLASSNAME, THIS_TITLE,
           0, // style.
           0, 0, 0, 0, // location, size
           HWND_MESSAGE, // Message-only window.
           NULL, hInst, NULL);

  g_hook =  SetWinEventHook(
              EVENT_OBJECT_NAMECHANGE, EVENT_OBJECT_NAMECHANGE,
              NULL, // Handle to DLL.
              HandleWinEvent, // The callback.
              hProc, 0, // Process and thread IDs of interest (0 = all)
              WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
  if (g_hook == 0) {
    printf("Failed to SetWinEventHook\n");
    return 2;
  }

  //  Message loop
  while (TRUE) {
    bRet = GetMessage(&msg, NULL, 0, 0);
    if (bRet == 0 || bRet == -1)
      break;
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  UnregisterClass(THIS_CLASSNAME, hInst);

  return 0;
}

void showMagic()
{
  int len = GetWindowText(targetWindow, buff, LEN_BUFF_LONG);
  // printf("Title len = %d\n[%S]\n", len, buff);

  TCHAR magicMarker = len > lstrlen(targetTitle) ? buff[lstrlen(targetTitle)] : 0;
  switch (magicMarker) {
  case 0:
    printf("NULL char");
    break;
  case 9:
    printf("tab");
    break;
  case 30:
    printf("Record Seperator");
    break;
  case 31:
    printf("Unit Seperator");
    break;
  default:
    printf("Not enough magic: %d", magicMarker);
    break;
  }
  printf("\n");
}

void HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                    LONG idObject, LONG idChild,
                    DWORD dwEventThread, DWORD dwmsEventTime)
{
  if (event == EVENT_OBJECT_NAMECHANGE) {
    showMagic();
  }
}


BOOL CALLBACK callback(HWND hWnd, LPARAM lParam)
{
  int len;

  if (targetWindow != NULL) {
    return TRUE;
  }

  len = GetClassName(hWnd, buff, lstrlen(targetClass) + 1);
  if (len <= 0  ||
      lstrcmp(buff, targetClass) != 0) {
    return TRUE;
  }
  len = GetWindowText(hWnd, buff, lstrlen(targetTitle) + 1);
  if (len > 0 &&
      lstrcmp(buff, targetTitle) == 0) {
    targetWindow = hWnd;
  }

  // printf("window: [%S], [%S]\n", title, className);
  return TRUE;
}

void findMyPutty() {
  EnumWindows(callback, 0);
}
