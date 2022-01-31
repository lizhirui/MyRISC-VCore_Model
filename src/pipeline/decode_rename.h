#pragma once
#include "common.h"
#include "config.h"
#include "pipeline_common.h"

namespace pipeline
{
    typedef struct decode_rename_op_info_t
    {
        bool enable;//this item has op
        uint32_t value;
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

    typedef struct decode_rename_pack_t : public if_print_t
    {
        decode_rename_op_info_t op_info[DECODE_WIDTH];

        virtual void print(std::string indent)
        {
            std::string blank = "    ";

            for(auto i = 0;i < DECODE_WIDTH;i++)
            {
                std::cout << indent << "Item " << i << ":" << std::endl;

                std::cout << indent << "\tenable = " << outbool(op_info[i].enable);
                std::cout << blank << "value = 0x" << fillzero(8) << outhex(op_info[i].value);
                std::cout << blank << "valid = " << outbool(op_info[i].valid);
                std::cout << blank << "pc = 0x" << fillzero(8) << outhex(op_info[i].pc);
                std::cout << blank << "imm = 0x" << fillzero(8) << outhex(op_info[i].imm);

                std::cout << blank << "rs1 = " << op_info[i].rs1;
                std::cout << blank << "arg1_src = " << outenum(op_info[i].arg1_src);
                std::cout << blank << "rs1_need_map = " << outbool(op_info[i].rs1_need_map);

                std::cout << blank << "rs2 = " << op_info[i].rs2;
                std::cout << blank << "arg2_src = " << outenum(op_info[i].arg2_src);
                std::cout << blank << "rs2_need_map = " << outbool(op_info[i].rs2_need_map) << std::endl;

                std::cout << indent << "\trd = " << op_info[i].rd;
                std::cout << blank << "rd_enable = " << outbool(op_info[i].rd_enable);
                std::cout << blank << "need_rename = " << outbool(op_info[i].need_rename);

                std::cout << blank << "csr = 0x" << fillzero(8) << outhex(op_info[i].csr);
                std::cout << blank << "op = " << outenum(op_info[i].op);
                std::cout << blank << "op_unit = " << outenum(op_info[i].op_unit);
                std::cout << blank << "sub_op = ";

                switch(op_info[i].op_unit)
                {
                    case op_unit_t::alu:
                        std::cout << outenum(op_info[i].sub_op.alu_op);
                        break;
                    
                    case op_unit_t::bru:
                        std::cout << outenum(op_info[i].sub_op.bru_op);
                        break;

                    case op_unit_t::csr:
                        std::cout << outenum(op_info[i].sub_op.csr_op);
                        break;

                    case op_unit_t::div:
                        std::cout << outenum(op_info[i].sub_op.div_op);
                        break;

                    case op_unit_t::lsu:
                        std::cout << outenum(op_info[i].sub_op.lsu_op);
                        break;

                    case op_unit_t::mul:
                        std::cout << outenum(op_info[i].sub_op.mul_op);
                        break;

                    default:
                        std::cout << "<Unsupported>";
                        break;
                }

                std::cout << std::endl;
            }
        }
    }decode_rename_pack_t;
}