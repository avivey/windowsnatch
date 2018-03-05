#undef UNICODE
#define UNICODE
#include <windows.h>
#include <stdio.h>

#include "trayicon.h"
#include "usb_hid.h"

#define LEN_BUFF_LONG (50)
#define NUMBER_OF_TARGETS (6)

TCHAR buff[LEN_BUFF_LONG];
TCHAR *targetClass = TEXT("PuTTY");
TCHAR *targetTitle = TEXT("target");

typedef struct TARGET_WINDOW {
  HWND windowHandle;
  HWINEVENTHOOK eventHook;
} TARGET_WINDOW;

TARGET_WINDOW targets[NUMBER_OF_TARGETS];

BOOL teensyConnected = FALSE;

void findMyPutty(BOOL silent);
void HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                    LONG idObject, LONG idChild,
                    DWORD dwEventThread, DWORD dwmsEventTime);
void WINAPI handleTeensyMessage(DWORD, DWORD, LPOVERLAPPED);
BOOL TrayiconCommandHandler(HWND, WORD, HWND);
void ReleaseTarget(TARGET_WINDOW*);

void DisconnectTeensy();
BOOL ConnectTeensy(BOOL silent);

void ShowError(LPCTSTR body);

char rawhid_buf[64];

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE prev, LPSTR cmdline, int show)
{
  OnCommand_fallback = TrayiconCommandHandler;

  memset(&targets, 0, NUMBER_OF_TARGETS * sizeof(TARGET_WINDOW));

  ConnectTeensy(TRUE);
  findMyPutty(TRUE);

  RegisterApplicationClass(hInst);

  HWND hWnd = CreateWindow(
                THIS_CLASSNAME, THIS_TITLE,
                0, // style.
                0, 0, 0, 0, // location, size
                HWND_MESSAGE, // Message-only window.
                NULL, hInst, NULL);

  //  Message loop
  while (TRUE) {
    DWORD wait_result = MsgWaitForMultipleObjectsEx(
                          0, // no handles
                          0, // no handles
                          INFINITE,
                          QS_ALLINPUT,
                          MWMO_ALERTABLE | MWMO_INPUTAVAILABLE);

    if (wait_result == WAIT_FAILED) {
      continue;
    }

    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) {
        goto stop_loop;
      }
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

  }
stop_loop:

  ReleaseTarget(&targets[0]);
  DestroyWindow(hWnd);
  UnregisterApplicationClass(hInst);

  return 0;
}

// This function is called as a fallback from trayicon.c's OnCommand.
BOOL TrayiconCommandHandler(HWND hWnd, WORD commandID, HWND hCtl) {

  switch (commandID) {
  case ID_RECONNECT_DEVICE:
    ConnectTeensy(FALSE);


    return 0;
  case ID_DISCONNECT_DEVICE:
    DisconnectTeensy();
    return 0;
  case ID_BIND_WINDOW_RANDOM:
    findMyPutty(FALSE);
    return 0;
  }
  printf("menu cmd: %d", commandID);
  return 1;
}

void sendCommandToPutty(TARGET_WINDOW *putty, BOOL btn1, BOOL btn2) {
  if (putty->windowHandle == NULL) {
    return;
  }

  if (btn1) {
    PostMessage(putty->windowHandle, WM_KEYDOWN, VK_UP, 0);
    PostMessage(putty->windowHandle, WM_KEYDOWN, VK_RETURN, 0);
  }

  if (btn2) {
    // This sometimes work:
    BOOL ok = SetForegroundWindow(putty->windowHandle);
    if (!ok) {
      ok = FlashWindowEx(&(FLASHWINFO) {
        sizeof(FLASHWINFO),
               putty->windowHandle,
               FLASHW_ALL,
               5, // count of flashes
               0, // default rate of flash
      });
    }
    // ok = BringWindowToTop(putty->windowHandle);
    if (!ok) {
      printf("didnt work\n");
    }
    // SwitchToThisWindow (putty->windowHandle, TRUE);
  }

}

void WINAPI handleTeensyMessage(
  DWORD dwErr, DWORD cbBytesRead, LPOVERLAPPED lpOverLap) {

  if (dwErr == ERROR_OPERATION_ABORTED) {
    // terminated, disconnect device?
    printf("error from teensy recv: ERROR_OPERATION_ABORTED\n");
    return;
  } else if (dwErr) {
    printf("error from teensy recv: %ld\n", dwErr);
  }

  rawhid_async_recv_complete(0, rawhid_buf, 64);

  sendCommandToPutty(&targets[0], rawhid_buf[3], rawhid_buf[4]);

  // listen to another message
  rawhid_async_recv(0, &handleTeensyMessage);
}

void showMagic(HWND puttyWindow)
{
  char magic = 10;
  char sendbuff[64];

  int len = GetWindowText(puttyWindow, buff, LEN_BUFF_LONG);
  // printf("Title len = %d\n[%S]\n", len, buff);

  TCHAR magicMarker = len > lstrlen(targetTitle) ? buff[len - 1] : 0;
  switch (magicMarker) {
  case 0:
    // printf("Too short");
    magic = 0b000;
    break;

  case 9:
    // printf("tab");
    magic = 0b100;
    break;
  case 30:
    // printf("Record Seperator");
    magic = 0b001;
    break;
  case 31:
    // printf("Unit Seperator");
    magic = 0b010;
    break;

  default:
    // printf("Not enough magic: %d", magicMarker);
    magic = 0b111;
    break;
  }
  if (magic < 10) {
    sendbuff[0] = magic;
    // printf("\nsending: %d", magic);
    if (teensyConnected)
      rawhid_send(0, sendbuff, 64, 100);
  }
}

void HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                    LONG idObject, LONG idChild,
                    DWORD dwEventThread, DWORD dwmsEventTime)
{
  if (event == EVENT_OBJECT_NAMECHANGE && idObject == OBJID_WINDOW) {
    showMagic(hwnd);
  }
}


BOOL CALLBACK find_putty_callback(HWND hWnd, LPARAM target_index)
{
  TARGET_WINDOW *putty = &targets[target_index];
  if (putty->windowHandle != NULL) {
    return FALSE;
  }

  int len = GetClassName(hWnd, buff, lstrlen(targetClass) + 1);
  if (len <= 0  || lstrcmp(buff, targetClass) != 0) {
    return TRUE;
  }
  len = GetWindowText(hWnd, buff, lstrlen(targetTitle) + 1);
  if (len > 0 && lstrcmp(buff, targetTitle) == 0) {
    putty->windowHandle = hWnd;
    return FALSE;
  }

  return TRUE;
}

void ReleaseTarget(TARGET_WINDOW *target) {
  if (target->windowHandle == NULL) {
    return;
  }
  UnhookWinEvent(target->eventHook);
  target->windowHandle = NULL;
}

void findMyPutty(BOOL silent) {
  // This function still only supports target[0].
  TARGET_WINDOW *putty = targets;

  ReleaseTarget(putty);
  int target_id = 0;
  EnumWindows(find_putty_callback, target_id);

  if (putty->windowHandle == NULL) {
    if (!silent) ShowError(_T("Error finding target Putty"));
    return;
  }

  DWORD hProc = 0;
  GetWindowThreadProcessId(putty->windowHandle, &hProc);
  if (hProc == 0) {
    if (!silent) ShowError(_T("Found target Putty, it has no process"));
    putty->windowHandle = NULL;
    return;
  }

  putty->eventHook = SetWinEventHook(
                       EVENT_OBJECT_NAMECHANGE, EVENT_OBJECT_NAMECHANGE,
                       NULL, // Handle to DLL.
                       HandleWinEvent, // The event handler callback.
                       hProc, 0, // Process and thread IDs of interest (0 = all)
                       WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
  if (putty->eventHook == 0) {
    if (!silent) ShowError(_T("Failed to SetWinEventHook"));
    putty->windowHandle = NULL;
    return;
  }

  showMagic(putty->windowHandle);
}

BOOL ConnectTeensy(BOOL silent) {
  DisconnectTeensy();

  int count = rawhid_open(1, 0x16C0, 0x0486, 0xFFAB, 0x0200);
  if (count <= 0) {
    if (!silent) ShowError(_T("Teensy device not found"));
    return FALSE;
  }
  teensyConnected = TRUE;
  rawhid_async_recv(0, &handleTeensyMessage);

  // TODO showAllMagic()
  if (targets[0].windowHandle != NULL) showMagic(targets[0].windowHandle);
  return TRUE;
}

void DisconnectTeensy() {
  if (!teensyConnected) return;
  rawhid_async_recv_cancel(0);
  rawhid_close(0);
  teensyConnected = FALSE;
}

void ShowError(LPCTSTR body) {
  MessageBox(NULL, body, NULL, MB_ICONERROR);
}
