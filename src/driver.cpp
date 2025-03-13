#include <Arduino.h>

//BEWARE: register_size in different files is different. TODO: Have one unified register_size type.

using register_size = u_int16_t;

/**
 * @class Driver
 * @brief Controls the GPIO pins of register J on the Arduino Giga using a 16-bit bit-sequence.
 * 
 * This class allows for the initialization of GPIO pins. It updates them based on a sequence of 
 * 16-bit values and controls the timing of updates (optional).
 * 
 * **IMPORTANT**: You must call `initialize()` before calling any other method of the `Driver` class.
 */
class Driver
{
private:
    register_size *bit_sequence_ptr;
    register_size *current_bit_ptr;
    int current_index;
    int array_len;
public:
    Driver();
    ~Driver();

    void initialize(register_size *bit_sequence, int len);
    void run();
    void update_bits();

    //optional function, can also be defined outside of Driver-class
    void run_through_array(int delay_time);
};

Driver::Driver() : bit_sequence_ptr(nullptr), current_bit_ptr(nullptr), current_index(0), array_len(0) {}

Driver::~Driver()
{
}

/**
 * @brief Initializes the GPIO pins and the 16-bit bit-sequence.
 * 
 * @param sequence Pointer to the sequence of 16-bit values.
 * @param len The number of 16-bit elements inside the bit sequence.
 */
void Driver::initialize(register_size *sequence, int len){

    pinMode(25, OUTPUT);
    pinMode(27, OUTPUT);
    pinMode(29, OUTPUT);
    pinMode(31, OUTPUT);
    pinMode(33, OUTPUT);
    pinMode(35, OUTPUT);
    pinMode(37, OUTPUT);
    pinMode(38, OUTPUT);

    bit_sequence_ptr = sequence;
    current_bit_ptr = bit_sequence_ptr;
    current_index = 0;
    
    array_len = len;
}

/**
 * @brief Toggles the particular GPIO pins inside the J register according to the 16-bit bit-sequence.
 * 
 */
void Driver::run(){
    if(current_bit_ptr!=nullptr){
        GPIOJ->ODR = *current_bit_ptr;
    }
    else {
        Serial.println("Error: Driver not initialized. Please call initialize() first.");
    }
}

/**
 * @brief Updates the pointer to the current most-top bit.
 *
 */
void Driver::update_bits(){

    //the if-condition is here for it to be possible to use this function seperately from run_through_array
    if(current_index + 1 < array_len){
        ++ current_index;
        current_bit_ptr = &bit_sequence_ptr[current_index];
    }
}

/**
 * @brief Runs through a sequence of 16-bit values and sets the GPIO registers within a given delay time.
 * 
 * **IMPORTANT** You must call initialize() at least once before calling this function.
 * @param delay_time
 */
void Driver::run_through_array(int delay_time){

    while(current_index < array_len){
        run();
        update_bits();
        delay(delay_time);
    }

    Serial.println("End of sequence reached.");
}