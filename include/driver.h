#pragma once

#include <Arduino.h>
using register_size = u_int16_t;

/**
 * @class Driver
 * @brief Controls the GPIO pins of register J on the Arduino Giga using a 16-bit bit-sequence.
 * 
 * This class allows for the initialization of GPIO-J register pins. It updates them based on a sequence of 
 * 16-bit values.
 * 
 * **IMPORTANT**: You must call `initialize()` before calling any other method of the `Driver` class.
 */
class Driver
{
private:
    const register_size *current_bit_ptr;
    int current_index;
    int array_len;

public:
    Driver();
    ~Driver();

    void initialize();
    void run();
    void set_sequence(const register_size *bit_sequence, int len);

    bool has_sequence();
};