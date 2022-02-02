#include "common.h"
#include "csr.h"

namespace pipeline
{
    namespace execute
    {
        csr::csr(component::fifo<issue_execute_pack_t> *issue_csr_fifo, component::port<execute_wb_pack_t> *csr_wb_port, component::csrfile *csr_file)
        {
            this->issue_csr_fifo = issue_csr_fifo;
            this->csr_wb_port = csr_wb_port;
            this->csr_file = csr_file;
            this->cur_state = state_t::idle;
            this->csr_value = 0;
            this->csr_succ = true;
            memset(&this->rev_pack, 0, sizeof(this->rev_pack));
        }

        void csr::run()
        {
            execute_wb_pack_t send_pack;

            memset(&send_pack, 0, sizeof(send_pack));

            if(this->cur_state == state_t::idle)
            {
                if(!issue_csr_fifo->is_empty())
                {
                    assert(issue_csr_fifo->pop(&this->rev_pack));

                    if(this->rev_pack.enable)
                    {
                        if(this->rev_pack.valid)
                        {
                            this->csr_succ = csr_file->read(this->rev_pack.csr, &this->csr_value);
                            this->cur_state = state_t::calculate;
                        }
                        else
                        {
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
                        }
                    }
                }
            }
            else
            {
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

                if(!csr_succ)
                {
                    send_pack.has_exception = true;
                    send_pack.exception_id = riscv_exception_t::illegal_instruction;
                    send_pack.exception_value = rev_pack.value;
                }
                else
                {
                    send_pack.rd_value = csr_value;

                    switch(rev_pack.sub_op.csr_op)
                    {
                        case csr_op_t::csrrc:
                            csr_value = csr_value & ~(rev_pack.src1_value);
                            break;

                        case csr_op_t::csrrs:
                            csr_value = csr_value | rev_pack.src1_value;
                            break;

                        case csr_op_t::csrrw:
                            csr_value = rev_pack.src1_value;
                            break;
                    }

                    if(!csr_file->write_check(rev_pack.csr, csr_value))
                    {
                        send_pack.has_exception = true;
                        send_pack.exception_id = riscv_exception_t::illegal_instruction;
                        send_pack.exception_value = rev_pack.value;
                    }
                    else
                    {
                        csr_file->write_sync(rev_pack.csr, csr_value);
                    }
                }

                cur_state = state_t::idle;
            }

            csr_wb_port->set(send_pack);
        }
    }
}