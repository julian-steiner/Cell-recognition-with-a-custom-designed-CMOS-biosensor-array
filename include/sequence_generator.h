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
    const u_int8_t ROW_RESET_DATA = 0b000010;
    const u_int8_t ROW_READ_DATA = 0b100010;
    const u_int8_t COL_RESET_DATA = 0b010000;
    const u_int8_t COL_READ_DATA = 0b010100;

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
     * @param sequence sequence to insert the data
     * @param start_index index where the data insertion starts
     * @param data_row data for the column pin
     * @param data_col data for the row pin
     */
    template <typename T, size_t sequence_size, size_t data_size>
    void write_data_to_custom_spi_sequence(std::array<T, sequence_size> &sequence, const unsigned &start_index, const std::bitset<data_size> &data_row, const std::bitset<data_size> &data_col)
    {
        if (sequence_size - start_index < 2 * data_size)
        {
            throw std::out_of_range("Sequence buffer too small");
        }

        // Write data to custom SPI sequence
        for (unsigned i = 0; i < data_size; i++)
        {
            sequence.at(start_index + 2 * i) = data_row[i] * DATA_ROW_PIN | data_col[i] * DATA_COL_PIN | CLK_PIN;
            sequence.at(start_index + 2 * i + 1) = data_row[i] * DATA_ROW_PIN | data_col[i] * DATA_COL_PIN;
        }
    }

    /**
     * @brief Writes a sequence into another SPI sequence
     *
     * @tparam T the type of the spi sequence elements
     * @tparam old_sequence_size The size of the sequence where the new sequence is inserted
     * @tparam new_sequence_size The size of the new sequence
     * @param old_sequence
     * @param start_index the index at which the insertion starts
     * @param new_sequence
     */
    template <typename T, size_t old_sequence_size, size_t new_sequence_size>
    void write_sequence_to_custom_spi_sequence(std::array<T, old_sequence_size> &old_sequence, const unsigned &start_index, const std::array<T, new_sequence_size> &new_sequence)
    {
        if (old_sequence_size - start_index < new_sequence_size)
        {
            throw std::out_of_range("Sequence buffer too small");
        }

        // Write data to custom SPI sequence
        for (unsigned i = 0; i < new_sequence_size; i++)
        {
            old_sequence.at(start_index + i) = new_sequence.at(i);
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
    std::array<register_size, 62> get_custom_spi_data_signal(const u_int8_t col_addr, const u_int8_t row_addr, const u_int8_t col_data, const u_int8_t row_data);

}