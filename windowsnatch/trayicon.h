#ifndef _TRAYICON_H
#define _TRAYICON_H

#include <tchar.h>
#include <windows.h>

#define HELP_ABOUT _T("Extract status from the ether.")

// TODO these definitions are used in other contexts too; Cleanup.
#define THIS_CLASSNAME      _T("WindowSnatch")
#define THIS_TITLE          _T("WindowSnatch")


enum {
  ID_TRAYICON         = 1,

  APPWM_TRAYICON      = WM_APP,
  APPWM_NOP           = WM_APP + 1,

  //  Our commands
  ID_EXIT             = 2000,
  ID_ABOUT,
};

void (*app_close_listener)(HWND);
LRESULT(*WindowProc_fallback)(HWND, UINT, WPARAM, LPARAM);

void RegisterTrayIcon(HINSTANCE hInstance);
inline void UnregisterTrayIcon(HINSTANCE hInstance) {
  UnregisterClass(THIS_CLASSNAME, hInstance);
}

#endif
