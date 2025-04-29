import serial
from io import StringIO
import matplotlib.pyplot as plt
import numpy as np
import csv
import os

class GenerateImage:
    def __init__(self, port='/dev/ttyACM0', baudrate=9600, image_size=128):
        self.port = port
        self.baudrate = baudrate
        self.image_size = image_size
        self.reset_level = 0
        self.img = np.zeros((image_size, image_size), dtype=np.uint16)
        self.ser = serial.Serial(port, baudrate)

        if os.path.exists("dark_frame.npy"):
            self.dark_frame_array = np.load("dark_frame.npy")
        else:
            self.dark_frame_array = np.zeros((image_size, image_size), dtype=np.uint16)

    def open_connection(self):
        if self.ser.is_open:
            self.ser.close()
        self.ser.open()

    def close_connection(self):
        self.ser.close()

    def parse_line(self, line):
        line = str(line, 'utf-8')
        f = StringIO(line)
        reader = csv.reader(f, delimiter=',')
        return [line for line in reader]
    
    def dark_frame(self):
        self.open_connection()
        while True:
            line = self.ser.readline()
            if line == b'BEGIN_DATA\n':
                line_num = 0
                line = self.ser.readline()
                data = self.parse_line(line)
                while True:
                    line = self.ser.readline()
                    if line == b'END_DATA\n':
                        break
                    data = np.array(self.parse_line(line))
                    row_data = np.array(data[0,:-1], dtype=np.uint16)

                    self.dark_frame_array[line_num, ...] = row_data
                    np.save("dark_frame.npy", self.dark_frame_array)

                    line_num += 1
                break
        self.close_connection()

    def read_image(self):
        self.open_connection()
        while True:
            line = self.ser.readline()
            if line == b'BEGIN_DATA\n':
                line_num = 0
                line = self.ser.readline()
                data = self.parse_line(line)
                self.reset_level = int(data[0][1])
                while True:
                    line = self.ser.readline()
                    if line == b'END_DATA\n':
                        break
                    data = np.array(self.parse_line(line))
                    row_data = np.array(data[0,:-1], dtype=np.uint16)

                    corrected = np.clip(row_data - self.dark_frame_array[line_num], 0, 65535)
                    self.img[line_num, :] = corrected.astype(np.uint16)

                    line_num += 1
                break
        self.close_connection()

    def show_image(self):
        plt.imshow(self.img, cmap='gray', vmin=0, vmax=self.reset_level)
        plt.show()

    def save_image(self, filename="readout.png"):
        plt.imshow(self.img, cmap='gray', vmin=0, vmax=self.reset_level)
        plt.savefig(filename)
