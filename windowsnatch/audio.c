#include <windows.h>
#include "audio.h"

void VolumeChange(int amt) {
  if (amt == 0) {
    return;
  }

// TODO group to one big SendInput call?
  INPUT ip;
  ip.type = INPUT_KEYBOARD;
  ip.ki.wVk = VK_VOLUME_UP;

  if (amt < 0) {
    ip.ki.wVk = VK_VOLUME_DOWN;
    amt *= -1;
  }

  while (amt > 0) {
    ip.ki.dwFlags = 0;
    SendInput(1, &ip, sizeof(INPUT));
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));

    amt --;
  }

}

void VolumeMute() {
  INPUT ip;
  ip.type = INPUT_KEYBOARD;
  ip.ki.wVk = VK_VOLUME_MUTE;
  ip.ki.dwFlags = 0;
  SendInput(1, &ip, sizeof(INPUT));
  ip.ki.dwFlags = KEYEVENTF_KEYUP;
  SendInput(1, &ip, sizeof(INPUT));
}
