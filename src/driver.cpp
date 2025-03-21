#include "driver.h"

Driver::Driver() : current_bit_ptr(nullptr), current_index(0), array_len(0), repeat(false) {}

Driver::~Driver()
{
}

/**
 * @brief Initializes the GPIO-J register pins.
 * 
 */
void Driver::initialize(){

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
void Driver::set_sequence(const register_size *sequence, int len, bool repeat){
    current_bit_ptr = sequence;
    current_index = 0;
    array_len = len;

    this->repeat = repeat;
}

/**
 * @brief Writes one 16-bit element of the sequence to the GPIO-J register. When sequence finished, it writes NO_SIGNAL.
 * 
 */
void Driver::run(){
    if(current_index >= array_len){
        if(!repeat){
            current_bit_ptr = nullptr;
        }
        else{
            current_index = 0;
        }
    }
    
    if(current_bit_ptr!=nullptr){
        Serial.println(current_bit_ptr[current_index]);
        GPIOJ->ODR = current_bit_ptr[current_index];
        ++current_index;
    }
    else {
        GPIOJ->ODR = 1 << 5 | 1 << 1;
    }
}

bool Driver::has_sequence(){
    return (current_bit_ptr != nullptr);
}