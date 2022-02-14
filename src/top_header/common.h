#pragma once

//import header files
#include <cstddef>
#include <cstdint>
/*#ifdef NDEBUG
    #undef NDEBUG
    #include <cassert>
    #define NDEBUG 1
#else
    #include <cassert>
#endif*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <queue>
#include <string>
#include <iomanip>
#include <type_traits>
#include <unordered_map>
#include <memory>
#include <utility>
#include <algorithm>
#include <map>
#include <vector>
#include <cctype>
#include "magic_enum.h"

#undef assert
#define assert(cond)\
    if(!(cond)) \
    { \
        printf("In file %s, Line %d, %s\n", __FILE__, __LINE__, #cond);\
        abort();\
    }

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

#define fillzero(length) std::setw(length) << std::setfill('0')
#define outhex(x) std::setiosflags(std::ios::uppercase) << std::hex << (x) << std::dec
#define outbool(x) std::boolalpha << (x)
#define outenum(x) magic_enum::enum_name(x)

enum class riscv_exception_t
{
    instruction_address_misaligned = 0,
    instruction_access_fault,
    illegal_instruction,
    breakpoint,
    load_address_misaligned,
    load_access_fault,
    store_amo_address_misaligned,
    store_amo_access_fault,
    environment_call_from_u_mode,
    environment_call_from_s_mode,
    reserved_10,
    environment_call_from_m_mode,
    instruction_page_fault,
    load_page_fault,
    reserved_14,
    store_amo_page_fault,
};

inline bool is_align(uint32_t x, uint32_t access_size)
{
    return !(x & (access_size - 1));
}

inline uint32_t sign_extend(uint32_t imm, uint32_t imm_length)
{
    assert((imm_length > 0) && (imm_length < 32));
    auto sign_bit = (imm >> (imm_length - 1));
    auto extended_imm = ((sign_bit == 0) ? 0 : (((sign_bit << (32 - imm_length)) - 1) << imm_length)) | imm;
    return extended_imm;
}

typedef struct if_print_t
{
    virtual void print(std::string indent)
    {
        std::cout << "<Not Implemented Method>" << std::endl;
    }
}if_print_t;

template<typename T> class if_print_fake : public if_print_t
{
    private:
        T value;

    public:
        if_print_fake()
        {
        }

        if_print_fake(const T& value) : value(value)
        {
        }

        bool operator==(const T& x)
        {
           return this->value == x; 
        }
};