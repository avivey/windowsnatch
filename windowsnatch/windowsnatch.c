#undef UNICODE
#define UNICODE
#include <windows.h>
#include <stdio.h>

#define LEN_BUFF_SHORT (10)

TCHAR title[LEN_BUFF_SHORT];
TCHAR className[LEN_BUFF_SHORT];
TCHAR *targetClass = TEXT("PuTTY");
TCHAR *targetTitle = TEXT("target");

HWND targetWindow = NULL;

void findMyPutty();

int main()
{
    findMyPutty();
    if (targetWindow == NULL) {
        printf("didn't find the target window");
        return 1;
    }


    return 0;
}

BOOL CALLBACK callback(HWND hWnd, LPARAM lParam)
{
    int len;

    if (targetWindow != NULL) {
        return TRUE;
    }

    len = GetClassName(hWnd, className, LEN_BUFF_SHORT);
    if (len <= 0  ||
            lstrcmp(className, targetClass) != 0) {
        return TRUE;
    }
    len = GetWindowText(hWnd, title, lstrlen(targetTitle) + 1);
    if (len > 0 &&
            lstrcmp(title, targetTitle) == 0) {
        targetWindow = hWnd;
    }

    // printf("window: [%S], [%S]\n", title, className);
    return TRUE;
}

void findMyPutty() {
    EnumWindows(callback, 0);
}
