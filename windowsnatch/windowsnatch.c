#undef UNICODE
#define UNICODE
#include <windows.h>
#include <stdio.h>

#define LEN_BUFF_SHORT (10)
#define LEN_BUFF_LONG (50)

TCHAR buff[LEN_BUFF_LONG];
TCHAR *targetClass = TEXT("PuTTY");
TCHAR *targetTitle = TEXT("target");

HWND targetWindow = NULL;

void findMyPutty();

int main()
{
  int len;

  findMyPutty();
  if (targetWindow == NULL) {
    printf("didn't find the target window");
    return 1;
  }

  len = GetWindowText(targetWindow, buff, LEN_BUFF_LONG);
  printf("Title len = %d\n[%S]\n", len, buff);

  TCHAR magicMarker = buff[lstrlen(targetTitle)];
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

  return 0;
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
