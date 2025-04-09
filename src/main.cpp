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
const int EXPOSURE_TIME_MILLIS = 20;
  
// Declarations
SPI_Driver spi_driver;
std::array<register_size, 62> seq;
std::array<register_size, 62> reset_seq;
SensorDriver sensor_driver(spi_driver, A0, A1, A2, A3);


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

uint16_t pixel_buffer = 0;
uint16_t image_buffer[128*128] = {};

void setup()
{
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize the SPI driver
  spi_driver.initialize();

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

  // Test to reset the sensor and read a single pixel
  sensor_driver.reset_sensor();
  sensor_driver.read_single_pixel(0, 0, &pixel_buffer, EXPOSURE_TIME_MILLIS);

  // Test to read the whole image
  // sensor_driver.read_image(image_buffer, EXPOSURE_TIME_MILLIS);
}

void loop()
{
  HAL_Delay(400);
  Serial.println(pixel_buffer);
}