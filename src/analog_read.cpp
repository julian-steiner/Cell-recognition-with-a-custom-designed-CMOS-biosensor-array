#include <Arduino.h>

using register_size = u_int16_t;

namespace analog
{
    double read_value(const u_int8_t READ_PIN, const int res){

        pinMode(READ_PIN, INPUT);
        analogReadResolution(res);

        double max_value = std::pow(2,res)*1.0;
    
        //sensorValue ranges from 0 to 65'535 in discrete steps. Range of 0V-3.3V is divided into 2^res discrete steps. With res=16 approx 50muV per step.
        int read_value = analogRead(READ_PIN);
        double read_voltage = read_value * (3.3 / max_value);

        return read_voltage;    
    }
}