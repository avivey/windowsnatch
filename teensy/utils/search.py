#! /usr/bin/env python

missing = ['analog_init', 'analogWriteDAC0', 'analogWriteDAC1', '_exit', ]

cmd = ['grep', '--color=always', '-w']

for symbol in missing:
    cmd.append('-e')
    cmd.append(symbol)

print(' '.join(cmd))
