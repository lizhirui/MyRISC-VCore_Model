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

    typedef struct fetch_decode_pack_t : public if_print_t
    {
        fetch_decode_op_info_t op_info[FETCH_WIDTH];

        virtual void print(std::string indent)
        {
            for(auto i = 0;i < FETCH_WIDTH;i++)
            {
                std::cout << indent << "Item " << i << ":" << std::endl;
                std::cout << indent << "\tpc = 0x" << fillzero(8) << outhex(op_info[i].pc);
                std::cout << "    value = 0x" << fillzero(8) << outhex(op_info[i].value);
                std::cout << "    enable = " << outbool(op_info[i].enable) << std::endl;
            }
        }
    }fetch_decode_pack_t;
}