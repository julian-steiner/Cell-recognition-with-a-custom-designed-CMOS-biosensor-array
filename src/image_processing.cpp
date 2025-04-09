#include <Arduino.h>
#include <SD.h>
#include "image_processing.h"

//TODO: define sd-card pin
#define SD_CS_PIN 10  // Define SD card chip select-pin

using register_size = u_int16_t;

ProcessImage::ProcessImage() : ptr_pxl_zero_zero(nullptr), height(128), width(128), counter(1) {}

ProcessImage::~ProcessImage()
{
}

//takes the pointer to array[0][0], height and width information (usually 128x128)
void ProcessImage::initialize(const register_size *array_ptr, const int h, const int w){
    height = h;
    width = w;
    ptr_pxl_zero_zero = array_ptr;

    snprintf(filename, sizeof(filename), "image.raw");
    counter = 1;
}

// Initialize SD card
void ProcessImage::setupSD() {
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed.");
    return;
  }
  Serial.println("SD card initialized.");
}

bool ProcessImage::fileExists(const char* filename) {
    File file = SD.open(filename, FILE_READ);
    if (file) {
      file.close();  // File exists, close it
      return true;
    }
    return false;  // File doesn't exist
}

// Write image data to SD card
void ProcessImage::writeImage() {

    // Check if file already exists, and if it does, modify the name
    while (fileExists(filename)) {
        snprintf(filename, sizeof(filename), "image_%d.raw", counter);
        ++counter;
    }

    File file = SD.open(filename, FILE_WRITE); // Create a raw file to store the image

    // Write the image from raw 16-bit grayscale data
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {

            //Computes pointer to [y][x] element ("1D-array of pointers")
            register_size pixel = *(ptr_pxl_zero_zero + y*width + x);

            file.write((pixel >> 8) & 0xFF);  // Write high byte
            file.write(pixel & 0xFF);         // Write low byte
        }
    }

    file.close();
}