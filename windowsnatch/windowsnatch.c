#undef UNICODE
#define UNICODE
#include <windows.h>
#include <stdio.h>

#include "trayicon.h"
#include "usb_hid.h"

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
void WINAPI handleTeensyMessage(DWORD, DWORD, LPOVERLAPPED);

char rawhid_buf[64];


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE prev, LPSTR cmdline, int show)
{
  HWND hWnd;
  MSG msg;
  // BOOL bRet;
  int iRetValue = 9;
  HWINEVENTHOOK g_hook;
  DWORD hProc = 0; //putty proc

  int count;

  findMyPutty();
  if (targetWindow == NULL) {
    printf("didn't find the target window");
    return 1;
  }

  GetWindowThreadProcessId(targetWindow, &hProc);
  if (hProc == 0) {
    printf("no process to call\n");
    return 3;
  }

  count = rawhid_open(1, 0x16C0, 0x0486, 0xFFAB, 0x0200);
  if (count <= 0) {
    printf("no rawhid device found\n");
    return 4;
  }
  rawhid_async_recv(0, &handleTeensyMessage);

  showMagic();


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
    iRetValue = 2;
    goto cleanup;
  }

  //  Message loop
  while (TRUE) {
    DWORD wait_result = MsgWaitForMultipleObjectsEx(0, // no handles
                        0, // no handles
                        INFINITE,
                        QS_ALLINPUT,
                        MWMO_ALERTABLE | MWMO_INPUTAVAILABLE);

    if (wait_result == WAIT_FAILED) {
      printf("boom11");
      continue;
    }

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) {
        goto stop_loop;
      }
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    /*    count = rawhid_recv(rawhid_handle, rawhid_buf, 64, 220);
        if (count < 0) {
          printf("teensy gone away\n");
          rawhid_close(rawhid_handle);
          break;
        }
        if (count > 0) {
          printf("teensy speaks\n");
        }*/
  }
stop_loop:

  iRetValue = 0;

cleanup:

  UnhookWinEvent(g_hook);
  DestroyWindow(hWnd);
  UnregisterApplicationClass(hInst);

  return iRetValue;
}

void WINAPI handleTeensyMessage(DWORD dwErr, DWORD cbBytesRead, LPOVERLAPPED lpOverLap) {
  // printf("got teensy msg\n");

  rawhid_async_recv_complete(0, rawhid_buf, 64);

  // do another one
  rawhid_async_recv(0, &handleTeensyMessage);
}

void showMagic()
{
  char magic = 10;
  char sendbuff[64];


  int len = GetWindowText(targetWindow, buff, LEN_BUFF_LONG);
  // printf("Title len = %d\n[%S]\n", len, buff);

  TCHAR magicMarker =
    len > lstrlen(targetTitle) ? buff[lstrlen(targetTitle)] : 0;
  switch (magicMarker) {
  case 0:
    printf("Too short");
    magic = 0;
    break;

  case 9:
    printf("tab");
    magic = 2;
    break;
  case 30:
    printf("Record Seperator");
    magic = 1;
    break;
  case 31:
    printf("Unit Seperator");
    magic = 4;
    break;

  default:
    printf("Not enough magic: %d", magicMarker);
    magic = 7;
    break;
  }
  if (magic < 10) {
    sendbuff[0] = magic;
    printf("\nsending: %d", magic);
    rawhid_send(0, sendbuff, 64, 100);
  }
  printf("\n");
}

void HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                    LONG idObject, LONG idChild,
                    DWORD dwEventThread, DWORD dwmsEventTime)
{
  if (event == EVENT_OBJECT_NAMECHANGE && idObject == OBJID_WINDOW) {
    showMagic();
  }
}


BOOL CALLBACK callback(HWND hWnd, LPARAM lParam)
{
  int len;

  if (targetWindow != NULL) {
    return FALSE;
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
    return FALSE;
  }

  // printf("window: [%S], [%S]\n", title, className);
  return TRUE;
}

void findMyPutty() {
  EnumWindows(callback, 0);
}
