#!/usr/bin/env python

import subprocess

app = [
    'build/src/toolset.o',
    'build/src/keydown.o',
    'build/src/main.o',
]
lib = {
    'AudioStream.o': False,
    'DMAChannel.o': False,
    'EventResponder.o': 2,
    'HardwareSerial1.o': 2,
    'HardwareSerial2.o': 2,
    'HardwareSerial3.o': 2,
    'HardwareSerial4.o': 2,
    'HardwareSerial5.o': 2,
    'HardwareSerial6.o': 2,
    'IPAddress.o': False,
    'IntervalTimer.o': False,
    'Print.o': False,
    'Stream.o': False,
    'Tone.o': False,
    'WMath.o': False,
    'WString.o': False,
    'analog.o': 1,
    'avr_emulation.o': False,
    'eeprom.o': False,
    'keylayouts.o': False,
    'math_helper.o': False,
    'mk20dx128.o': 2,
    'new.o': False,
    'nonstd.o': 3,
    'pins_teensy.o': True,
    'ser_print.o': False,
    'serial1.o': 2,
    'serial2.o': 2,
    'serial3.o': 2,
    'serial4.o': 2,
    'serial5.o': 2,
    'serial6.o': False,
    'serial6_lpuart.o': 2,
    'touch.o': False,
    'usb_audio.o': False,
    'usb_desc.o': 2,
    'usb_dev.o': 1,
    'usb_flightsim.o': False,
    'usb_inst.o': 2,
    'usb_joystick.o': False,
    'usb_keyboard.o': False,
    'usb_mem.o': 2,
    'usb_midi.o': False,
    'usb_mouse.o': False,
    'usb_mtp.o': False,
    'usb_rawhid.o': True,
    'usb_seremu.o': 2,
    'usb_serial.o': False,
    'usb_touch.o': False,
    'yield.o': 1,
}

lib_build_root = 'build/teensy-core/'

used_libs = [file for file, used in lib.items() if used]
lib_files = [lib_build_root + file for file in used_libs]

cmd = [
    'tools/arm/bin/arm-none-eabi-gcc', '-Os', '-Wl,--gc-sections',
    '-mthumb', '-mcpu=cortex-m4', '-mfloat-abi=hard', '-mfpu=fpv4-sp-d16',
    '-Tteensy-core/mk66fx1m0.ld',
    '-o', 'teensy.elf',
]

cmd.extend(app)
cmd.extend(lib_files)

cmd.extend(['-lm', '-larm_cortexM4lf_math'])

subprocess.check_call(cmd)

tohex = [
    'tools/arm/bin/arm-none-eabi-objcopy', '-O', 'ihex', '-R', '.eeprom',
    "teensy.elf", "teensy.hex",
]

subprocess.check_call(tohex)

ls = ['ls', '-l', "teensy.elf", "teensy.hex"]
sizes = subprocess.check_output(ls)
sizes = sizes.splitlines()

missing = subprocess.check_output(
    'nm teensy.elf | grep U | cut -c12-', shell=True)
missing = missing.splitlines()
ok_missing = {
    'hardware_init_hook': True,
    'software_init_hook': True,
    '__stack': True,
}
missing = [m for m in missing if not ok_missing.get(m)]

with open('../data.tsv', 'a') as f:
    f.write('%s\t%s\t%s\n' % (sizes, used_libs, missing))

print(missing)

# can find defs using:
# nm --defined-only -g -A *.o | `../../search.py` | grep -o '^[^:]*:' | sort -u
