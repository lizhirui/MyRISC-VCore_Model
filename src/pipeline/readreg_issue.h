#pragma once

namespace pipeline
{
    typedef struct readreg_issue_op_info
    {
        bool enable;//this item has op
        bool valid;//this item has valid op
        uint32_t pc;
        uint32_t imm;

        uint32_t rs1;
        arg_src arg1_src;
        bool rs1_need_map;
        uint32_t rs1_phy;
        uint32_t src1_value;
        bool src1_loaded;

        uint32_t rs2;
        arg_src arg2_src;
        bool rs2_need_map;
        uint32_t rs2_phy;
        uint32_t src2_value;
        bool src2_loaded;

        uint32_t rd;
        bool rd_enable;
        bool need_rename;
        uint32_t rd_phy;

        uint32_t csr;
        op op;
        op_unit op_unit;
        
        union
        {
            alu_op alu_op;
            bru_op bru_op;
            div_op div_op;
            lsu_op lsu_op;
            mul_op mul_op;
            csr_op csr_op;
        }sub_op;
    }readreg_issue_op_info;

    typedef struct readreg_issue_pack
    {
        readreg_issue_op_info op_info[READREG_WIDTH];
    }readreg_issue_pack;
}