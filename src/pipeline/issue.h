#pragma once
#include "common.h"
#include "config.h"
#include "../component/port.h"
#include "../component/fifo.h"
#include "../component/issue_queue.h"
#include "readreg_issue.h"
#include "issue_execute.h"
#include "wb.h"
#include "commit.h"

namespace pipeline
{
    typedef struct issue_feedback_pack_t
    {
        bool stall;
    }issue_feedback_pack_t;
    
    class issue : public if_print_t
    {
        private:
            typedef struct issue_queue_item_t : public if_print_t
            {
                bool enable;//this item has op
                uint32_t value;
                bool valid;//this item has valid op
                uint32_t rob_id;
                uint32_t pc;
                uint32_t imm;
                bool has_exception;
                riscv_exception_t exception_id;
                uint32_t exception_value;

                uint32_t rs1;
                arg_src_t arg1_src;
                bool rs1_need_map;
                uint32_t rs1_phy;
                uint32_t src1_value;
                bool src1_loaded;

                uint32_t rs2;
                arg_src_t arg2_src;
                bool rs2_need_map;
                uint32_t rs2_phy;
                uint32_t src2_value;
                bool src2_loaded;

                uint32_t rd;
                bool rd_enable;
                bool need_rename;
                uint32_t rd_phy;

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
                    std::string blank = "  ";

                    std::cout << indent << "\tenable = " << outbool(enable);
                    std::cout << blank << "value = 0x" << fillzero(8) << outhex(value);
                    std::cout << blank << "valid = " << outbool(valid);
                    std::cout << blank << "rob_id = " << rob_id;
                    std::cout << blank << "pc = 0x" << fillzero(8) << outhex(pc);
                    std::cout << blank << "imm = 0x" << fillzero(8) << outhex(imm);
                    std::cout << blank << "has_exception = " << outbool(has_exception);
                    std::cout << blank << "exception_id = " << outenum(exception_id);
                    std::cout << blank << "exception_value = 0x" << fillzero(8) << outhex(exception_value) << std::endl;

                    std::cout << indent << "\trs1 = " << rs1;
                    std::cout << blank << "arg1_src = " << outenum(arg1_src);
                    std::cout << blank << "rs1_need_map = " << outbool(rs1_need_map);
                    std::cout << blank << "rs1_phy = " << rs1_phy;
                    std::cout << blank << "src1_value = 0x" << fillzero(8) << outhex(src1_value);
                    std::cout << blank << "src1_loaded = " << outbool(src1_loaded) << std::endl;

                    std::cout << indent << "\trs2 = " << rs2;
                    std::cout << blank << "arg2_src = " << outenum(arg2_src);
                    std::cout << blank << "rs2_need_map = " << outbool(rs2_need_map);
                    std::cout << blank << "rs2_phy = " << rs2_phy;
                    std::cout << blank << "src2_value = 0x" << fillzero(8) << outhex(src2_value);
                    std::cout << blank << "src2_loaded = " << outbool(src2_loaded);

                    std::cout << blank << "rd = " << rd;
                    std::cout << blank << "rd_enable = " << outbool(rd_enable);
                    std::cout << blank << "need_rename = " << outbool(need_rename);
                    std::cout << blank << "rd_phy = " << rd_phy << std::endl;

                    std::cout << indent << "\tcsr = 0x" << fillzero(8) << outhex(csr);
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
            }issue_queue_item_t;
            
            component::port<readreg_issue_pack_t> *readreg_issue_port;
            component::fifo<issue_execute_pack_t> **issue_alu_fifo;
            component::fifo<issue_execute_pack_t> **issue_bru_fifo;
            component::fifo<issue_execute_pack_t> **issue_csr_fifo;
            component::fifo<issue_execute_pack_t> **issue_div_fifo;
            component::fifo<issue_execute_pack_t> **issue_lsu_fifo;
            component::fifo<issue_execute_pack_t> **issue_mul_fifo;
            
            component::issue_queue<issue_queue_item_t> issue_q;
            bool busy = false;
            bool is_inst_waiting = false;
            uint32_t inst_waiting_rob_id = 0;

            uint32_t last_index = 0;
            uint32_t alu_index = 0;
            uint32_t bru_index = 0;
            uint32_t csr_index = 0;
            uint32_t div_index = 0;
            uint32_t lsu_index = 0;
            uint32_t mul_index = 0;
               
        public:
            issue(component::port<readreg_issue_pack_t> *readreg_issue_port, component::fifo<issue_execute_pack_t> **issue_alu_fifo, component::fifo<issue_execute_pack_t> **issue_bru_fifo, component::fifo<issue_execute_pack_t> **issue_csr_fifo, component::fifo<issue_execute_pack_t> **issue_div_fifo, component::fifo<issue_execute_pack_t> **issue_lsu_fifo, component::fifo<issue_execute_pack_t> **issue_mul_fifo);
            issue_feedback_pack_t run(wb_feedback_pack_t wb_feedback_pack, commit_feedback_pack_t commit_feedback_pack);
            virtual void print(std::string indent);
    };
}
