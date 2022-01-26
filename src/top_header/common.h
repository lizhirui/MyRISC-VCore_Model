#pragma once

//import header files
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <cstring>
#include <queue>

//machine types
using size_t = std::size_t;

//unsigned integer types
using uint8_t = std::uint8_t;
using uint16_t = std::uint16_t;
using uint32_t = std::uint32_t;
using uint64_t = std::uint64_t;

//signed integer types
using int8_t = std::int8_t;
using int16_t = std::int16_t;
using int32_t = std::int32_t;
using int64_t = std::int64_t;

#define bitsizeof(x) (sizeof(x) * 8)

inline bool is_align(uint32_t x, uint32_t access_size)
{
    return !(x & (access_size - 1));
}

inline uint32_t sign_extend(uint32_t imm, uint32_t imm_length)
{
    assert((imm_length > 0) && (imm_length < 32));
    auto sign_bit = (imm >> (imm_length - 1));
    auto extended_imm = (((sign_bit << (32 - imm_length)) - 1) << imm_length) | imm;
    return extended_imm;
}