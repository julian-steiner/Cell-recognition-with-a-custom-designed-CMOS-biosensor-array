#pragma once

#include <Arduino.h>
using register_size = u_int16_t;

/**
 * @class SPI_Driver
 * @brief Controls the GPIO pins of register J on the Arduino Giga using a 16-bit bit-sequence.
 * 
 * This class allows for the initialization of GPIO-J register pins. It updates them based on a sequence of 
 * 16-bit values.
 * 
 * **IMPORTANT**: You must call `initialize()` before calling any other method of the `SPI_Driver` class.
 */
class SPI_Driver
{
private:
    const register_size *current_bit_ptr;
    int current_index;
    int array_len;
    bool __has_sequence;
    bool repeat;

public:
    SPI_Driver();
    ~SPI_Driver();

    void initialize();
    void run();
    void set_sequence(const register_size *bit_sequence, int len, bool repeat);

    bool has_sequence();

    int get_current_index() {return current_index;}
};