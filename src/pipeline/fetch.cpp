#include "common.h"
#include "fetch.h"
#include "../component/fifo.h"
#include "../component/memory.h"
#include "execute/bru.h"
#include "fetch_decode.h"

namespace pipeline
{
    fetch::fetch(component::memory *memory, component::fifo<fetch_decode_pack_t> *fetch_decode_fifo, component::checkpoint_buffer *checkpoint_buffer, component::branch_predictor *branch_predictor, uint32_t init_pc)
    {
        this->memory = memory;
        this->fetch_decode_fifo = fetch_decode_fifo;
        this->checkpoint_buffer = checkpoint_buffer;
        this->branch_predictor = branch_predictor;
        this->init_pc = init_pc;
        this->pc = init_pc;
        this->jump_wait = false;
    }

    void fetch::reset()
    {
        this->pc = init_pc;
        this->jump_wait = false;
    }

    void fetch::run(pipeline::execute::bru_feedback_pack_t bru_feedback_pack, commit_feedback_pack_t commit_feedback_pack)
    {
        if(!(commit_feedback_pack.enable && commit_feedback_pack.flush))
        {
            uint32_t cur_pc = this->pc;
            uint32_t i0_pc = cur_pc;
            bool i0_has_exception = !memory->check_align(i0_pc, 4) || !memory->check_boundary(i0_pc, 4);
            uint32_t i0 = i0_has_exception ? 0 : this->memory->read32(i0_pc);
            bool i0_enable = !jump_wait;
            bool i0_jump = ((i0 & 0x7f) == 0x6f) || ((i0 & 0x7f) == 0x67) || ((i0 & 0x7f) == 0x63) || (i0 == 0x30200073);
            //uint32_t i1_pc = cur_pc ^ 0x04;
            uint32_t i1_pc = cur_pc + 4;
            bool i1_has_exception = !memory->check_align(i1_pc, 4) || !memory->check_boundary(i1_pc, 4);
            uint32_t i1 = i1_has_exception ? 0 : this->memory->read32(i1_pc);
            //bool i1_enable = is_align(cur_pc, 8) && !jump_wait && !i0_jump;
            bool i1_enable = !jump_wait && !i0_jump;
            bool i1_jump = ((i1 & 0x7f) == 0x6f) || ((i1 & 0x7f) == 0x67) || ((i1 & 0x7f) == 0x63) || (i1 == 0x30200073);

            if(jump_wait)
            {
                /*if(bru_feedback_pack.enable)
                {
                    this->jump_wait = false;

                    if(bru_feedback_pack.jump)
                    {
                        this->pc = bru_feedback_pack.next_pc;
                    }
                }*/

                if(commit_feedback_pack.jump_enable)
                {
                    this->jump_wait = false;

                    if(commit_feedback_pack.jump)
                    {
                        this->pc = commit_feedback_pack.next_pc;
                    }
                }
            }
            else if(!this->fetch_decode_fifo->is_full())
            {
                fetch_decode_pack_t t_fetch_decode_pack;
                
                memset(&t_fetch_decode_pack, 0, sizeof(t_fetch_decode_pack));

                if((i0_enable && i0_jump) || (i1_enable && i1_jump))
                {
                    uint32_t jump_pc = (i0_enable && i0_jump) ? i0_pc : i1_pc;
                    uint32_t jump_instruction = (i0_enable && i0_jump) ? i0 : i1;
                    uint32_t jump_index = (i0_enable && i0_jump) ? 0 : 1;
                    uint32_t jump_next_pc = 0;
                    bool jump_result = false;

                    /*uint32_t i1_jump_next_pc = 0;
                    bool i1_jump_result = false;
                    bool i1_can_prediction = branch_predictor->get_prediction(i1_pc, i1, &i1_jump_result, &i1_jump_next_pc);*/

                    if(branch_predictor->get_prediction(jump_pc, jump_instruction, &jump_result, &jump_next_pc))
                    {
                        component::checkpoint_t cp;
                        t_fetch_decode_pack.op_info[jump_index].checkpoint_id_valid = checkpoint_buffer->push(cp, &t_fetch_decode_pack.op_info[jump_index].checkpoint_id);
                        
                        if(!t_fetch_decode_pack.op_info[jump_index].checkpoint_id_valid)
                        {
                            this->jump_wait = true;
                            this->pc += (i1_enable ? 8 : 4);
                        }
                        else
                        {
                            t_fetch_decode_pack.op_info[jump_index].predicted = true;
                            t_fetch_decode_pack.op_info[jump_index].predicted_jump = jump_result;
                            t_fetch_decode_pack.op_info[jump_index].predicted_next_pc = jump_next_pc;
                            uint32_t next_pc = jump_result ? jump_next_pc : (this->pc + (i1_enable ? 8 : 4));

                            //judge i1_enable isn't necessary
                            /*if(i0_enable && i0_jump && (next_pc == i1_pc))
                            {
                                i1_enable = true;

                                if(i1_jump)
                                {
                                    if(i1_can_prediction)
                                    {
                                        t_fetch_decode_pack.op_info[1].checkpoint_id_valid = checkpoint_buffer->push(cp, &t_fetch_decode_pack.op_info[1].checkpoint_id);

                                        if(!t_fetch_decode_pack.op_info[1].checkpoint_id_valid)
                                        {
                                            this->jump_wait = true;
                                            this->pc = i1_pc + 4;
                                        }
                                        else
                                        {
                                            t_fetch_decode_pack.op_info[1].predicted = true;
                                            t_fetch_decode_pack.op_info[1].predicted_jump = i1_jump_result;
                                            t_fetch_decode_pack.op_info[1].predicted_next_pc = i1_jump_next_pc;
                                            this->pc = i1_jump_result ? i1_jump_next_pc : (i1_pc + 4);
                                            this->jump_wait = false;
                                        }
                                    }
                                }
                                else
                                {
                                    this->pc = i1_pc + 4;
                                    this->jump_wait = false;
                                }
                            }
                            else
                            {
                                this->pc = next_pc;
                                this->jump_wait = false;
                            }*/

                            this->pc = next_pc;
                            this->jump_wait = false;
                        }
                    }
                    else
                    {
                        this->jump_wait = true;
                        this->pc += (i1_enable ? 8 : 4);
                    }
                }
                else
                {
                    this->pc += (i1_enable ? 8 : 4);
                }

                t_fetch_decode_pack.op_info[0].value = i0_enable ? i0 : 0;
                t_fetch_decode_pack.op_info[0].enable = i0_enable;
                t_fetch_decode_pack.op_info[0].pc = i0_pc;
                t_fetch_decode_pack.op_info[0].has_exception = i0_has_exception;
                t_fetch_decode_pack.op_info[0].exception_id = !memory->check_align(i0_pc, 4) ? riscv_exception_t::instruction_address_misaligned : riscv_exception_t::instruction_access_fault;
                t_fetch_decode_pack.op_info[0].exception_value = i0_pc;
                t_fetch_decode_pack.op_info[1].value = i1_enable ? i1 : 0;
                t_fetch_decode_pack.op_info[1].enable = i1_enable;
                t_fetch_decode_pack.op_info[1].pc = i1_pc;
                t_fetch_decode_pack.op_info[1].has_exception = i1_has_exception;
                t_fetch_decode_pack.op_info[1].exception_id = !memory->check_align(i1_pc, 4) ? riscv_exception_t::instruction_address_misaligned : riscv_exception_t::instruction_access_fault;
                t_fetch_decode_pack.op_info[1].exception_value = i1_pc;

                this->fetch_decode_fifo->push(t_fetch_decode_pack);
            }
        }
        else
        {
            this->fetch_decode_fifo->flush();
            this->jump_wait = false;

            if(commit_feedback_pack.has_exception)
            {
                this->pc = commit_feedback_pack.exception_pc;
            }
            else if(commit_feedback_pack.jump_enable)
            {
                this->pc = commit_feedback_pack.next_pc;
            }
        }
    }

    uint32_t fetch::get_pc()
    {
        return this->pc;
    }

    void fetch::print(std::string indent)
    {
        std::cout << indent << "pc = 0x" << fillzero(8) << outhex(this->pc);
        std::cout << "    jump_wait = " << outbool(this->jump_wait) << std::endl;
    }

    json fetch::get_json()
    {
        json j;
        j["pc"] = this->pc;
        j["jump_wait"] = this->jump_wait;
        return j;
    }
}