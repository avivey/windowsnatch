#! /usr/bin/env python3

from collections import namedtuple
import subprocess
from os import path
from pprint import pprint

PinAssignment = namedtuple(
    'PinAssignment',
    ['red', 'green', 'blue', 'button1', 'button2'])


def hex(n, bytes=2):
    code = '0%dX' % (2 * bytes)
    return '0x' + format(n, code)

def get_git_version():
    dirty = subprocess.call(
        ['git', 'diff', '--quiet'],
        cwd=path.dirname(__file__))

    hash = subprocess.check_output(['git', 'log', '-1', '--format=%h'])
    hash = hash.strip().decode();

    if dirty:
        hash += '-dirty'

    return hash;

CONFIG = {
    'version': get_git_version(),
    'num windows': 6,
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
        'usage id': hex(0x0002),        # This encodes protocol version.
    },
    'magic number': hex(0xA2, 1),
    'icd commands': {
        'GET_VERSION': 1,
        'SET_LED': 2,
        'VERSION_STRING': 16 + 1,
        'BUTTON_PRESS': 16 + 2,
    }
}

if __name__ == '__main__':
    # print('a', __file__);
    pprint(CONFIG)


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


VERSION_STRING:
- NULL-terminated ASCII string starting at byte 2.
'''
