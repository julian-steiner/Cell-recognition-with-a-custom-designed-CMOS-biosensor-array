#include "driver.h"

SPI_Driver::SPI_Driver() : current_bit_ptr(nullptr), current_index(0), array_len(0), repeat(false), __has_sequence(false) {}

SPI_Driver::~SPI_Driver()
{
}

/**
 * @brief Initializes the GPIO-J register pins.
 *
 */
void SPI_Driver::initialize()
{
    pinMode(25, OUTPUT);
    pinMode(27, OUTPUT);
    pinMode(29, OUTPUT);
    pinMode(31, OUTPUT);
    pinMode(33, OUTPUT);
    pinMode(35, OUTPUT);
    pinMode(37, OUTPUT);
    pinMode(38, OUTPUT);
}

/**
 * @brief Initializes the sequence of 16-bit values.
 *
 * @param sequence Pointer to the sequence of 16-bit elements.
 * @param len The number of 16-bit elements inside the bit sequence.
 * @param repeat Repeat sequence upon ending for testing purposes.
 */
void SPI_Driver::set_sequence(const register_size *sequence, int len, bool repeat)
{
    current_bit_ptr = sequence;
    current_index = 0;
    array_len = len;
    __has_sequence = true;

    this->repeat = repeat;
}

/**
 * @brief Writes one 16-bit element of the sequence to the GPIO-J register. When sequence finished, it writes NO_SIGNAL.
 *
 */
void SPI_Driver::run()
{
    if (current_index >= array_len)
    {
        if (!repeat)
        {
            current_bit_ptr = nullptr;
            __has_sequence = false;
        }
        else
        {
            current_index = 0;
        }
    }

    if (current_bit_ptr != nullptr)
    {
        GPIOJ->ODR = current_bit_ptr[current_index];
        ++current_index;
    }
    else
    {
        __has_sequence = false;
        GPIOJ->ODR = 1 << 5 | 1 << 1;
    }
}

bool SPI_Driver::has_sequence()
{
    return __has_sequence;
}