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
    def __init__(self, port='COM3', baudrate=9600, image_size=128):
        self.port = port
        self.baudrate = baudrate
        self.image_size = image_size
        self.img = np.zeros((image_size, image_size), dtype=float)
        self.ser = serial.Serial(port, baudrate)

        if os.path.exists("dark_frame.npy"):
            self.dark_frame_array = np.load("dark_frame.npy")
        else:
            self.dark_frame_array = np.zeros((image_size, image_size), dtype=float)

        if os.path.exists("light_frame.npy"):
            self.light_frame_array = np.load("light_frame.npy")
        else:
            self.light_frame_array = np.zeros((image_size, image_size), dtype=float)

    def open_connection(self):
        self.ser.close()
        self.ser.open()
        print("connection opened")

    def close_connection(self):
        self.ser.close()
        print("connection closed")

    def parse_line(self, line):
         line = str(line, 'utf-8')
         f = StringIO(line)
         reader = csv.reader(f, delimiter=',')
         return [line for line in reader]
    
    def clean_dead_pixels(self, kernel_size=5, thresh_fraction=0.85):
        self.img = cv2.GaussianBlur(self.img, (kernel_size, kernel_size), 0)
        # self.img = np.minimum(self.img, high)
        # thresh = int(high * thresh_fraction)
        # mask   = (self.img >= thresh)
        # img_u16 = self.img.astype(np.uint16)
        # med_u16 = cv2.medianBlur(img_u16, kernel_size)
        # lf.img[mask] = med_u16[mask].astype(self.img.dtype)


    def edge_detection(self, gaussian_kernel=(5, 5), sigma=1.0, low_thresh=150, high_thresh=250):
        img8 = (self.img.astype(np.float32) / 65535 * 255.0).clip(0,255).astype(np.uint8)
        blurred = cv2.GaussianBlur(img8, gaussian_kernel, sigma)
        edges = cv2.Canny(blurred, low_thresh, high_thresh)
        plt.figure()
        plt.imshow(edges, cmap='gray')
        plt.axis('off')
        plt.show()

    def component_analysis(self):
        # 3. Find Connected Components
        # Use cv2.CC_STAT_AREA and cv2.CC_STAT_BBOX to get area and bounding box
        self.img = self.img * 255 / 65535
        _, thresh = cv2.threshold(self.img.astype('uint8'),0,65535,cv2.THRESH_BINARY+cv2.THRESH_OTSU)
        num_labels, labels, stats, centroids = cv2.connectedComponentsWithStats(thresh, 8, cv2.CV_32S)

        # 4. and 5. Analyze and Filter Components
        # Create a black image to draw the filtered components
        output_img = np.zeros_like(self.img)

        # Iterate through each component (skip the background component at index 0)
        for i in range(1, num_labels):
            area = stats[i, 4] # CC_STAT_AREA

            # Define filtering criteria
            # These values will need to be tuned based on the size and shape of your cells
            min_cell_area = 2  # Minimum area for a component to be considered a cell

            # Check if the component meets the criteria for a cell
            if min_cell_area < area:
                # If it's a cell, draw it on the output image
                output_img[labels == i] = 255
        
        self.img = output_img / 255 * 65535

    def dark_frame(self):
        self.open_connection()

        while True:
            line = self.ser.readline()
            # print(line)
            # Read whole image data
            line_num = 0
            if line == b'BEGIN_DATA\n':
                print("got signal")
                # Extract reset level
                line = self.ser.readline()
                data = self.parse_line(line)
                reset_level = int(data[0][1])

                # Extract byte data
                while True:
                    line = self.ser.readline()
                    if (line == b'END_DATA\n'):
                        break

                    data = np.array(self.parse_line(line))
                    self.dark_frame_array[line_num, ...] = data[0, :-1]
                    line_num+=1

                break

        self.close_connection()

        print(f"Reset level received : {reset_level}")
        plt.imshow(self.dark_frame_array, cmap='gray', vmin=0, vmax=reset_level)
        plt.show()

        np.save("dark_frame.npy", self.dark_frame_array)

    def light_frame(self):
        self.open_connection()

        while True:
            line = self.ser.readline()
            # print(line)
            # Read whole image data
            line_num = 0
            if line == b'BEGIN_DATA\n':
                print("got signal")
                # Extract reset level
                line = self.ser.readline()
                data = self.parse_line(line)
                reset_level = int(data[0][1])

                # Extract byte data
                while True:
                    line = self.ser.readline()
                    if (line == b'END_DATA\n'):
                        break

                    data = np.array(self.parse_line(line))
                    self.light_frame_array[line_num, :] = data[0, :-1]
                    line_num+=1

                break

        self.close_connection()
        print(f"Reset level received : {reset_level}")
        print("Saving image")
        plt.imshow(self.light_frame_array, cmap='gray', vmin=0, vmax=reset_level)
        plt.show()
        np.save("light_frame.npy", self.light_frame_array)

    def read_image(self, load=False):
        if load:
            self.img = np.load("image.npy")
            reset_level=13000
        else:
            self.open_connection()
            while True:
                line = self.ser.readline()
                if line == b'BEGIN_DATA\n':
                    print("Received image")
                    line_num = 0
                    line = self.ser.readline()
                    data = self.parse_line(line)
                    reset_level = int(data[0][1])
                    # self.reset_level = int(data[0][1])
                    while True:
                        line = self.ser.readline()
                        if line == b'END_DATA\n':
                            break
                        data = np.array(self.parse_line(line))
                        row_data = np.array(data[0,:-1], dtype=float)

                        # Store the corrected row in the image array
                        self.img[line_num, :] = row_data

                        line_num += 1
                    break
            self.close_connection()

        # self.img = self.img * 65535 / reset_level
        # plt.imshow(self.img, cmap='gray', vmin=0, vmax=reset_level)
        # plt.show()

        np.save("image.npy", self.img)

    def apply_flat_field_correction(self):
        for y in range(128):
            for x in range(128):
                if (self.light_frame_array[y, x] - self.dark_frame_array[y, x] == 0):
                    print("Encountered zero")
                    continue

                self.img[y, x] = (self.img[y, x] - self.dark_frame_array[y, x])/(self.light_frame_array[y, x] - self.dark_frame_array[y, x])

        # print(self.img)
        self.img *= np.median((self.light_frame_array-self.dark_frame_array).flatten())*3.5
        print(self.img)

    def subtract_calibration_image(self):
        self.img = self.light_frame_array - self.img
        self.img *= 15
        self.img = np.clip(self.img, 0, 65535)


    def show_image(self):
        plt.imshow(self.img, cmap='gray', vmin=0, vmax=65535)
        plt.show()

    def save_image(self, filename="readout.png"):
        plt.imshow(self.img, cmap='gray', vmin=0, vmax=65535)
        plt.savefig(filename)