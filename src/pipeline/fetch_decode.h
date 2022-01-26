#pragma once
#include "common.h"
#include "config.h"

namespace pipeline
{
    typedef struct fetch_decode_op_info_t
    {
        uint32_t pc;
        uint32_t value;
        bool enable;
    }op_info;

    typedef struct fetch_decode_pack_t
    {
        fetch_decode_op_info_t op_info[FETCH_WIDTH];
    }fetch_decode_pack_t;
}