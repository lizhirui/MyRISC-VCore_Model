#include "common.h"
#include "fetch.h"
#include "../component/fifo.h"
#include "../component/memory.h"
#include "execute/bru.h"
#include "fetch_decode.h"

namespace pipeline
{
    fetch::fetch(component::memory *memory, component::fifo<fetch_decode_pack_t> *fetch_decode_fifo, uint32_t init_pc)
    {
        this->memory = memory;
        this->fetch_decode_fifo = fetch_decode_fifo;
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
            bool i0_jump = ((i0 & 0x7f) == 0x6f) || ((i0 & 0x7f) == 0x67) || ((i0 & 0x7f) == 0x63);
            uint32_t i1_pc = cur_pc ^ 0x04;
            bool i1_has_exception = !memory->check_align(i1_pc, 4) || !memory->check_boundary(i1_pc, 4);
            uint32_t i1 = i1_has_exception ? 0 : this->memory->read32(i1_pc);
            bool i1_enable = is_align(cur_pc, 8) && !jump_wait && !i0_jump;
            bool i1_jump = ((i1 & 0x7f) == 0x6f) || ((i1 & 0x7f) == 0x67) || ((i1 & 0x7f) == 0x63);

            if(jump_wait)
            {
                if(bru_feedback_pack.enable)
                {
                    this->jump_wait = false;

                    if(bru_feedback_pack.jump)
                    {
                        this->pc = bru_feedback_pack.next_pc;
                    }
                    else
                    {
                        this->pc = (this->pc & (~0x04)) + 8;
                    }
                }
            }
            else if(!this->fetch_decode_fifo->is_full())
            {
                if(i0_jump || i1_jump)
                {
                    this->jump_wait = true;
                }
                else
                {
                    this->pc += (i1_enable ? 8 : 4);
                }

                fetch_decode_pack_t t_fetch_decode_pack;

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

            if(commit_feedback_pack.has_exception)
            {
                this->pc = commit_feedback_pack.exception_pc;
            }
        }
    }

    void fetch::print(std::string indent)
    {
        std::cout << indent << "pc = 0x" << fillzero(8) << outhex(this->pc);
        std::cout << "    jump_wait = " << outbool(this->jump_wait) << std::endl;
    }
}