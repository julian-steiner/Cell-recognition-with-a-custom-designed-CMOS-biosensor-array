#include "sensor_driver.h"

SensorDriver::SensorDriver(SPI_Driver &driver_handle, uint8_t pin_1, uint8_t pin_2, uint8_t pin_3, uint8_t pin_4) : pin_1(pin_1),
                                                                                                                    pin_2(pin_2),
                                                                                                                    pin_3(pin_3),
                                                                                                                    pin_4(pin_4),
                                                                                                                    driver_handle(driver_handle),
                                                                                                                    calibration_level(13902)
{
    // Initialize the analog input pins
    pinMode(pin_1, INPUT);
    analogReadResolution(16);
    pinMode(pin_2, INPUT);
    analogReadResolution(16);
    pinMode(pin_3, INPUT);
    analogReadResolution(16);
    pinMode(pin_4, INPUT);
    analogReadResolution(16);
}

void SensorDriver::initialize_sensor()
{
    // Ensure that the driver has no sequence
    while (driver_handle.has_sequence())
    {
        HAL_Delay(1);
    }
    driver_handle.set_sequence(sequence_generator::RESET_SIGNAL.data(), sequence_generator::RESET_SIGNAL_SIZE, false);
}

void SensorDriver::calibrate_readout(int x, int y)
{
    // Ensure that the driver has no sequence (maybe integrate into driver at this point)
    while (driver_handle.has_sequence())
    {
        HAL_Delay(1);
    }

    // Set the pixel into the calibrate state
    current_sequence = sequence_generator::get_custom_spi_data_signal(x, y, sequence_generator::COL_CALIBRATE_DATA, sequence_generator::ROW_CALIBRATE_DATA);
    driver_handle.set_sequence(current_sequence.data(), current_sequence.size(), false);
    HAL_Delay(100);

    // Extract the calibration level
    calibration_level = analogRead(analog_pin_map(x, y));

    // Reset the pixel afterwards
    current_sequence = sequence_generator::get_custom_spi_data_signal(0, 0, sequence_generator::COL_RESET_DATA, sequence_generator::ROW_RESET_DATA);
    driver_handle.set_sequence(current_sequence.data(), current_sequence.size(), false);
}

void SensorDriver::reset_sensor()
{
    // Loop through all the pixels for now
    for (unsigned x = 0; x < 128; x++)
    {
        for (unsigned y = 0; y < 128; y++)
        {
            // Actively wait while the driver is still busy
            while (driver_handle.has_sequence())
            {
                HAL_Delay(1);
            }

            // Generate sequence and apply
            current_sequence = sequence_generator::get_custom_spi_data_signal(0, 0, sequence_generator::COL_RESET_DATA, sequence_generator::ROW_RESET_DATA);
            driver_handle.set_sequence(current_sequence.data(), current_sequence.size(), false);
        }
    }
}

void SensorDriver::read_single_pixel(int x, int y, uint16_t *buffer, int exposure_time_millis)
{
    // Ensure driver has no sequence at the time
    while (driver_handle.has_sequence())
    {
        HAL_Delay(1);
    }

    current_sequence = sequence_generator::get_custom_spi_data_signal(x, y, sequence_generator::COL_READ_DATA, sequence_generator::ROW_READ_DATA);
    driver_handle.set_sequence(current_sequence.data(), current_sequence.size(), false);

    // Expose the pixel and read the voltage
    HAL_Delay(exposure_time_millis);
    *buffer = analogRead(analog_pin_map(x, y)) - calibration_level;

    // Reset the pixel again
    current_sequence = sequence_generator::get_custom_spi_data_signal(x, y, sequence_generator::COL_RESET_DATA, sequence_generator::ROW_RESET_DATA);
    driver_handle.set_sequence(current_sequence.data(), current_sequence.size(), false);
}

void SensorDriver::read_image(uint16_t *buffer, int exposure_time_millis)
{
    // Loop through the pixels and perform readouts, saving into the buffer
    for (unsigned y = 0; y < 128; y++) {
        for (unsigned x = 0; x < 128; x++) {
            read_single_pixel(x, y, buffer++, exposure_time_millis);
        }
    }
}

uint8_t SensorDriver::analog_pin_map(int x, int y)
{
    if (x < 32)
    {
        return pin_1;
    }
    if (x < 64)
    {
        return pin_2;
    }
    if (x < 96)
    {
        return pin_3;
    }
    return 4;
}
