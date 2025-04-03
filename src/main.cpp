#include <Arduino.h>
#include <Serial.h>
#include "driver.h"
#include "sequence_generator.h"
#include <analog_read.h>

// Constants
// Sets the frequency to 1Hz (120Mhz / ((prescaler+1)*(period+1)))
// Settings for 1KHZ
// const int HTIM3_PRESCALER = 999;
// const int HTIM3_PERIOD = 119;
// Hardware timer for spi driver (10KHz setting)
// const int HTIM3_PRESCALER = 99;
// const int HTIM3_PERIOD = 119;
// 200KHz setting
const int HTIM3_PRESCALER = 49;
const int HTIM3_PERIOD = 11;
const int EXPOSURE_TIME_MILLIS = 20;

  
// Declarations
Driver spi_driver;
std::array<register_size, 62> seq;
std::array<register_size, 62> reset_seq;
Analog analog_input_manager;


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

double voltage = 0;

void setup()
{
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize the SPI driver
  spi_driver.initialize();
  analog_input_manager.analog_setup(16);

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

  pinMode(A0, INPUT);

  // Set a sequence to the driver to execute
  // spi_driver.set_sequence(&sequence_generator::RESET_SIGNAL[0], sequence_generator::RESET_SIGNAL_SIZE);
  // seq = sequence_generator::get_custom_spi_data_signal(127, 127, sequence_generator::COL_READ_DATA, sequence_generator::ROW_READ_DATA);
  // spi_driver.set_sequence(seq.data(), seq.size(), true);

  // Reset the sensor
  spi_driver.set_sequence(sequence_generator::RESET_SIGNAL.data(), sequence_generator::RESET_SIGNAL_SIZE, false);
  HAL_Delay(1000);

  // // Test data which pulls pin to 0.7V
  // // ROW (CTL 0-5): SVN_EN, SVP_EN, DVP_EN, EXN_EN, DVN_EN, EXP_EN
  // // COL (CTL 0-5): PEN_EN, EEN_EN, EXP_EN, SVN_EN, SVP_EN, EXN_EN
  // const u_int8_t TEST_ROW_DATA = 0b011001;
  // const u_int8_t TEST_COL_DATA = 0b101010;
  // seq = sequence_generator::get_custom_spi_data_signal(70, 0, TEST_COL_DATA, TEST_ROW_DATA);
  // spi_driver.set_sequence(seq.data(), seq.size(), false);

  seq = sequence_generator::get_custom_spi_data_signal(10, 10, sequence_generator::COL_RESET_DATA, sequence_generator::ROW_RESET_DATA);
  spi_driver.set_sequence(seq.data(), seq.size(), false);

  HAL_Delay(100);

  seq = sequence_generator::get_custom_spi_data_signal(10, 10, sequence_generator::COL_READ_DATA, sequence_generator::ROW_READ_DATA);
  spi_driver.set_sequence(seq.data(), seq.size(), false);

  voltage = analog_input_manager.read_value();

  // HAL_Delay(300);
  // spi_driver.set_sequence(reset_seq.data(), reset_seq.size(), false);
}

void loop()
{
  HAL_Delay(100);
  // Serial.println(voltage);
  Serial.println(analog_input_manager.read_value());
}