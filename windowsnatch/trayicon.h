#ifndef _TRAYICON_H
#define _TRAYICON_H

#include <tchar.h>
#include <windows.h>

#define HELP_ABOUT _T("Extract status from the ether.")

// TODO these definitions are used in other contexts too; Cleanup.
#define THIS_CLASSNAME      _T("WindowSnatch")
#define THIS_TITLE          _T("WindowSnatch")


// ids for messages in the message-loop
enum {
  ID_TRAYICON         = 1,

  APPWM_TRAYICON      = WM_APP,
  APPWM_NOP           = WM_APP + 1,

  APPWM_CLICKED_WINDOW,
};

//  tray commands
enum {
  ID_EXIT             = 2000,
  ID_ABOUT,

  ID_RECONNECT_DEVICE = 3000,
  ID_DISCONNECT_DEVICE,
  ID_BIND_WINDOW_RANDOM,

  ID_REBIND_TARGET_0,
  ID_REBIND_TARGET_1,
  ID_REBIND_TARGET_2,
  ID_REBIND_TARGET_3,
  ID_REBIND_TARGET_4,
  ID_REBIND_TARGET_5,
};

void (*app_close_listener)(HWND);
LRESULT(*WindowProc_fallback)(HWND, UINT, WPARAM, LPARAM);
BOOL (*OnCommand_fallback)(HWND hWnd, WORD wID, HWND hCtl);
// This one is defined in an unrelated file! superhackday!
void BuildRebindSubmenu(HMENU submenu);

void RegisterApplicationClass(HINSTANCE hInstance);
inline void UnregisterApplicationClass(HINSTANCE hInstance) {
  UnregisterClass(THIS_CLASSNAME, hInstance);
}

#endif
