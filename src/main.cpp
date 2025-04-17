#include <Arduino.h>
#include <Serial.h>
#include "driver.h"
#include "sequence_generator.h"
#include <analog_read.h>
#include "sensor_driver.h"

// Constants
// Sets the frequency to 1Hz (120Mhz / ((prescaler+1)*(period+1)))
// Settings for 1KHZ
// const int HTIM3_PRESCALER = 999;
// const int HTIM3_PERIOD = 119;
// Hardware timer for spi driver (10KHz setting)
// const int HTIM3_PRESCALER = 99;
// const int HTIM3_PERIOD = 119;
// 200KHz setting
const int HTIM3_PRESCALER = 29;
const int HTIM3_PERIOD = 5;
const int EXPOSURE_TIME_MILLIS = 5;
  
// Declarations
SPI_Driver spi_driver;
std::array<register_size, 62> seq;
std::array<register_size, 62> reset_seq;
const int image_size = 128;
SensorDriver sensor_driver(spi_driver, A0, A1, A2, A3, true, false);


// Set up the interrupt handler and hardware timer
static TIM_HandleTypeDef htim3;

// Defines the interrupt handler for TIM3
extern "C" void TIM3_IRQHandler()
{
  HAL_TIM_IRQHandler(&htim3);
}

// Defines the callback handler that runs the driver
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM3)
  {
    spi_driver.run();
  }
}

int pixel_buffer = 0;
int reset_level = 0;
int image_buffer[image_size*image_size] = {};

void setup()
{
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize the SPI driver
  spi_driver.initialize();

  // Turn on the LED
  pinMode(86, OUTPUT);
  digitalWrite(86, LOW);

  // Initialize the hardware timer 3
  // Enable the interrupt handler for the timer
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
  HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);

  // Configure the timer
  __HAL_RCC_TIM3_CLK_ENABLE();
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  htim3.Init.RepetitionCounter = 0;
  htim3.Instance = TIM3;

  htim3.Init.Prescaler = HTIM3_PRESCALER;
  htim3.Init.Period = HTIM3_PERIOD;

  HAL_TIM_Base_Init(&htim3);

  // Start the timer and interrupt handler
  HAL_TIM_Base_Start_IT(&htim3);

  // Initialize the sensor driver
  sensor_driver.initialize_sensor();
  sensor_driver.calibrate_readout(0, 0);

  reset_level = sensor_driver.calibration_level;

  HAL_Delay(100);

  // Read a full image
  sensor_driver.reset_sensor();
  // sensor_driver.read_image(image_buffer, EXPOSURE_TIME_MILLIS);

  // Read a single pixel
  // sensor_driver.reset_single_pixel(120, 120);
  // HAL_Delay(100);
  // sensor_driver.read_single_pixel(120, 120, &pixel_buffer, EXPOSURE_TIME_MILLIS);

  // Ensure that the driver is finished before proceeding
  while (spi_driver.has_sequence()) {}
}

void loop()
{
  digitalWrite(86, HIGH);
  Serial.print("BEGIN_DATA\n");
  Serial.print("RESET_LEVEL," + String(reset_level) + "\n");
  for (unsigned y = 0; y < image_size; y++) {
    for (unsigned x = 0; x < image_size; x++) {
      Serial.print(String(*(image_buffer+image_size*y+x)) + ",");
    }
    Serial.print("\n");
  }
  Serial.print("END_DATA\n");
  HAL_Delay(1000);
  // Serial.println(pixel_buffer);
}