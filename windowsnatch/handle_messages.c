#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include "windowsnatch.h"

#include "common/icd_messages.h"

void signal_error(int count);
void ShowError(LPCTSTR body);
void SendCommandToPutty(TARGET_WINDOW *putty, BOOL btn1, BOOL btn2);

void handle_message_VERSION_STRING(Buffer buffer) {
  signal_error(17);
}

void handle_message_BUTTON_PRESS(Buffer buffer) {
  int pair_count = buffer[2];

  int ptr = 3;
  while (pair_count-- > 0 && ptr < RAW_HID_BUFFER_SIZE - 1) {
    uint8_t targetId = buffer[ptr++];
    uint8_t buttonId = buffer[ptr++];

    TARGET_WINDOW *target = GetTargetWindow(targetId);
    if (!IsTargetWindowActive(target)) {
      continue;
    }

    switch (buttonId) {
    case 1:
      return SendCommandToPutty(target, TRUE, FALSE);
    case 2:
      return SendCommandToPutty(target, FALSE, TRUE);
    default:
      ShowError(_T("Unexpected buttonId"));
      return;
    }
  }
}


void handle_message_GET_VERSION(Buffer buffer) {
  signal_error(101);
}

void handle_message_SET_LED(Buffer buffer) {
  signal_error(102);
}

void handle_message_ENTER_PROGRAMMING_MODE(Buffer buffer) {
  signal_error(115);
}
