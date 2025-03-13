#include <Arduino.h>

//BEWARE: register_size in different files is different. TODO: Have one unified register_size type.

using register_size = u_int16_t;

class Driver
{
private:
    register_size *array_pointer;
    register_size *current_pointer;
    int current_index;
    int array_len;
public:
    Driver();
    ~Driver();

    void initialize(register_size *bit_sequence, int len);
    void run();
    void update_bits();

    void run_through_array(register_size *bit_sequence, int len);
};

Driver::Driver() : array_pointer(nullptr), current_pointer(nullptr), current_index(0), array_len(0) {}

Driver::~Driver()
{
}

/**
 * @brief Sets up the digital output pins on the Arduino Giga and sets current index to 0
 * 
 * @param bit_sequence sequence-array of bits
 * @param len INT-LENGTH(!) of 16-bit-sequence-array
 */
void Driver::initialize(register_size *bit_sequence, int len){

    pinMode(25, OUTPUT);
    pinMode(27, OUTPUT);
    pinMode(29, OUTPUT);
    pinMode(31, OUTPUT);
    pinMode(33, OUTPUT);
    pinMode(35, OUTPUT);
    pinMode(37, OUTPUT);
    pinMode(38, OUTPUT);

    array_pointer = bit_sequence;
    current_pointer = array_pointer;
    current_index = 0;
    
    array_len = len;
}

/**
 * @brief Activates the different pins according to the bit_sequence
 * 
 */
void Driver::run(){
    if(current_pointer!=nullptr)
        GPIOJ->ODR = *current_pointer;
}

/**
 * @brief Updates the pointer inside the bit_array
 * the if-condition is here for it to be possible to use this function seperately from run_through_array
 */
void Driver::update_bits(){

    if(current_index + 1 < array_len){
        ++ current_index;
        current_pointer = &array_pointer[current_index];
    }

    else return;
}

void Driver::run_through_array(register_size *bit_sequence, int len){
    
    initialize(bit_sequence, len);

    while(current_index + 1 < array_len){
        run();
        update_bits();
    }
}