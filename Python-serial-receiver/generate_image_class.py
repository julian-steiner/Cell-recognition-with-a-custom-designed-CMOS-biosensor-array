import serial
from io import StringIO
import matplotlib.pyplot as plt
import numpy as np
import csv
import os
import cv2

#TODO: do dark frame, file-save and array subtraction inside c++ for efficiency and only read_image inside python?
# Type int not ideal for image generation, uint16 was out of bounds?

class GenerateImage:
    def __init__(self, port='/dev/cu.usbmodem14301', baudrate=9600, image_size=128):
        self.port = port
        self.baudrate = baudrate
        self.image_size = image_size
        self.reset_level = 0
        self.reset_level_array = np.zeros(image_size, dtype=int)
        self.img = np.zeros((image_size, image_size), dtype=int)
        self.ser = serial.Serial(port, baudrate)

        if os.path.exists("dark_frame_average.npy"):
            self.dark_frame_array = np.load("dark_frame_average.npy")
        else:
            self.dark_frame_array = np.zeros((image_size, image_size), dtype=int)

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
    
    def clean_dead_pixels(self, kernel_size=5, thresh_fraction=0.85):
        self.img = np.minimum(self.img, self.reset_level)
        thresh = int(self.reset_level * thresh_fraction)
        mask   = (self.img >= thresh)
        img_u16 = self.img.astype(np.uint16)
        med_u16 = cv2.medianBlur(img_u16, kernel_size)
        self.img[mask] = med_u16[mask].astype(self.img.dtype)


    def edge_detection(self, gaussian_kernel=(5, 5), sigma=1.0, low_thresh=200, high_thresh=250):
        img8 = (self.img.astype(np.float32) / self.reset_level * 255.0).clip(0,255).astype(np.uint8)
        blurred = cv2.GaussianBlur(img8, gaussian_kernel, sigma)
        edges = cv2.Canny(blurred, low_thresh, high_thresh)
        plt.figure()
        plt.imshow(edges, cmap='gray')
        plt.axis('off')
        plt.show()


        # Display the edges
        plt.imshow(edges, cmap='gray')
        plt.title('Canny Edge Detection')
        plt.axis('off')
        plt.show()

    def dark_frame(self, add_to_average=True):
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
                    row_data = np.array(data[0,:-1], dtype=int)

                    #dark frame averager
                    #TODO: Counter to know how many dark frame images made.
                    if add_to_average:
                        self.dark_frame_array[line_num, :] = (row_data + self.dark_frame_array[line_num, :])/2
                    else:
                        self.dark_frame_array[line_num, :] = row_data

                    line_num += 1
                break
        self.close_connection()

        if add_to_average:
            np.save("dark_frame_average.npy", self.dark_frame_array)
        else:
            np.save("dark_frame_single.npy", self.dark_frame_array)


    def read_image(self):
        self.open_connection()
        while True:
            line = self.ser.readline()
            if line == b'BEGIN_DATA\n':
                line_num = 0
                line = self.ser.readline()
                data = self.parse_line(line)
                # self.reset_level = int(data[0][1])
                while True:
                    line = self.ser.readline()
                    if line == b'END_DATA\n':
                        break
                    data = np.array(self.parse_line(line))
                    row_data = np.array(data[0,:-1], dtype=int)

                    corrected = np.clip(row_data - self.dark_frame_array[line_num], 0, 65535)
                    self.img[line_num, :] = corrected.astype(int)

                    #Determines the reset level. Very inefficient so far.
                    # for pixel_value in self.img[line_num, :]:
                    #     if pixel_value>self.reset_level:
                    #         self.reset_level = pixel_value

                    self.reset_level_array[line_num] = max(self.img[line_num, :])

                    line_num += 1
                break
        self.close_connection()
        self.reset_level = self.reset_level_array.min()


    def show_image(self):
        plt.imshow(self.img, cmap='gray', vmin=0, vmax=self.reset_level)
        plt.show()

    def save_image(self, filename="readout.png"):
        plt.imshow(self.img, cmap='gray', vmin=0, vmax=self.reset_level)
        plt.savefig(filename)