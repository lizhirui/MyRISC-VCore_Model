#include "common.h"
#include "lsu.h"

namespace pipeline
{
    namespace execute
    {
        lsu::lsu(component::fifo<issue_execute_pack_t> *issue_lsu_fifo, component::port<execute_wb_pack_t> *lsu_wb_port, component::bus *bus, component::store_buffer *store_buffer)
        {
            this->issue_lsu_fifo = issue_lsu_fifo;
            this->lsu_wb_port = lsu_wb_port;
            this->bus = bus;
            this->store_buffer = store_buffer;
            this->busy = false;
            //this->lsu_rev_pack.enable = false;
        }

        void lsu::reset()
        {
            this->busy = false;
            /*this->lsu_rev_pack.enable = false;
            memset(&this->lsu_rev_pack, 0, sizeof(this->lsu_rev_pack));*/
        }

        execute_feedback_channel_t lsu::run(commit_feedback_pack_t commit_feedback_pack)
        {
            execute_wb_pack_t send_pack;

            memset(&send_pack, 0, sizeof(send_pack));

            if((((!issue_lsu_fifo->is_empty()) || this->busy) && !(commit_feedback_pack.enable && commit_feedback_pack.flush)))
            {
                issue_execute_pack_t rev_pack;

                if(this->busy)
                {
                    rev_pack = this->hold_rev_pack;
                    this->busy = false;
                }
                else
                {
                    assert(issue_lsu_fifo->pop(&rev_pack));
                }
                
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

                if(rev_pack.enable && rev_pack.valid)
                {
                    auto addr = rev_pack.lsu_addr;

                    if(!send_pack.has_exception)
                    {
                        component::store_buffer_item_t item;

                        switch(rev_pack.sub_op.lsu_op)
                        {
                            case lsu_op_t::lb:
                                send_pack.rd_value = sign_extend(store_buffer->get_feedback_value(addr, 1, bus->read8(addr)), 8);
                                break;

                            case lsu_op_t::lbu:
                                send_pack.rd_value = store_buffer->get_feedback_value(addr, 1, bus->read8(addr));
                                break;

                            case lsu_op_t::lh:
                                send_pack.rd_value = sign_extend(store_buffer->get_feedback_value(addr, 2, bus->read16(addr)), 16);
                                break;

                            case lsu_op_t::lhu:
                                send_pack.rd_value = store_buffer->get_feedback_value(addr, 2, bus->read16(addr));
                                break;

                            case lsu_op_t::lw:
                                send_pack.rd_value = store_buffer->get_feedback_value(addr, 4, bus->read32(addr));
                                break;

                            case lsu_op_t::sb:
                                if(store_buffer->is_full())
                                {
                                    this->busy = true;
                                    this->hold_rev_pack = rev_pack;
                                    send_pack.enable = false;
                                }
                                else
                                {
                                    item.enable = true;
                                    item.addr = addr;
                                    item.size = 1;
                                    item.data = rev_pack.src2_value & 0xff;
                                    item.committed = false;
                                    item.pc = rev_pack.pc;
                                    item.rob_id = rev_pack.rob_id;
                                    store_buffer->push_sync(item);
                                }

                                break;

                            case lsu_op_t::sh:
                                if(store_buffer->is_full())
                                {
                                    this->busy = true;
                                    this->hold_rev_pack = rev_pack;
                                    send_pack.enable = false;
                                }
                                else
                                {
                                    item.enable = true;
                                    item.addr = addr;
                                    item.size = 2;
                                    item.data = rev_pack.src2_value & 0xffff;
                                    item.committed = false;
                                    item.pc = rev_pack.pc;
                                    item.rob_id = rev_pack.rob_id;
                                    store_buffer->push_sync(item);
                                }

                                break;

                            case lsu_op_t::sw:
                                if(store_buffer->is_full())
                                {
                                    this->busy = true;
                                    this->hold_rev_pack = rev_pack;
                                    send_pack.enable = false;
                                }
                                else
                                {
                                    item.enable = true;
                                    item.addr = addr;
                                    item.size = 4;
                                    item.data = rev_pack.src2_value;
                                    item.committed = false;
                                    item.pc = rev_pack.pc;
                                    item.rob_id = rev_pack.rob_id;
                                    store_buffer->push_sync(item);
                                }

                                break;
                        }
                    }
                }
            }
            else if(commit_feedback_pack.enable && commit_feedback_pack.flush)
            {
                this->busy = false;
            }

            lsu_wb_port->set(send_pack);
            execute_feedback_channel_t feedback_pack;
            feedback_pack.enable = send_pack.enable && send_pack.valid && send_pack.rd_enable && send_pack.need_rename;
            feedback_pack.phy_id = send_pack.rd_phy;
            feedback_pack.value = send_pack.rd_value;
            return feedback_pack;
        }

        /*
        execute_feedback_channel_t lsu::run(commit_feedback_pack_t commit_feedback_pack)
        {
            execute_wb_pack_t send_pack;

            memset(&send_pack, 0, sizeof(send_pack));

            if((!(commit_feedback_pack.enable && commit_feedback_pack.flush)) && this->lsu_rev_pack.enable)
            {
                issue_execute_pack_t rev_pack;

                if(this->busy)
                {
                    rev_pack = this->hold_rev_pack;
                    this->busy = false;
                }
                else
                {
                    rev_pack = this->lsu_rev_pack;
                }
                
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

                if(rev_pack.enable && rev_pack.valid)
                {
                    auto addr = this->lsu_addr;
                    send_pack.exception_value = addr;

                    switch(rev_pack.sub_op.lsu_op)
                    {
                        case lsu_op_t::lb:
                        case lsu_op_t::lbu:
                            send_pack.has_exception = !bus->check_align(addr, 1);
                            send_pack.exception_id = !bus->check_align(addr, 1) ? riscv_exception_t::load_address_misaligned : riscv_exception_t::load_access_fault;
                            break;

                        case lsu_op_t::lh:
                        case lsu_op_t::lhu:
                            send_pack.has_exception = !bus->check_align(addr, 2);
                            send_pack.exception_id = !bus->check_align(addr, 2) ? riscv_exception_t::load_address_misaligned : riscv_exception_t::load_access_fault;
                            break;

                        case lsu_op_t::lw:
                            send_pack.has_exception = !bus->check_align(addr, 4);
                            send_pack.exception_id = !bus->check_align(addr, 4) ? riscv_exception_t::load_address_misaligned : riscv_exception_t::load_access_fault;
                            break;

                        case lsu_op_t::sb:
                            send_pack.has_exception = !bus->check_align(addr, 1);
                            send_pack.exception_id = !bus->check_align(addr, 1) ? riscv_exception_t::store_amo_address_misaligned : riscv_exception_t::store_amo_access_fault;
                            break;

                        case lsu_op_t::sh:
                            send_pack.has_exception = !bus->check_align(addr, 2);
                            send_pack.exception_id = !bus->check_align(addr, 2) ? riscv_exception_t::store_amo_address_misaligned : riscv_exception_t::store_amo_access_fault;
                            break;

                        case lsu_op_t::sw:
                            send_pack.has_exception = !bus->check_align(addr, 4);
                            send_pack.exception_id = !bus->check_align(addr, 4) ? riscv_exception_t::store_amo_address_misaligned : riscv_exception_t::store_amo_access_fault;
                            break;
                    }

                    if(!send_pack.has_exception)
                    {
                        component::store_buffer_item_t item;

                        switch(rev_pack.sub_op.lsu_op)
                        {
                            case lsu_op_t::lb:
                                send_pack.rd_value = sign_extend(store_buffer->get_feedback_value(addr, 1, bus->read8(addr)), 8);
                                break;

                            case lsu_op_t::lbu:
                                send_pack.rd_value = store_buffer->get_feedback_value(addr, 1, bus->read8(addr));
                                break;

                            case lsu_op_t::lh:
                                send_pack.rd_value = sign_extend(store_buffer->get_feedback_value(addr, 2, bus->read16(addr)), 16);
                                break;

                            case lsu_op_t::lhu:
                                send_pack.rd_value = store_buffer->get_feedback_value(addr, 2, bus->read16(addr));
                                break;

                            case lsu_op_t::lw:
                                send_pack.rd_value = store_buffer->get_feedback_value(addr, 4, bus->read32(addr));
                                break;

                            case lsu_op_t::sb:
                                if(store_buffer->is_full())
                                {
                                    this->busy = true;
                                    this->hold_rev_pack = rev_pack;
                                    send_pack.enable = false;
                                }
                                else
                                {
                                    item.enable = true;
                                    item.addr = addr;
                                    item.size = 1;
                                    item.data = rev_pack.src2_value & 0xff;
                                    item.committed = false;
                                    item.pc = rev_pack.pc;
                                    item.rob_id = rev_pack.rob_id;
                                    store_buffer->push_sync(item);
                                }

                                break;

                            case lsu_op_t::sh:
                                if(store_buffer->is_full())
                                {
                                    this->busy = true;
                                    this->hold_rev_pack = rev_pack;
                                    send_pack.enable = false;
                                }
                                else
                                {
                                    item.enable = true;
                                    item.addr = addr;
                                    item.size = 2;
                                    item.data = rev_pack.src2_value & 0xffff;
                                    item.committed = false;
                                    item.pc = rev_pack.pc;
                                    item.rob_id = rev_pack.rob_id;
                                    store_buffer->push_sync(item);
                                }

                                break;

                            case lsu_op_t::sw:
                                if(store_buffer->is_full())
                                {
                                    this->busy = true;
                                    this->hold_rev_pack = rev_pack;
                                    send_pack.enable = false;
                                }
                                else
                                {
                                    item.enable = true;
                                    item.addr = addr;
                                    item.size = 4;
                                    item.data = rev_pack.src2_value;
                                    item.committed = false;
                                    item.pc = rev_pack.pc;
                                    item.rob_id = rev_pack.rob_id;
                                    store_buffer->push_sync(item);
                                }

                                break;
                        }
                    }
                }
            }

            if(!(commit_feedback_pack.enable && commit_feedback_pack.flush))
            {
                if(!this->busy)
                {
                    if(!issue_lsu_fifo->is_empty())
                    {
                        assert(issue_lsu_fifo->pop(&this->lsu_rev_pack));
                        this->lsu_addr = this->lsu_rev_pack.src1_value + this->lsu_rev_pack.imm;
                    }
                    else
                    {
                        this->lsu_rev_pack.enable = false;
                        memset(&this->lsu_rev_pack, 0, sizeof(this->lsu_rev_pack));
                    }
                }
            }
            else
            {
                this->busy = false;
                this->lsu_rev_pack.enable = false;
                memset(&this->lsu_rev_pack, 0, sizeof(this->lsu_rev_pack));
            }

            if(!send_pack.enable)
            {
                memset(&send_pack, 0, sizeof(send_pack));
            }

            lsu_wb_port->set(send_pack);
            execute_feedback_channel_t feedback_pack;
            feedback_pack.enable = send_pack.enable && send_pack.valid && send_pack.rd_enable && send_pack.need_rename;
            feedback_pack.phy_id = send_pack.rd_phy;
            feedback_pack.value = send_pack.rd_value;
            return feedback_pack;
        }*/
    }
}