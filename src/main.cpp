#include <Arduino.h>
#include <Serial.h>
#include "driver.h"
#include "sequence_generator.h"

Driver spi_driver;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("initialized serial");

  spi_driver.initialize();
  //spi_driver.set_sequence(&sequence_generator::RESET_SIGNAL[0], sequence_generator::RESET_SIGNAL_SIZE);
  spi_driver.set_sequence(&sequence_generator::get_custom_spi_data_signal(0, 0, sequence_generator::COL_RESET_DATA, sequence_generator::ROW_RESET_DATA)[0], 62);
}

void loop() {

  spi_driver.run();
  delay(200);

  Serial.println(spi_driver.has_sequence());
}