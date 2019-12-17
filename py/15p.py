#!/usr/bin/env pyton3

# 15 palindrom search in ASCII input
# Optimized single-thread implementation 

import sys, os

def find_15_p(data, offset):
    start_i = 0
    while start_i + 15 <= len(data):
        if (data[start_i] == data[start_i + 14] and
           data[start_i + 1] == data[start_i + 13] and
           data[start_i + 2] == data[start_i + 12] and
           data[start_i + 3] == data[start_i + 11] and
           data[start_i + 4] == data[start_i + 10] and
           data[start_i + 5] == data[start_i + 9] and
           data[start_i + 6] == data[start_i + 8]):
            return start_i + offset, data[start_i:start_i+15]
        start_i += 1
    return None, None

read_buffer_size = 1024 * 64

_s = bytes()
_input_offset = 0
while True:
    data = os.read(0, read_buffer_size)
    if len(data) == 0:
        break

    start_off, substr = find_15_p(_s + data, _input_offset)
    if start_off is not None:
        print('SUCCESS\n', start_off, substr)
        quit(0)

    _input_offset += len(_s) + len(data) - 14
    _s = data[-14:]

print('FAIL\n', _input_offset, _s.decode('ascii'))
