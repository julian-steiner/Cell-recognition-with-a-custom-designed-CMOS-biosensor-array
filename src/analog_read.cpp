#include <Arduino.h>
#include "analog_read.h"

using register_size = u_int16_t;

Analog::Analog() : resolution(16), max_value(65535.0) {}

Analog::~Analog()
{
}

void Analog::analog_setup(int res){
    resolution = res;
    pinMode(A0, INPUT);
    analogReadResolution(resolution);
    max_value = std::pow(2,resolution)*1.0;
}

double Analog::read_value(){
    //Sensor Value ranges from 0 to 65'535 in discrete steps. Range of 0V-3.3V is divided into 2^res discrete steps. With res=16 approx 50muV per step.
    int read_value = analogRead(A0);
    double read_voltage = read_value * (3.3 / max_value);
    return read_voltage;
}