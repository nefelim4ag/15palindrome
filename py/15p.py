#!/usr/bin/env pyton3


import sys
import threading
import queue

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

Q = queue.Queue()
result = None

def thread_routine():
    global result
    while True:
        p = Q.get()
        Q.task_done()
        if p is None:
            break
        data, offset = p
        #print(len(data), offset)
        start_off, substr = find_15_p(data, offset)
        if start_off is not None:
            result = (start_off, substr)
            break
    #print('thread exit')


threads = []
num_threads = 8

for ii in range(num_threads):
    #print('start thread', ii)
    t = threading.Thread(target=thread_routine)
    t.start()
    threads.append(t)



_s = bytes()
_input_offset = 0
while result is None:
    data = sys.stdin.buffer.read(4096 * 1024)
    if len(data) == 0:
        break

    Q.put((_s + data, _input_offset))
    #start_off, substr = find_15_p(_s + data, _input_offset)
    #if start_off is not None:
    #    print('SUCCESS\n', start_off, substr)
    #    quit(0)

    _input_offset += len(_s) + len(data) - 14
    _s = data[-14:]

for i in range(num_threads):
    Q.put(None)

#Q.join()

if result is None:
    print('FAIL\n', _input_offset, _s.decode('ascii'))
else:
    print('SUCCESS\n', result[0], result[1])

for t in threads:
    t.join()



