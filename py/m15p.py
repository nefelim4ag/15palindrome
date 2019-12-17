#!/usr/bin/env pyton3

# 15 palindrom search in ASCII input
# Multiprocess implementation

import sys, os
from multiprocessing import Pool

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


pool = Pool(processes=8)
results = []

_s = bytes()
_input_offset = 0
while True:
    data = os.read(0, 64 * 1024)
    if len(data) == 0:
        break

    results.append(pool.apply_async(find_15_p, (_s + data, _input_offset,)))

    _input_offset += len(_s) + len(data) - 14
    _s = data[-14:]

print('input complete')

#pool.close()
#pool.join()

for r in results:
    p = r.get()
    if p[0] is not None:
        print('SUCCESS\n', p[0], p[1])
        quit(0)

print('FAIL\n', _input_offset, _s.decode('ascii'))





