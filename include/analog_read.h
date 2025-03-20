#include <Arduino.h>

using register_size = u_int16_t;

class Analog
{
private:
    double max_value;
    int resolution;

public:
    Analog();
    ~Analog();

    //currently only sets up pin A0.
    void analog_setup(int res);
    double read_value();
};