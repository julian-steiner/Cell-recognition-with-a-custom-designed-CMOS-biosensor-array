#include "bitset_utils.h"
#include "sequence_generator.h"

/**
 * @brief Create a signal for sending the selected data to the addresses with the custom SPI protocol
 * 
 * @param col_addr The address of the column (0-127)
 * @param row_addr The address of the row (0-127)
 * @param col_data The data to send to the column (read, reset)
 * @param row_data The data to send to the row (read, reset)
 * @return const std::array<register_size, 62> 
 */
void sequence_generator::get_custom_spi_data_signal(register_size* buffer, u_int8_t col_addr, const u_int8_t row_addr, const u_int8_t col_data, const u_int8_t row_data)
{
    const uint length = SPI_SEQUENCE_SIZE;
    const uint data_size = 6 + 6 + 3 + 7 + 2;
    const uint data_start = 6;
    const uint write_data_start = 6 + 6;
    const uint addr_start = 6 + 6 + 3;

    // Generate the bitset to write
    std::bitset<data_size> col_data_bits;
    std::bitset<data_size> row_data_bits;

    // Generate address bitmasks
    std::bitset<data_size> col_address_bitmask = std::bitset<data_size>(col_addr) << addr_start;
    std::bitset<data_size> row_address_bitmask = std::bitset<data_size>(row_addr) << addr_start;

    // Generate data bitmask
    std::bitset<data_size> row_data_bitmask = std::bitset<data_size>(row_data) << data_start;
    std::bitset<data_size> col_data_bitmask = std::bitset<data_size>(col_data) << data_start;

    // Generate write data signal bitmask
    // Write data is 101
    std::bitset<data_size> write_data_bitmask = std::bitset<data_size>(5) << write_data_start;

    // Generate the bit sets
    row_data_bits |= row_address_bitmask | row_data_bitmask | write_data_bitmask;
    col_data_bits |= col_address_bitmask | col_data_bitmask | write_data_bitmask;

    // Write the data to the sequence
    sequence_generator::write_data_to_custom_spi_sequence<register_size, data_size>(buffer, row_data_bits, col_data_bits);

    // TODO: determine which position to insert
    sequence_generator::write_sequence_to_custom_spi_sequence<register_size>(buffer + data_size*2, sequence_generator::LATCH_SIGNAL.data(), sequence_generator::LATCH_SIGNAL.size());

    for (unsigned i = 0; i < length; i++){
        *(buffer+i) |= DEFAULT_SIGNAL;
    }
}