#include "common.h"
#include "bru.h"

namespace pipeline
{
    namespace execute
    {
        bru::bru(component::fifo<issue_execute_pack_t> *issue_bru_fifo, component::port<execute_wb_pack_t> *bru_wb_port, component::csrfile *csr_file, component::branch_predictor *branch_predictor, component::checkpoint_buffer *checkpoint_buffer)
        {
            this->issue_bru_fifo = issue_bru_fifo;
            this->bru_wb_port = bru_wb_port;
            this->csr_file = csr_file;
            this->branch_predictor = branch_predictor;
            this->checkpoint_buffer = checkpoint_buffer;
        }

        bru_feedback_pack_t bru::run(commit_feedback_pack_t commit_feedback_pack)
        {
            execute_wb_pack_t send_pack;
            bru_feedback_pack_t feedback_pack;

            memset(&send_pack, 0, sizeof(send_pack));
            memset(&feedback_pack, 0, sizeof(feedback_pack));

            if(!issue_bru_fifo->is_empty() && !(commit_feedback_pack.enable && commit_feedback_pack.flush))
            {
                issue_execute_pack_t rev_pack;
                assert(issue_bru_fifo->pop(&rev_pack));
                
                send_pack.enable = rev_pack.enable;
                send_pack.value = rev_pack.value;
                send_pack.valid = rev_pack.valid;
                send_pack.rob_id = rev_pack.rob_id;
                send_pack.pc = rev_pack.pc;
                send_pack.imm = rev_pack.imm;
                send_pack.has_exception = rev_pack.has_exception;
                send_pack.exception_id = rev_pack.exception_id;
                send_pack.exception_value = rev_pack.exception_value;

                send_pack.predicted = rev_pack.predicted;
                send_pack.predicted_jump = rev_pack.predicted_jump;
                send_pack.predicted_next_pc = rev_pack.predicted_next_pc;
                send_pack.checkpoint_id_valid = rev_pack.checkpoint_id_valid;
                send_pack.checkpoint_id = rev_pack.checkpoint_id;

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
                //feedback_pack.enable = true;
                //feedback_pack.next_pc = rev_pack.pc + rev_pack.imm;
                send_pack.bru_next_pc = rev_pack.pc + rev_pack.imm;

                if(rev_pack.enable && rev_pack.valid)
                {
                    switch(rev_pack.sub_op.bru_op)
                    {
                        case bru_op_t::beq:
                            send_pack.bru_jump = rev_pack.src1_value == rev_pack.src2_value;
                            break;

                        case bru_op_t::bge:
                            send_pack.bru_jump = ((int32_t)rev_pack.src1_value) >= ((int32_t)rev_pack.src2_value);
                            break;

                        case bru_op_t::bgeu:
                            send_pack.bru_jump = ((uint32_t)rev_pack.src1_value) >= ((uint32_t)rev_pack.src2_value);
                            break;

                        case bru_op_t::blt:
                            send_pack.bru_jump = ((int32_t)rev_pack.src1_value) < ((int32_t)rev_pack.src2_value);
                            break;

                        case bru_op_t::bltu:
                            send_pack.bru_jump = ((uint32_t)rev_pack.src1_value) < ((uint32_t)rev_pack.src2_value);
                            break;

                        case bru_op_t::bne:
                            send_pack.bru_jump = rev_pack.src1_value != rev_pack.src2_value;
                            break;

                        case bru_op_t::jal:
                            send_pack.rd_value = rev_pack.pc + 4;
                            send_pack.bru_jump = true;
                            break;

                        case bru_op_t::jalr:
                            send_pack.rd_value = rev_pack.pc + 4;
                            send_pack.bru_jump = true;
                            send_pack.bru_next_pc = (rev_pack.imm + rev_pack.src1_value) & (~0x01);
                            break;

                        case bru_op_t::mret:
                            send_pack.bru_jump = true;
                            send_pack.bru_next_pc = csr_file->read_sys(CSR_MEPC);
                            break;
                    }
                }

                if(!send_pack.bru_jump)
                {
                    send_pack.bru_next_pc = rev_pack.pc + 4;
                }

                if(send_pack.valid && send_pack.checkpoint_id_valid)
                {
                    branch_predictor->update_prediction_bru_guess(checkpoint_buffer->get_item(send_pack.checkpoint_id), send_pack.pc, send_pack.value, send_pack.bru_jump, send_pack.bru_next_pc, send_pack.predicted_jump == send_pack.bru_jump);
                }
            }

            //feedback_pack.jump = send_pack.bru_jump;
            //feedback_pack.next_pc = send_pack.bru_next_pc;

            bru_wb_port->set(send_pack);
            execute_feedback_channel_t exe_feedback_pack;
            exe_feedback_pack.enable = send_pack.enable && send_pack.valid && send_pack.rd_enable && send_pack.need_rename;
            exe_feedback_pack.phy_id = send_pack.rd_phy;
            exe_feedback_pack.value = send_pack.rd_value;
            feedback_pack.execute_feedback_channel = exe_feedback_pack;
            return feedback_pack;
        }
    }
}