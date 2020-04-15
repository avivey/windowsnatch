#! /usr/bin/env python3

from collections import namedtuple, OrderedDict
import subprocess
from os import path

PinAssignment = namedtuple(
    'PinAssignment',
    ['red', 'green', 'blue', 'button1', 'button2'])


def hex(n, bytes=2):
    code = '0%dX' % (2 * bytes)
    return '0x' + format(n, code)

def get_git_version():
    dirty = subprocess.call(
        ['git', 'diff', '--quiet', 'HEAD'],
        cwd=path.dirname(__file__))

    hash = subprocess.check_output(['git', 'log', '-1', '--format=%h'])
    hash = hash.strip().decode()

    if dirty:
        hash += '-dirty'

    return hash

CONFIG = {
    'version': get_git_version(),
    'pin assignment': [
       PinAssignment(0, 1, 2, 3, 4),
       PinAssignment(5, 6, 7, 8, 9),
       PinAssignment(10, 11, 12, 24, 25),
       PinAssignment(26, 27, 28, 29, 30),
       PinAssignment(23, 22, 21, 20, 19),
       PinAssignment(18, 17, 16, 15, 14),
    ],
    'usb identifier': {
        'vendor': hex(0x16c0),
        'product': hex(0x0486),
        'usage page': hex(0xFFAB),
        'usage id': hex(0x0003),        # This encodes protocol version.
    },
    'keepalive timer': 60,    # seconds. device will wait for 2 cycles.
    'magic number': hex(0xA2, 1),
    'icd commands': {
        'PING': 0,
        'GET_VERSION': 1,
        'SET_LED': 2,
        'ENTER_PROGRAMMING_MODE': 15,
        'VERSION_STRING': 16 + 0,
        'BUTTON_PRESS': 16 + 2,
    },
}


def build_configuration():
    usb = CONFIG['usb identifier']

    conf = [
        '#ifndef WINDOWSNATCH_CONFIG_H',
        '#define WINDOWSNATCH_CONFIG_H',

        '#define USB_VENDOR_ID %s' % usb['vendor'],
        '#define USB_PRODUCT_ID %s' % usb['product'],
        '#define USB_HID_USAGE_PAGE %s' % usb['usage page'],
        '#define USB_HID_USAGE_ID %s' % usb['usage id'],

        '#define CODE_VERSION_STR "%s"' % CONFIG['version'],
        '#define CODE_VERSION_LEN %s' % len(CONFIG['version']),

        '#define NUMBER_OF_TARGETS %d' % len(CONFIG['pin assignment']),

        '#define KEEPALIVE_TIMER_SECONDS %d' % CONFIG['keepalive timer'],

        '#endif // WINDOWSNATCH_CONFIG_H',
    ]
    return conf

mask_counter = 0
def mask_value():
    global mask_counter
    v = '0b1' + mask_counter * '0'
    mask_counter += 1
    return v

def build_keydown_config():
    names = [
        '#ifndef BUTTON_NAMES_H',
        '#define BUTTON_NAMES_H',
    ]

    i = 1
    names.append('typedef enum BUTTONS {')

    buttons = OrderedDict()
    for set in CONFIG['pin assignment']:
        buttons['BUTTON%d1' % i] = set.button1
        buttons['BUTTON%d2' % i] = set.button2
        i += 1

    init, mask, select = list(), list(), list()
    for button, pin in buttons.items():
        names.append('%s = %d,' % (button, pin))
        mask.append('#define %s_MASK %s' % (button, mask_value()))
        init.append('pinMode(%s, INPUT_PULLDOWN);' % button)
        select.append(
            'if (button == %s)\n  return __is_keydown(button, %s_MASK);' % (
                button, button)
        )

    names.append('} button_t;')
    names.append('#endif // BUTTON_NAMES_H')

    incl = list()
    incl.extend(mask)
    incl.append('void init_buttons() {')
    incl.extend(init)
    incl.append('}')

    button_count = len(CONFIG['pin assignment']) * 2
    if button_count <= 8:
        incl.append('typedef uint_fast8_t button_mask_t;')
    elif button_count <= 16:
        incl.append('typedef uint_fast16_t button_mask_t;')
    elif button_count <= 32:
        incl.append('typedef uint_fast32_t button_mask_t;')
    else:
        raise Exception('No type is large enough')

    incl.append('int __is_keydown(int pin, button_mask_t mask);')
    incl.append('int is_keydown(button_t button) {')
    incl.extend(select)
    incl.append('return 0;')
    incl.append('}')

    return names, incl

def build_init_toolsets():
    o = list()
    i = 0

    sets_count = len(CONFIG['pin assignment'])

    o.append('toolset_t toolset_all_toolsets[%d] = {' % sets_count)
    for set in CONFIG['pin assignment']:
        o.append('{%d, %d, %d, %d, %s, %s},' % (
            i, set.red, set.green, set.blue, set.button1, set.button2))
        i += 1
    o.append('};')

    o.append('const int NUM_TOOLSETS = %d;' % sets_count)

    return o

def build_icd_config():
    dispatcher = list()

    header = [
        '#ifndef ICD_H',
        '#define ICD_H',
        '#include "icd.h"',
        '#define ICD_MAGIC_NUMBER %s' % CONFIG['magic number'],
    ]

    dispatcher = [
        '#include "icd_messages.h"',
        '#include "compat.h"',
        'void dispatch_incoming_message(Buffer buffer) {',
        'uint8_t msg_code = buffer[1];',
        'switch (msg_code) {',
    ]

    for command, code in CONFIG['icd commands'].items():
        header.extend([
            '#define MSG_CODE_%s  %d' % (command, code),
            'void handle_message_%s(Buffer);' % command,
        ])
        dispatcher.extend([
            'case MSG_CODE_%s: ' % command,
            'return handle_message_%s(buffer);' % command,
        ])

    header.append('#endif // ICD_H')
    dispatcher.append('return signal_error(5); \n }\n }')

    return header, dispatcher

written_files = list()
root = path.dirname(__file__)
def write_to(filename, content):
    with open(root + filename, 'w') as file:
        file.write('// @' + 'generated\n')
        file.write('// This file was generated by %s.\n' % __file__)
        file.write('\n'.join(content))
    written_files.append(root + filename)

def assert_pin_sanity():
    pins = set()
    for pinset in CONFIG['pin assignment']:
        for pin in pinset:
            assert pin not in pins, 'Pin defined twice: %s' % pin
            pins.add(pin)

if __name__ == '__main__':
    assert_pin_sanity()

    config = build_configuration()
    write_to('/common/configuration.h', config)
    icd_header, icd_dispatch = build_icd_config()
    write_to('/common/icd_messages.h', icd_header)
    write_to('/common/icd_dispatch.c', icd_dispatch)

    names, incl = build_keydown_config()
    write_to('/teensy/src/button_names.h', names)
    write_to('/teensy/src/button_setup.inc', incl)

    toolsets = build_init_toolsets()
    write_to('/teensy/src/toolsets_init.inc', toolsets)

    subprocess.call(['astyle', '-n', '-s2', '-q', '-pUH'] + written_files)

'''
The ICD looks like this:
- All messages are 64 bytes long, because I think that's how HID works.
- Byte 0: the magic number.
- Byte 1: Command.
- Bytes 2-63: payload.

For command SET_LED, payload is:
- Byte 2: number of pairs.
- Pair: Byte 0 - index of LED (0-based)
-       Byte 1 - Color (0-7), RGB, one bit per color.

For command BUTTON_PRESS:
- Byte 2: number of pairs.
- Pair: Byte 0 - index of LED (0-based)
-       Byte 1 - Which button pressed (1 or 2).

ENTER_PROGRAMMING_MODE:
- There's no extra data

VERSION_STRING:
- Bytes 2-62: NULL-terminated ASCII string starting at byte 2.
- Byte 63: Ignored.
'''
