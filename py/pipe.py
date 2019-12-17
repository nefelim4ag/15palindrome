
import os


while True:
    data = os.read(0, 64 * 1024)
    if len(data) == 0:
        break
    os.write(1, data)


