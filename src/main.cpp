#include <Arduino.h>
#include <Serial.h>
#include "driver.h"
#include "sequence_generator.h"
#include "analog_read.cpp"

Driver spi_driver;

std::array<register_size, 62> seq;

//double max_value = std::pow(2,16)*1.0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  spi_driver.initialize();
  //spi_driver.set_sequence(&sequence_generator::RESET_SIGNAL[0], sequence_generator::RESET_SIGNAL_SIZE);
  seq = sequence_generator::get_custom_spi_data_signal(12, 12, sequence_generator::COL_RESET_DATA, sequence_generator::ROW_RESET_DATA);
  spi_driver.set_sequence(seq.data(), seq.size(), true);

  analog::analog_setup(16);
}

void loop() {

  //const register_size* ptr = seq.data();
  //for (unsigned i = 0; i < seq.size(); i++) {
  //  Serial.println(ptr[i]);
  //}
  spi_driver.run();
  delay(10);

  if(spi_driver.has_sequence()){
    double read_voltage = analog::read_value(16);
    Serial.println(analog::read_value(16));
  }
}