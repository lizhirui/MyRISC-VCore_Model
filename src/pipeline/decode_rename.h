#pragma once
#include "common.h"
#include "config.h"
#include "pipeline_common.h"

namespace pipeline
{
    typedef struct decode_rename_pack_t : public if_print_t
    {
        bool enable;//this item has op
        uint32_t value;
        bool valid;//this item has valid op
        uint32_t pc;
        uint32_t imm;
        bool has_exception;
        riscv_exception_t exception_id;
        uint32_t exception_value;
        bool predicted;
        bool predicted_jump;
        uint32_t predicted_next_pc;
        bool checkpoint_id_valid;
        uint32_t checkpoint_id;
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

        virtual void print(std::string indent)
        {
            std::string blank = "    ";

            std::cout << indent << "enable = " << outbool(enable);
            std::cout << blank << "value = 0x" << fillzero(8) << outhex(value);
            std::cout << blank << "valid = " << outbool(valid);
            std::cout << blank << "pc = 0x" << fillzero(8) << outhex(pc);
            std::cout << blank << "imm = 0x" << fillzero(8) << outhex(imm);
            std::cout << blank << "has_exception = " << outbool(has_exception);
            std::cout << blank << "exception_id = " << outenum(exception_id);
            std::cout << blank << "exception_value = 0x" << fillzero(8) << outhex(exception_value) << std::endl;

            std::cout << indent << "\trs1 = " << rs1;
            std::cout << blank << "arg1_src = " << outenum(arg1_src);
            std::cout << blank << "rs1_need_map = " << outbool(rs1_need_map);

            std::cout << blank << "rs2 = " << rs2;
            std::cout << blank << "arg2_src = " << outenum(arg2_src);
            std::cout << blank << "rs2_need_map = " << outbool(rs2_need_map) << std::endl;

            std::cout << indent << "\trd = " << rd;
            std::cout << blank << "rd_enable = " << outbool(rd_enable);
            std::cout << blank << "need_rename = " << outbool(need_rename);

            std::cout << blank << "csr = 0x" << fillzero(8) << outhex(csr);
            std::cout << blank << "op = " << outenum(op);
            std::cout << blank << "op_unit = " << outenum(op_unit);
            std::cout << blank << "sub_op = ";

            switch(op_unit)
            {
                case op_unit_t::alu:
                    std::cout << outenum(sub_op.alu_op);
                    break;
                    
                case op_unit_t::bru:
                    std::cout << outenum(sub_op.bru_op);
                    break;

                case op_unit_t::csr:
                    std::cout << outenum(sub_op.csr_op);
                    break;

                case op_unit_t::div:
                    std::cout << outenum(sub_op.div_op);
                    break;

                case op_unit_t::lsu:
                    std::cout << outenum(sub_op.lsu_op);
                    break;

                case op_unit_t::mul:
                    std::cout << outenum(sub_op.mul_op);
                    break;

                default:
                    std::cout << "<Unsupported>";
                    break;
            }

            std::cout << std::endl;
        }

        virtual json get_json()
        {
            json t;
            t["enable"] = enable;
            t["value"] = value;
            t["valid"] = valid;
            t["pc"] = pc;
            t["imm"] = imm;
            t["has_exception"] = has_exception;
            t["exception_id"] = outenum(exception_id);
            t["exception_value"] = exception_value;
            t["predicted"] = predicted;
            t["predicted_jump"] = predicted_jump;
            t["predicted_next_pc"] = predicted_next_pc;
            t["checkpoint_id_valid"] = checkpoint_id_valid;
            t["checkpoint_id"] = checkpoint_id;
            t["rs1"] = rs1;
            t["arg1_src"] = arg1_src;
            t["rs1_need_map"] = rs1_need_map;
            t["rs2"] = rs2;
            t["arg2_src"] = outenum(arg2_src);
            t["rs2_need_map"] = rs2_need_map;
            t["rd"] = rd;
            t["rd_enable"] = rd_enable;
            t["need_rename"] = need_rename;
            t["csr"] = csr;
            t["op"] = outenum(op);
            t["op_unit"] = outenum(op_unit);

            switch(op_unit)
            {
                case op_unit_t::alu:
                    t["sub_op"] = outenum(sub_op.alu_op);
                    break;
                    
                case op_unit_t::bru:
                    t["sub_op"] = outenum(sub_op.bru_op);
                    break;

                case op_unit_t::csr:
                    t["sub_op"] = outenum(sub_op.csr_op);
                    break;

                case op_unit_t::div:
                    t["sub_op"] = outenum(sub_op.div_op);
                    break;

                case op_unit_t::lsu:
                    t["sub_op"] = outenum(sub_op.lsu_op);
                    break;

                case op_unit_t::mul:
                    t["sub_op"] = outenum(sub_op.mul_op);
                    break;

                default:
                    t["sub_op"] = "<Unsupported>";
                    break;
            }

            return t;
        }
    }decode_rename_pack_t;
}