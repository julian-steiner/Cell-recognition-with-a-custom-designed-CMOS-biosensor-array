# Direct cell recognition with a custom-designed CMOS biosensor array

## Quick start
This project was implemented using an Arduino Giga STM32H7. 
To quickly get started, connect the MCU to the motherboard and upload the code.
The python receiver in the Python-serial-receiver needs to be started during the readout.

While the MCU is taking the image, the on-board LED is lit up in red and it transitions to green as soon as the image is ready in about 30s.

## Image creating on the microcontroller
### SPI Sequence
The SPI data payloads can be modified in the sequence_generator.h header file.

To modify the sequence structure, the sequence generator in the .cpp file needs to be modified.

## Image creation
The main parameters to be adjusted are in the main.cpp file.
There, you can change the exposure time used to take an image with EXPOSURE_TIME_MILLIS.
You can change which path is used (DVN or DVP) through a parameter while creating the sensor_driver in main.cpp.

In the setup function, the pixel used to calibrate the readout can be changed in case pixel (0, 0) would be broken.

## Image processing on PC
The pipeline for image processing can easily be adjusted in the main.py file in Python-serial-receiver class.
Several processing algorithms are present in the generate_image_class.py file which can be chained to create custom pipelines.