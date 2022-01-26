#pragma once
#include "common.h"
#include "config.h"

namespace pipeline
{
    typedef struct fetch_decode_op_info
    {
        uint32_t pc;
        uint32_t value;
        bool enable;
    }op_info;

    typedef struct fetch_decode_pack
    {
        fetch_decode_op_info op_info[FETCH_WIDTH];
    }fetch_decode_pack;
}