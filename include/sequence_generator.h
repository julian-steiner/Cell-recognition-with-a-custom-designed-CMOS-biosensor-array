#pragma once

#include <array>
#include <bitset>

using register_size = u_int16_t;

namespace sequence_generator
{
    // Pin assignments
    const register_size RESET_PIN = 1 << 1;
    const register_size LATCH_PIN = 1 << 2;
    const register_size DATA_ROW_PIN = 1 << 3;
    const register_size DATA_COL_PIN = 1 << 4;
    const register_size CLK_PIN = 1 << 5;
    const register_size POWER_PIN = 1 << 6;

    // Default actions and states
    // States for custom SPI
    const register_size LATCH_STATE = CLK_PIN | LATCH_PIN;
    const register_size NO_SIGNAL = CLK_PIN | RESET_PIN;

    // Default DIO states
    const register_size RESET_STATE = CLK_PIN;
    const register_size DEFAULT_SIGNAL = RESET_PIN;

    // Default sizes
    const size_t LATCH_SIGNAL_SIZE = 14;
    const size_t RESET_SIGNAL_SIZE = 32;

    // Data signals (passed to chip from right to left)
    // ROW (CTL 0-5): SVN_EN, SVP_EN, DVP_EN, EXN_EN, DVN_EN, EXP_EN
    // Data for EXP
    const u_int8_t ROW_RESET_DATA_EXP = 0b001001;
    const u_int8_t ROW_READ_DATA_EXP = 0b011000;
    const u_int8_t ROW_CALIBRATE_DATA_EXP = 0b011001;
    // Data for EXN
    const u_int8_t ROW_RESET_DATA_EXN = 0b000110;
    const u_int8_t ROW_READ_DATA_EXN = 0b100010;
    const u_int8_t ROW_CALIBRATE_DATA_EXN = 0b100110;

    // COL (CTL 0-5): PEN_EN, EEN_EN, EXP_EN, SVN_EN, SVP_EN, EXN_EN
    // Data for EXP
    const u_int8_t COL_RESET_DATA_EXP = 0b101000;
    const u_int8_t COL_READ_DATA_EXP = 0b100010;
    const u_int8_t COL_CALIBRATE_DATA_EXP = 0b101010;
    // Data for EXN
    const u_int8_t COL_RESET_DATA_EXN = 0b100001;
    const u_int8_t COL_READ_DATA_EXN = 0b100100;
    const u_int8_t COL_CALIBRATE_DATA_EXN = 0b100101;

    // Precoded signals
    // SPI Signals
    const std::array<register_size, LATCH_SIGNAL_SIZE> LATCH_SIGNAL = {NO_SIGNAL, NO_SIGNAL, LATCH_STATE, LATCH_STATE, NO_SIGNAL, NO_SIGNAL, LATCH_STATE, LATCH_STATE, NO_SIGNAL, NO_SIGNAL, LATCH_STATE, LATCH_STATE, NO_SIGNAL, NO_SIGNAL};
    const std::array<register_size, RESET_SIGNAL_SIZE> RESET_SIGNAL = {NO_SIGNAL, NO_SIGNAL, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, NO_SIGNAL, NO_SIGNAL, NO_SIGNAL, NO_SIGNAL, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, RESET_STATE, NO_SIGNAL, NO_SIGNAL};

    /**
     * @brief Writes data to an spi sequence using the constant data pins specified. Needs 2*data_size space after the start_index
     *
     * @tparam T the type of the entries to the SPI sequence
     * @tparam sequence_size size of the spi sequence
     * @tparam data_size size of the data to be inserted
     * @param sequence pointer in the sequence where to insert the data
     * @param data_row data for the column pin
     * @param data_col data for the row pin
     */
    template <typename T, size_t data_size>
    void write_data_to_custom_spi_sequence(T* sequence, const std::bitset<data_size> &data_row, const std::bitset<data_size> &data_col)
    {
        // Write data to custom SPI sequence
        for (unsigned i = 0; i < data_size; i++)
        {
            *(sequence++) = data_row[i] * DATA_ROW_PIN | data_col[i] * DATA_COL_PIN | CLK_PIN;
            *(sequence++) = data_row[i] * DATA_ROW_PIN | data_col[i] * DATA_COL_PIN;
        }
    }

    /**
     * @brief Writes a sequence into another SPI sequence
     *
     * @tparam T the type of the spi sequence elements
     * @param old_sequence
     * @param new_sequence
     */
    template <typename T>
    void write_sequence_to_custom_spi_sequence(T* old_sequence, const T* new_sequence, size_t new_sequence_size)
    {
        // Write data to custom SPI sequence
        for (unsigned i = 0; i < new_sequence_size; i++)
        {
            *(old_sequence++) = *(new_sequence++);
        }
    }

    /**
     * @brief Create a signal for sending the selected data to the addresses with the custom SPI protocol
     *
     * @param col_addr The address of the column (0-127)
     * @param row_addr The address of the row (0-127)
     * @param col_data The data to send to the column (read, reset)
     * @param row_data The data to send to the row (read, reset)
     * @return const std::array<register_size, 62>
     */
    void get_custom_spi_data_signal(register_size* buffer, u_int8_t col_addr, const u_int8_t row_addr, const u_int8_t col_data, const u_int8_t row_data);

}