#ifndef WINDOWSNATCH_H
#define WINDOWSNATCH_H

typedef struct TARGET_CLASS {
  TCHAR* className;
} TARGET_CLASS;

typedef struct TARGET_WINDOW {
  int targetId;
  HWND windowHandle;
  HWINEVENTHOOK eventHook1;
  HWINEVENTHOOK eventHook2;
  TARGET_CLASS* targetClass;
} TARGET_WINDOW;

/** returns an active target, or NULL */
TARGET_WINDOW* GetTargetWindow(int id);
TARGET_WINDOW* GetTargetWindowByWindowHandle(HWND hwnd);
TARGET_WINDOW* FindEmptyTargetWindow();
BOOL IsTargetWindowActive(TARGET_WINDOW*);

#endif // WINDOWSNATCH_H
