import serial
from io import StringIO
import matplotlib.pyplot as plt
import numpy as np
import csv

x=list()
y=list()
ser = serial.Serial('/dev/ttyACM0',9600)
ser.close()
ser.open()

reset_level = 0
image_size = 128

def parse_line(line):
    line = str(line, 'utf-8')
    f = StringIO(line)
    reader = csv.reader(f, delimiter=',') 
    return [line for line in reader]

img = np.zeros((image_size, image_size))

while True:
    line = ser.readline()
    # print(line)
    # Read whole image data
    line_num = 0
    if line == b'BEGIN_DATA\n':
        print("got signal")
        # Extract reset level
        line = ser.readline()
        data = parse_line(line)
        reset_level = int(data[0][1])

        # Extract byte data
        while True:
            line = ser.readline()
            if (line == b'END_DATA\n'):
                break

            data = np.array(parse_line(line))
            img[line_num, ...] = data[0, :-1]
            line_num+=1

        break

# Normalize the image
print(f'The measured reset level is : {reset_level}')

img = img * (1/reset_level) * 255
ser.close()

plt.imshow(img, cmap='gray', vmin=0, vmax=255)
plt.savefig("readout1.png")
plt.show()

print(img)