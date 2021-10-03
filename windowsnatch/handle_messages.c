#include <windows.h>
#include <tchar.h>

#include "windowsnatch.h"
#include "audio.h"

#include "common/icd_messages.h"

void signal_error(int count);
void ShowError(LPCTSTR body);
void SendCommandToPutty(TARGET_WINDOW *putty, BOOL btn1, BOOL btn2);
void HandleTeensyVersionString(LPCTSTR version_string);

void handle_message_VERSION_STRING(Buffer buffer) {
  TCHAR version[62];
  size_t convertedChars = 0;
  mbstowcs_s(&convertedChars, version, 62, (char*)(buffer + 2), _TRUNCATE);
  if (convertedChars == 0) {
    ShowError(_T("version str not converted"));
    return;
  }
  HandleTeensyVersionString(version);
}

void handle_message_BUTTON_PRESS(Buffer buffer) {
  int pair_count = buffer[2];

  int ptr = 3;
  while (pair_count-- > 0 && ptr < RAW_HID_BUFFER_SIZE - 1) {
    uint8_t targetId = buffer[ptr++];
    uint8_t buttonId = buffer[ptr++];

    TARGET_WINDOW *target = GetTargetWindow(targetId);
    if (!target) {
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

void handle_message_ENCODER_CHANGE(Buffer buffer) {
  int ptr = 2;
  int id = buffer[ptr++];
  int old = (int8_t)buffer[ptr++];
  int new = (int8_t)buffer[ptr++];

  if (id > 2) {
    // For now, all (2) encoders do the same thing.
  }

  const int DELTA = 256; // Total size of wheel
  // Buttom half of the wheel is within 1/4 of the zero.
  const int QUARTER = DELTA / 4;

  int change = new - old;

  if (change == 0) {
    // TODO remove this hack, have proper press message.
    VolumeMute();
    return;
  }
  if (change > QUARTER) {
    change -= DELTA;
  }
  else if (change < -QUARTER) {
    change += DELTA;
  }

  VolumeChange(change);
}

void handle_message_PING(Buffer buffer) {
  signal_error(101);
}

void handle_message_GET_VERSION(Buffer buffer) {
  signal_error(117);
}

void handle_message_SET_LED(Buffer buffer) {
  signal_error(102);
}

void handle_message_ENTER_PROGRAMMING_MODE(Buffer buffer) {
  signal_error(115);
}
