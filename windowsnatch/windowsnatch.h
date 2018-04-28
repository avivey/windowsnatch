#ifndef WINDOWSNATCH_H
#define WINDOWSNATCH_H

typedef struct TARGET_CLASS {
  TCHAR* className;
} TARGET_CLASS;

typedef struct TARGET_WINDOW {
  int targetId;
  HWND windowHandle;
  HWINEVENTHOOK eventHook;
  TARGET_CLASS* targetClass;
} TARGET_WINDOW;

TARGET_WINDOW* GetTargetWindow(int id);
TARGET_WINDOW* GetTargetWindowByWindowHandle(HWND hwnd);

BOOL IsTargetWindowActive(TARGET_WINDOW*);

#endif // WINDOWSNATCH_H
