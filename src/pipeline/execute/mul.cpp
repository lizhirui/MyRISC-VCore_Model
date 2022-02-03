#include "common.h"
#include "mul.h"

namespace pipeline
{
    namespace execute
    {
        mul::mul(component::fifo<issue_execute_pack_t> *issue_mul_fifo, component::port<execute_wb_pack_t> *mul_wb_port)
        {
            this->issue_mul_fifo = issue_mul_fifo;
            this->mul_wb_port = mul_wb_port;
        }

        void mul::run(commit_feedback_pack_t commit_feedback_pack)
        {
            execute_wb_pack_t send_pack;

            memset(&send_pack, 0, sizeof(send_pack));

            if(!issue_mul_fifo->is_empty() && !(commit_feedback_pack.enable && commit_feedback_pack.flush))
            {
                issue_execute_pack_t rev_pack;
                assert(issue_mul_fifo->pop(&rev_pack));
                
                send_pack.enable = rev_pack.enable;
                send_pack.value = rev_pack.value;
                send_pack.valid = rev_pack.valid;
                send_pack.rob_id = rev_pack.rob_id;
                send_pack.pc = rev_pack.pc;
                send_pack.imm = rev_pack.imm;
                send_pack.has_exception = rev_pack.has_exception;
                send_pack.exception_id = rev_pack.exception_id;
                send_pack.exception_value = rev_pack.exception_value;

                send_pack.rs1 = rev_pack.rs1;
                send_pack.arg1_src = rev_pack.arg1_src;
                send_pack.rs1_need_map = rev_pack.rs1_need_map;
                send_pack.rs1_phy = rev_pack.rs1_phy;
                send_pack.src1_value = rev_pack.src1_value;
                send_pack.src1_loaded = rev_pack.src1_loaded;

                send_pack.rs2 = rev_pack.rs2;
                send_pack.arg2_src = rev_pack.arg2_src;
                send_pack.rs2_need_map = rev_pack.rs2_need_map;
                send_pack.rs2_phy = rev_pack.rs2_phy;
                send_pack.src2_value = rev_pack.src2_value;
                send_pack.src2_loaded = rev_pack.src2_loaded;

                send_pack.rd = rev_pack.rd;
                send_pack.rd_enable = rev_pack.rd_enable;
                send_pack.need_rename = rev_pack.need_rename;
                send_pack.rd_phy = rev_pack.rd_phy;

                send_pack.csr = rev_pack.csr;
                send_pack.op = rev_pack.op;
                send_pack.op_unit = rev_pack.op_unit;
                memcpy(&send_pack.sub_op, &rev_pack.sub_op, sizeof(rev_pack.sub_op));

                if(rev_pack.enable && rev_pack.valid)
                {
                    switch(rev_pack.sub_op.mul_op)
                    {
                        case mul_op_t::mul:
                            send_pack.rd_value = (uint32_t)(((uint64_t)(((int64_t)rev_pack.src1_value) * ((int64_t)rev_pack.src2_value))) & 0xffffffffull);
                            break;

                        case mul_op_t::mulh:
                            send_pack.rd_value = (uint32_t)((((uint64_t)(((int64_t)rev_pack.src1_value) * ((int64_t)rev_pack.src2_value))) >> 32) & 0xffffffffull);
                            break;

                        case mul_op_t::mulhsu:
                            send_pack.rd_value = (uint32_t)((((uint64_t)(((int64_t)rev_pack.src1_value) * ((uint64_t)rev_pack.src2_value))) >> 32) & 0xffffffffull);
                            break;

                        case mul_op_t::mulhu:
                            send_pack.rd_value = (uint32_t)((((uint64_t)(((uint64_t)rev_pack.src1_value) * ((int64_t)rev_pack.src2_value))) >> 32) & 0xffffffffull);
                            break;
                    }
                }
            }

            mul_wb_port->set(send_pack);
        }
    }
}