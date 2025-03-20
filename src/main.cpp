#include <Arduino.h>
#include <Serial.h>
#include "driver.h"
#include "sequence_generator.h"

Driver spi_driver;

std::array<register_size, 62> seq;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("initialized serial");

  spi_driver.initialize();
  //spi_driver.set_sequence(&sequence_generator::RESET_SIGNAL[0], sequence_generator::RESET_SIGNAL_SIZE);
  seq = sequence_generator::get_custom_spi_data_signal(12, 12, sequence_generator::COL_RESET_DATA, sequence_generator::ROW_RESET_DATA);
  spi_driver.set_sequence(seq.data(), seq.size(), true);
}

void loop() {

  //const register_size* ptr = seq.data();
  //for (unsigned i = 0; i < seq.size(); i++) {
  //  Serial.println(ptr[i]);
  //}
  spi_driver.run();
  delay(10);

  //Serial.println(spi_driver.has_sequence());
}