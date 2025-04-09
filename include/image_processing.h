#pragma once
#include <Arduino.h>
using register_size = u_int16_t;

class ProcessImage
{
private:
    int width = 128;
    int height = 128;
    const register_size *ptr_pxl_zero_zero;

    char filename[20];
    int counter;

public:
    ProcessImage();
    ~ProcessImage();

    void initialize(const register_size *array_ptr, const int h, const int w);
    void setupSD();
    void writeImage();

    bool fileExists(const char* filename);
};