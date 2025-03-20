#include <Arduino.h>
#include <Serial.h>
#include "driver.h"
#include "sequence_generator.h"

// Constants
// Sets the frequency to 1Hz (120Mhz / ((prescaler+1)*(period+1)))
// Settings for 1KHZ
// htim3.Init.Prescaler = 999;
// htim3.Init.Period = 119;
// Hardware timer for spi driver (10KHz setting)
const int HTIM3_PRESCALER = 99;
const int HTIM3_PERIOD = 119;
// 100KHz setting
// const int HTIM3_PRESCALER = 99;
// const int HTIM3_PERIOD = 11;

  
// Declarations
Driver spi_driver;
std::array<register_size, 62> seq;


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

  // Set a sequence to the driver to execute
  // spi_driver.set_sequence(&sequence_generator::RESET_SIGNAL[0], sequence_generator::RESET_SIGNAL_SIZE);
  seq = sequence_generator::get_custom_spi_data_signal(13, 127, sequence_generator::COL_READ_DATA, sequence_generator::ROW_READ_DATA);
  spi_driver.set_sequence(seq.data(), seq.size(), true);

  analog_instance.analog_setup(16);
}

void loop()
{
}