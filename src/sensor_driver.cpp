#include "sensor_driver.h"
#include <memory>

SensorDriver::SensorDriver(SPI_Driver &driver_handle, uint8_t pin_1, uint8_t pin_2, uint8_t pin_3, uint8_t pin_4, bool exp, bool debug_mode) : pin_1(pin_1),
                                                                                                                                               pin_2(pin_2),
                                                                                                                                               pin_3(pin_3),
                                                                                                                                               pin_4(pin_4),
                                                                                                                                               driver_handle(driver_handle),
                                                                                                                                               calibration_level(13902),
                                                                                                                                               debug_mode(debug_mode),
                                                                                                                                               exp(exp)
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

    current_sequence = (register_size*)malloc(62*sizeof(register_size));
    current_sequence_size = 62;

    // Initialize the signals
    if (exp)
    {
        ROW_RESET_DATA = sequence_generator::ROW_RESET_DATA_EXP;
        ROW_READ_DATA = sequence_generator::ROW_READ_DATA_EXP;
        ROW_CALIBRATE_DATA = sequence_generator::ROW_CALIBRATE_DATA_EXP;
        COL_RESET_DATA = sequence_generator::COL_RESET_DATA_EXP;
        COL_READ_DATA = sequence_generator::COL_READ_DATA_EXP;
        COL_CALIBRATE_DATA = sequence_generator::COL_CALIBRATE_DATA_EXP;
    } else {
        ROW_RESET_DATA = sequence_generator::ROW_RESET_DATA_EXN;
        ROW_READ_DATA = sequence_generator::ROW_READ_DATA_EXN;
        ROW_CALIBRATE_DATA = sequence_generator::ROW_CALIBRATE_DATA_EXN;
        COL_RESET_DATA = sequence_generator::COL_RESET_DATA_EXN;
        COL_READ_DATA = sequence_generator::COL_READ_DATA_EXN;
        COL_CALIBRATE_DATA = sequence_generator::COL_CALIBRATE_DATA_EXN;
    }
}

SensorDriver::~SensorDriver() {
    free(current_sequence);
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
    while (driver_handle.has_sequence()){}

    // Set the pixel into the calibrate state
    sequence_generator::get_custom_spi_data_signal(current_sequence, x, y, COL_CALIBRATE_DATA, ROW_CALIBRATE_DATA);
    driver_handle.set_sequence(current_sequence, current_sequence_size, false);
    HAL_Delay(100);

    // Extract the calibration level
    calibration_level = analogRead(analog_pin_map(x, y));
}

void SensorDriver::reset_sensor()
{
    // Debug mode that doesn't do all the pixels
    unsigned upper_bound = 128;
    if (debug_mode)
    {
        upper_bound = debug_mode_limit;
    }

    // Allocate the pingpong buffer
    
    register_size* col_reset_seq = (register_size*)malloc(62*upper_bound*sizeof(register_size));
    register_size* col_reset_seq_t = (register_size*)malloc(62*upper_bound*sizeof(register_size));

    // Loop through all the pixels for now
    for (unsigned y = 0; y < upper_bound; y++)
    {
        // Fill the sequence for the whole row
        for (unsigned x = 0; x < upper_bound; x++) {
            sequence_generator::get_custom_spi_data_signal(col_reset_seq_t+(x*62), x, y, COL_RESET_DATA, ROW_RESET_DATA);
        }

        // Actively wait while the driver is still busy
        while (driver_handle.has_sequence()) {}

        // Generate sequence and apply
        std::swap(col_reset_seq, col_reset_seq_t);
        driver_handle.set_sequence(col_reset_seq, 62*upper_bound, false);
    }

    // Wait until the driver finishes and free the memory (also remove sequence from driver)
    while (driver_handle.has_sequence()) {}
    free(col_reset_seq_t);
    free(col_reset_seq);
}

void SensorDriver::read_single_pixel(int x, int y, int *buffer, int exposure_time_millis)
{
    // Ensure driver has no sequence at the time
    while (driver_handle.has_sequence()) {}

    // The actual readout
    sequence_generator::get_custom_spi_data_signal(current_sequence, x, y, COL_READ_DATA, ROW_READ_DATA);
    driver_handle.set_sequence(current_sequence, current_sequence_size, false);

    // Expose the pixel and read the voltage
    HAL_Delay(exposure_time_millis);
    *buffer = calibration_level - analogRead(analog_pin_map(x, y));

    // Reset the pixel again
    sequence_generator::get_custom_spi_data_signal(current_sequence, x, y, COL_RESET_DATA, ROW_RESET_DATA);
    driver_handle.set_sequence(current_sequence, current_sequence_size, false);
}

void SensorDriver::reset_single_pixel(int x, int y)
{
    // Actively wait while the driver is still busy
    while (driver_handle.has_sequence()){}

    // Generate sequence and apply
    sequence_generator::get_custom_spi_data_signal(current_sequence, x, y, COL_RESET_DATA, ROW_RESET_DATA);
    driver_handle.set_sequence(current_sequence, current_sequence_size, false);
}

void SensorDriver::read_image(int *buffer, int exposure_time_millis)
{
    read_image_slow(buffer, exposure_time_millis);
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
    return pin_4;
}

void SensorDriver::read_image_slow(int *buffer, int exposure_time_millis)
{
    // Handle debug mode for faster debugging
    int upper_bound = 128;
    if (debug_mode)
    {
        upper_bound = debug_mode_limit;
    }

    // Loop through the pixels and perform readouts, saving into the buffer
    for (unsigned y = 0; y < upper_bound; y++)
    {
        for (unsigned x = 0; x < upper_bound; x++)
        {
            read_single_pixel(x, y, buffer++, exposure_time_millis);
        }
    }
}

void SensorDriver::read_image_optimized(int *buffer, int exposure_time_millis)
{
}
