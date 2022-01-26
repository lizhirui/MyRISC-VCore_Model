#pragma once
#include "common.h"
#include "config.h"
#include "pipeline_common.h"

namespace pipeline
{
    typedef struct decode_rename_op_info_t
    {
        bool enable;//this item has op
        bool valid;//this item has valid op
        uint32_t pc;
        uint32_t imm;
        uint32_t rs1;
        arg_src_t arg1_src;
        bool rs1_need_map;
        uint32_t rs2;
        arg_src_t arg2_src;
        bool rs2_need_map;
        uint32_t rd;
        bool rd_enable;
        bool need_rename;
        uint32_t csr;
        op_t op;
        op_unit_t op_unit;
        
        union
        {
            alu_op_t alu_op;
            bru_op_t bru_op;
            div_op_t div_op;
            lsu_op_t lsu_op;
            mul_op_t mul_op;
            csr_op_t csr_op;
        }sub_op;
    }decode_rename_op_info_t;

    typedef struct decode_rename_pack_t
    {
        decode_rename_op_info_t op_info[DECODE_WIDTH];
    }decode_rename_pack_t;
}