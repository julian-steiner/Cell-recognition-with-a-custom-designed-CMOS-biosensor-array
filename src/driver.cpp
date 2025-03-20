#include <Arduino.h>

//BEWARE: register_size in different files is different. TODO: Have one unified register_size type.

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
    register_size *current_bit_ptr;
    int current_index;
    int array_len;

public:
    Driver();
    ~Driver();

    void initialize();
    void run();
    void set_sequence(register_size *bit_sequence, int len);
};

Driver::Driver() : current_bit_ptr(nullptr), current_index(0), array_len(0) {}

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
 */
void Driver::set_sequence(register_size *sequence, int len){
    current_bit_ptr = sequence;
    current_index = 0;
    array_len = len;
}

/**
 * @brief Writes one 16-bit element of the sequence to the GPIO-J register. When sequence finished, it writes NO_SIGNAL.
 * 
 */
void Driver::run(){
    if(current_index >= array_len){
        current_bit_ptr = nullptr;
        GPIOJ->ODR = 1 << 5;
    }
    
    if(current_bit_ptr!=nullptr){
        GPIOJ->ODR = current_bit_ptr[current_index];
        ++current_index;
    }
    else {
        GPIOJ->ODR = 1 << 5;
    }
}