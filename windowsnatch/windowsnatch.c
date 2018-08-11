#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include "common/configuration.h"

#include "trayicon.h"
#include "usb_hid.h"
#include "find_window.h"
#include "relocate_window.h"
#include "windowsnatch.h"
#include "common/icd.h"
#include "common/icd_messages.h"

#define LEN_BUFF_LONG (256)

TCHAR buff[LEN_BUFF_LONG];
TCHAR *targetClass = _T("PuTTY");

TARGET_WINDOW targets[NUMBER_OF_TARGETS];
TARGET_CLASS PUTTY_TARGET_CLASS = {
  .className = _T("PuTTY"),
};

BOOL teensyConnected = FALSE;
const UINT_PTR autoReconnectTimer = 21;

void AutoBindTargets();
void installPutty(int targetId, HWND windowHandle, BOOL silent);
void AutoReconnectTeensy(HWND, UINT, UINT_PTR, DWORD);

void HandleTargetWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                          LONG idObject, LONG idChild,
                          DWORD dwEventThread, DWORD dwmsEventTime);
void WINAPI handleTeensyMessage(DWORD, DWORD, LPOVERLAPPED);
BOOL TrayiconCommandHandler(HWND, WORD, HWND);
LRESULT MessageLoopMessageHandler(HWND, UINT, WPARAM, LPARAM);
void ReleaseTarget(TARGET_WINDOW*);

void DisconnectTeensy();
BOOL ConnectTeensy(BOOL silent);
void ReprogramTeensy();

void RepositionTargetWindows();

void ShowError(LPCTSTR body);

unsigned char rawhid_buf[64];

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE prev, LPSTR cmdline, int show)
{
  OnCommand_fallback = TrayiconCommandHandler;
  WindowProc_fallback = MessageLoopMessageHandler;

  for (int i = 0; i < NUMBER_OF_TARGETS; i++) {
    targets[i].targetId = i;
    targets[i].windowHandle = NULL;
  }

  RegisterApplicationClass(hInst);

  HWND hWnd = CreateWindow(
                THIS_CLASSNAME, THIS_TITLE,
                0, // style.
                0, 0, 0, 0, // location, size
                HWND_MESSAGE, // Message-only window.
                NULL, hInst, NULL);

  SetTimer(hWnd, autoReconnectTimer, 30 * 1000, AutoReconnectTeensy);
  AutoReconnectTeensy(hWnd, 0, 0, 0);
  AutoBindTargets();

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
  for (int i = 0; i < NUMBER_OF_TARGETS; i++) {
    ReleaseTarget(&targets[1]);
  }
  DestroyWindow(hWnd);
  UnregisterApplicationClass(hInst);

  return 0;
}

// This function is called as a fallback from trayicon.c's OnCommand.
BOOL TrayiconCommandHandler(HWND hWnd, WORD commandID, HWND hCtl) {
  if (IsRebindCommand(commandID)) {
    FindWindowByClick(hWnd, APPWM_CLICKED_WINDOW,
                      commandID - ID_REBIND_TARGET_0);
    return 0;
  }

  switch (commandID) {
  case ID_RECONNECT_DEVICE:
    ConnectTeensy(FALSE);
    return 0;
  case ID_DISCONNECT_DEVICE:
    DisconnectTeensy();
    return 0;
  case ID_PROGRAM_DEVICE:
    ReprogramTeensy();
    return 0;

  case ID_BIND_TARGET_AUTO:
    AutoBindTargets();
    return 0;

  case ID_REPOSITION_TARGET_WINDOWS:
    RepositionTargetWindows();
    return 0;
  }
  printf("menu cmd: %d", commandID);
  return 1;
}

LRESULT MessageLoopMessageHandler(
  HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

  switch (uMsg) {
  case APPWM_CLICKED_WINDOW:
    printf("Got clicked window \n");
    HWND targetWindow = (HWND)lParam;
    int targetId = wParam;
    int len = GetClassName(targetWindow, buff, 48);
    if (len <= 0  || lstrcmp(buff, targetClass) != 0) {
      buff[len] = 0;
      printf("clicked on Not putty: %S\n", buff);
    } else {
      installPutty(targetId, targetWindow, FALSE);
    }
    return 0;

  default:
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
}

void FocusOnWindow(TARGET_WINDOW* target) {
  // This sometimes work:
  BOOL ok = SetForegroundWindow(target->windowHandle);
  if (!ok) {
    ok = FlashWindowEx(&(FLASHWINFO) {
      sizeof(FLASHWINFO),
             target->windowHandle,
             FLASHW_ALL,
             5, // count of flashes
             0, // default rate of flash
    });
  }
  // ok = BringWindowToTop(target->windowHandle);
  if (!ok) {
    printf("didnt work\n");
  }
  // SwitchToThisWindow (target->windowHandle, TRUE);
}

void SendCommandToPutty(TARGET_WINDOW *putty, BOOL btn1, BOOL btn2) {
  if (!IsTargetWindowActive(putty)) {
    printf("putty null");
    return;
  }

  if (btn1) {
    PostMessage(putty->windowHandle, WM_KEYDOWN, VK_UP, 0);
    PostMessage(putty->windowHandle, WM_KEYDOWN, VK_RETURN, 0);
  }

  if (btn2) {
    PostMessage(putty->windowHandle, WM_KEYDOWN, VK_CANCEL, 0);
  }
}

void WINAPI handleTeensyMessage(
  DWORD dwErr, DWORD cbBytesRead, LPOVERLAPPED lpOverLap) {

  if (dwErr == ERROR_OPERATION_ABORTED) {
    // terminated, disconnect device?
    printf("error from teensy recv: ERROR_OPERATION_ABORTED\n");
    return;
  } else if (dwErr == ERROR_DEVICE_NOT_CONNECTED) {
    printf("Device disconnect\n");
    DisconnectTeensy();
    return;
  } else if (dwErr) {
    printf("error from teensy recv: %ld\n", dwErr);
  }
  if (cbBytesRead == 0) {
    return;
  }

  rawhid_async_recv_complete(0, rawhid_buf, 64);

  dispatch_incoming_message(rawhid_buf);

  // listen to another message
  rawhid_async_recv(0, &handleTeensyMessage);
}

void showMagic(TARGET_WINDOW *target) {
  if (!teensyConnected) {
    return;
  }

  TCHAR magicMarker;
  if (IsTargetWindowActive(target)) {
    int len = GetWindowText(target->windowHandle, buff, LEN_BUFF_LONG);
    magicMarker = buff[len - 1];
  } else {
    magicMarker = 0;
  }
  char magic;
  switch (magicMarker) {
  case 0:
    // printf("Too short");
    magic = COLOR_BLACK;
    break;

  case 9:
    // printf("tab");
    magic = COLOR_RED;
    break;
  case 30:
    // printf("Record Seperator");
    magic = COLOR_BLUE;
    break;
  case 31:
    // printf("Unit Seperator");
    magic = COLOR_GREEN;
    break;

  default:
    // printf("Not enough magic: %d", magicMarker);
    magic = COLOR_WHITE;
    break;
  }

  rawhid_buf[1] = MSG_CODE_SET_LED;
  rawhid_buf[2] = 1;
  rawhid_buf[3] = target->targetId;
  rawhid_buf[4] = magic;
  rawhid_send(0, rawhid_buf, 64, 100);
}

void HandleTargetWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                          LONG idObject, LONG idChild,
                          DWORD dwEventThread, DWORD dwmsEventTime) {
  if (idObject != OBJID_WINDOW) {
    return;
  }
  TARGET_WINDOW *target = GetTargetWindowByWindowHandle(hwnd);
  if (target == NULL) {
    return;
  }

  switch (event) {
  case EVENT_OBJECT_NAMECHANGE:
    return showMagic(target);
  case EVENT_OBJECT_DESTROY:
    return ReleaseTarget(target);
  default:
    printf("Unexpected event 0x%lx", event);
  }
}

TARGET_WINDOW* GetTargetWindowByWindowHandle(HWND hwnd) {
  for (int i = 0; i < NUMBER_OF_TARGETS; i++) {
    TARGET_WINDOW* target = &targets[i];
    if (target->windowHandle == hwnd)
      return target;
  }
  return NULL;
}
TARGET_WINDOW* GetTargetWindow(int targetId) {
  if (targetId >= NUMBER_OF_TARGETS) {
    return NULL;
  }
  TARGET_WINDOW* target = &targets[targetId];
  if (IsTargetWindowActive(target)) {
    return target;
  }
  return NULL;
}
BOOL IsTargetWindowActive(TARGET_WINDOW *target) {
  return (target != NULL) && (target->windowHandle != NULL);
}
TARGET_WINDOW* FindEmptyTargetWindow() {
  return GetTargetWindowByWindowHandle(NULL);
}


void ReleaseTarget(TARGET_WINDOW *target) {
  if (!IsTargetWindowActive(target)) {
    return;
  }
  if (target->eventHook1) UnhookWinEvent(target->eventHook1);
  if (target->eventHook2) UnhookWinEvent(target->eventHook2);
  target->windowHandle = NULL;
  showMagic(target);
}

BOOL CALLBACK AutoBindPuttyCallback(HWND hWnd, LPARAM __) {
  TCHAR* className = PUTTY_TARGET_CLASS.className;
  int len = GetClassName(hWnd, buff, lstrlen(className) + 1);
  if (len <= 0  || lstrcmp(buff, className) != 0) {
    return TRUE;
  }

  if (GetTargetWindowByWindowHandle(hWnd) != NULL) {
    // We already have this one
    return TRUE;
  }

  TARGET_WINDOW *target = FindEmptyTargetWindow();
  if (target == NULL) {
    // Out of room - terminate loop
    return FALSE;
  }

  installPutty(target->targetId, hWnd, TRUE);

  return TRUE;
}

void AutoBindTargets() {
  EnumWindows(AutoBindPuttyCallback, 0);
}

void RepositionTargetWindows() {
  MagicallyRelocateAllWindows();
}

void installPutty(int targetId, HWND windowHandle, BOOL silent) {
  TARGET_WINDOW *putty = &targets[targetId];

  ReleaseTarget(putty);

  putty->windowHandle = windowHandle;
  putty->targetClass = &PUTTY_TARGET_CLASS;
  DWORD hProc = 0;
  GetWindowThreadProcessId(putty->windowHandle, &hProc);
  if (hProc == 0) {
    if (!silent) ShowError(_T("Found target Putty, it has no process"));
    putty->windowHandle = NULL;
    return;
  }

  putty->eventHook1 = SetWinEventHook(
                        EVENT_OBJECT_NAMECHANGE, EVENT_OBJECT_NAMECHANGE,
                        NULL,
                        HandleTargetWinEvent,
                        hProc, 0,
                        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
  if (putty->eventHook1 == 0) {
    if (!silent) ShowError(_T("Failed to SetWinEventHook"));
    putty->windowHandle = NULL;
    return;
  }

  putty->eventHook2 = SetWinEventHook(
                        EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY,
                        NULL,
                        HandleTargetWinEvent,
                        hProc, 0,
                        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
  if (putty->eventHook2 == 0) {
    if (!silent) ShowError(_T("Failed to set 2nd SetWinEventHook"));
  }

  showMagic(putty);
}

TCHAR *unbound_title = _T("[unbound]");
void BuildRebindSubmenu(HMENU submenu) {
  int i;

  for (i = 0; i < NUMBER_OF_TARGETS; i++) {
    TARGET_WINDOW* target = &targets[i];
    TCHAR *title = target->windowHandle ?
                   target->targetClass->className : unbound_title;

    _sntprintf(buff, LEN_BUFF_LONG, _T("%d: %s"), i, title);
    InsertMenu(submenu, i, MF_BYPOSITION | MF_STRING,
               ID_REBIND_TARGET_0 + i, buff);
  }
}

void AutoReconnectTeensy(HWND hwnd, UINT _, UINT_PTR idEvent, DWORD __) {
  if (teensyConnected) return;

  ConnectTeensy(TRUE);
}

#define HELP_ABOUT_TEMPLATE _T( \
  "Extract status from the ether.\n\n"   \
  "Client version: %s\n\n" \
  "Teensy version: %s" )

TCHAR teensy_version[64] = _T("unknown");
void ShowAboutMenuModal(HWND hWnd) {
  _sntprintf(buff, LEN_BUFF_LONG,
             HELP_ABOUT_TEMPLATE, _T(CODE_VERSION_STR), teensy_version);
  MessageBox(hWnd, buff, THIS_TITLE,
             MB_ICONINFORMATION | MB_OK);
}

void HandleTeensyVersionString(LPCTSTR version_string) {
  wcscpy_s(teensy_version, 64, version_string);
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

  // ask for version
  rawhid_buf[0] = ICD_MAGIC_NUMBER;
  rawhid_buf[1] = MSG_CODE_GET_VERSION;
  rawhid_send(0, rawhid_buf, 64, 100);

  for (int i = 0; i < NUMBER_OF_TARGETS; i++) {
    showMagic(&targets[i]);
  }
  return TRUE;
}

void DisconnectTeensy() {
  if (!teensyConnected) return;
  rawhid_async_recv_cancel(0);
  rawhid_close(0);
  teensyConnected = FALSE;

  wcscpy_s(teensy_version, 64, _T("unknown"));
}

void ReprogramTeensy() {
  if (!teensyConnected) {
    ShowError(_T("Teensy not connected"));
    return;
  }

  rawhid_buf[0] = ICD_MAGIC_NUMBER;
  rawhid_buf[1] = MSG_CODE_ENTER_PROGRAMMING_MODE;
  rawhid_buf[17] = 42;
  rawhid_send(0, rawhid_buf, 64, 100);
}

void ShowError(LPCTSTR body) {
  MessageBox(NULL, body, NULL, MB_ICONERROR);
}

void signal_error(int count) {
  _sntprintf(buff, LEN_BUFF_LONG, _T("Error from below: %d"), count);
  ShowError(buff);
}
