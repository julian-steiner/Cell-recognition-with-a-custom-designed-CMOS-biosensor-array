#pragma once
#include "driver.h"
#include "sequence_generator.h"

class SensorDriver {
private: 
    uint8_t pin_1;
    uint8_t pin_2;
    uint8_t pin_3;
    uint8_t pin_4;
    int debug_mode_limit = 40;
    bool debug_mode;
    SPI_Driver& driver_handle;
    std::array<register_size, 62> current_sequence;

    /**
     * @brief Returns the analog pin a pixel is assigned to
     * 
     * @param x 
     * @param y 
     * @return uint8_t 
     */
    uint8_t analog_pin_map(int x, int y);

public:
    int calibration_level;
    SensorDriver(SPI_Driver& driver_handle, uint8_t pin_1, uint8_t pin_2, uint8_t pin_3, uint8_t pin_4, bool debug_mode);

    void initialize_sensor();

    void calibrate_readout(int x, int y);

    /**
     * @brief Reset all photodiodes in the camera sensor to the reset voltage
     * 
     */
    void reset_sensor();

    /**
     * @brief Reads a single pixel and stores the result in the buffer specified
     * IMPORTANT: Pixel needs to be reset before this method it assumes the pixel is reset, the pixel is reset again after the measurement
     * 
     * @param x Pixel x coordinate
     * @param y Pixel y coordinate
     * @param buffer The buffer where to store the read out data
     * @param exposure_time_millis The exposure time
     */
    void read_single_pixel(int x, int y, int *buffer, int exposure_time_millis);

    void reset_single_pixel(int x, int y);

    /**
     * @brief Reads an image from the sensor
     * 
     * @param buffer The buffer to store the image (ensure that the size is big enough (128*128))
     * @param exposure_time_millis Exposure time
     */
    void read_image(int* buffer, int exposure_time_millis);
};