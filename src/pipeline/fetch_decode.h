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
        bool has_exception;
        riscv_exception_t exception_id;
        uint32_t exception_value;
    }op_info;

    typedef struct fetch_decode_pack_t : public if_print_t
    {
        fetch_decode_op_info_t op_info[FETCH_WIDTH];

        virtual void print(std::string indent)
        {
            std::string blank = "    ";

            for(auto i = 0;i < FETCH_WIDTH;i++)
            {
                std::cout << indent << "Item " << i << ":" << std::endl;
                std::cout << indent << "\tpc = 0x" << fillzero(8) << outhex(op_info[i].pc);
                std::cout << blank << "value = 0x" << fillzero(8) << outhex(op_info[i].value);
                std::cout << blank << "enable = " << outbool(op_info[i].enable);
                std::cout << blank << "has_exception = " << outbool(op_info[i].has_exception);
                std::cout << blank << "exception_id = " << outenum(op_info[i].exception_id);
                std::cout << blank << "exception_value = 0x" << fillzero(8) << outhex(op_info[i].exception_value) << std::endl;
            }
        }

        virtual json get_json()
        {
            json ret = json::array();

            for(auto i = 0;i < FETCH_WIDTH;i++)
            {
                json t;
                t["pc"] = op_info[i].pc;
                t["value"] = op_info[i].value;
                t["enable"] = op_info[i].enable;
                t["has_exception"] = op_info[i].has_exception;
                t["exception_id"] = outenum(op_info[i].exception_id);
                t["exception_value"] = op_info[i].exception_value;
                ret.push_back(t);
            }
            
            return ret;
        }
    }fetch_decode_pack_t;
}