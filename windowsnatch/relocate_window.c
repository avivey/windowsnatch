#include <windows.h>

#include "relocate_window.h"
#include "windowsnatch.h"

#include "common/configuration.h"

BOOL MagicallyRelocate4Windows(RECT* screen_size);
BOOL MagicallyRelocate6Windows(RECT* screen_size);

typedef enum {
  TOP = 1 << 0,
  VERTICAL_CENTER = 1 << 1,
  BOTTOM = 1 << 2,
  LEFT = 1 << 3,
  HORIZONTAL_CENTER = 1 << 4,
  RIGHT = 1 << 5,
} WINDOW_POSITION;

BOOL MagicallyRelocateAllWindows() {
  int max_active_index = -1;
  HMONITOR screen_handle = NULL;

  for (int i = 0; i < NUMBER_OF_TARGETS; i++) {
    TARGET_WINDOW* target = GetTargetWindow(i);
    if (target == NULL) {
      continue;
    }
    max_active_index = i;
    if (screen_handle == NULL) {
      screen_handle =
        MonitorFromWindow(target->windowHandle, MONITOR_DEFAULTTONEAREST);
    }
  }

  if (max_active_index < 0 || screen_handle == NULL) {
    return FALSE;
  }

  MONITORINFO monitor_info;
  monitor_info.cbSize = sizeof(MONITORINFO);
  GetMonitorInfoA(screen_handle, &monitor_info);
  RECT screen_size = monitor_info.rcWork;

  if (max_active_index <= 3) {
    return MagicallyRelocate4Windows(&screen_size);
  }
  return MagicallyRelocate6Windows(&screen_size);
}

// https://stackoverflow.com/questions/34139450/
#define WINDOW_BORDER_LEFT 7
#define WINDOW_BORDER_TOP 0
#define WINDOW_BORDER_RIGHT (-7)
#define WINDOW_BORDER_BOTTOM (-7)

BOOL PositionWindow(HWND window, WINDOW_POSITION position, RECT* screen_size) {
  RECT window_pos;
  GetWindowRect(window, &window_pos);
  LONG window_width = window_pos.right - window_pos.left;
  LONG window_height = window_pos.bottom - window_pos.top;
  LONG x = window_pos.left;
  LONG y = window_pos.top;

  if (position & LEFT) {
    x = screen_size->left - WINDOW_BORDER_LEFT;
  } else if (position & HORIZONTAL_CENTER) {
    x = (screen_size->left + screen_size->right - window_width) / 2;
  } else if (position & RIGHT) {
    x = screen_size->right - window_width - WINDOW_BORDER_RIGHT;
  }

  if (position & TOP) {
    y = screen_size->top - WINDOW_BORDER_TOP;
  } else if (position & VERTICAL_CENTER) {
    y = (screen_size->top + screen_size->bottom - window_height) / 2;
  } else if (position & BOTTOM) {
    y = screen_size->bottom - window_height - WINDOW_BORDER_BOTTOM;
  }

  UINT flags =
    SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOOWNERZORDER |
    SWP_NOSIZE | SWP_NOZORDER;
  return SetWindowPos(window, NULL, x, y, 0, 0, flags);
}

BOOL PositionWindowsFromArray(
  RECT* screen_size, WINDOW_POSITION* positions, int lenPositions) {

  for (int i = 0; i < lenPositions; i++) {
    TARGET_WINDOW* target = GetTargetWindow(i);
    if (target) {
      BOOL ok = PositionWindow(target->windowHandle, positions[i], screen_size);
      if (!ok) {
        return ok;
      }
    }
  }
  return TRUE;
}

BOOL MagicallyRelocate4Windows(RECT* screen_size) {
  WINDOW_POSITION positions[] = {
    TOP | LEFT,
    BOTTOM | LEFT,
    TOP | RIGHT,
    BOTTOM | RIGHT,
  };

  return PositionWindowsFromArray(screen_size, positions, 4);
}

BOOL MagicallyRelocate6Windows(RECT* screen_size) {
  WINDOW_POSITION positions[] = {
    TOP | LEFT,
    BOTTOM | LEFT,
    TOP | HORIZONTAL_CENTER,
    BOTTOM | HORIZONTAL_CENTER,
    TOP | RIGHT,
    BOTTOM | RIGHT,
  };

  return PositionWindowsFromArray(screen_size, positions, 6);
}
