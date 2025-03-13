#pragma once
#include <bitset>

namespace bitset_utils
{
    /**
     * @brief Flip all the bits inside a bitset and return it
     *
     * @tparam bitset_size the size of the bitset
     * @param bitset the bitset to be flipped
     */
    template <size_t bitset_size>
    void flip_bitset(std::bitset<bitset_size> &bitset)
    {
        for (unsigned i = 0; i < bitset_size / 2; i++)
        {
            bool t = bitset[i];
            bitset[i] = bitset[bitset_size - 1 - i];
            bitset[bitset_size - 1 - i] = t;
        }
    }

    /**
     * @brief Constexpr function to shift a bitset to the left
     *
     * @tparam bitset_size  the size of the bitset
     * @param bitset the bitset to apply the shift
     * @param shift_size number of bits to shift
     * @return constexpr std::bitset<bitset_size>
     */
    template <size_t bitset_size>
    constexpr std::bitset<bitset_size> constexpr_shift_left_bitset(const std::bitset<bitset_size> & bitset, const unsigned & shift_size)
    {
        std::bitset<bitset_size> new_bitset;

        new_bitset[0] = 0;
        for (unsigned i = shift_size; i < bitset_size; i++)
        {
            new_bitset[i] = bitset[i-shift_size];
        }

        return new_bitset;
    }

    /**
     * @brief Extend a bitset to a larger size
     *
     * @tparam bitset_size_old
     * @tparam bitset_size_new
     * @param bitset
     * @return constexpr std::bitset<bitset_size_new>
     */
    template <size_t bitset_size_old, size_t bitset_size_new>
    std::bitset<bitset_size_new> extend_bitset(const std::bitset<bitset_size_old> &bitset)
    {
        return std::bitset<bitset_size_new>(bitset.to_ullong());
    }
}