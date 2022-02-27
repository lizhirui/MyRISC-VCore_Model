#pragma once
#include "common.h"
#include "config.h"

namespace component
{
    class branch_predictor : public if_reset_t
    {
        private:
            uint32_t gshare_global_history = 0;
            uint32_t gshare_pht[GSHARE_PHT_SIZE];

            void gshare_global_history_update(bool jump)
            {
                gshare_global_history = ((gshare_global_history << 1) & GSHARE_GLOBAL_HISTORY_MASK) | (jump ? 1 : 0);
            }

            bool gshare_get_prediction(uint32_t pc)
            {
                uint32_t pc_p1 = (pc >> (2 + GSHARE_PC_P2_ADDR_WIDTH)) & GSHARE_PC_P1_ADDR_MASK;
                uint32_t pc_p2 = (pc >> 2) & GSHARE_PC_P2_ADDR_MASK;
                uint32_t pht_addr = ((gshare_global_history ^ pc_p1) << 6) | pc_p2;
                return gshare_pht[pht_addr] >= 2;
            }

            void gshare_update_prediction(uint32_t pc, bool jump)
            {
                uint32_t pc_p1 = (pc >> (2 + GSHARE_PC_P2_ADDR_WIDTH)) & GSHARE_PC_P1_ADDR_MASK;
                uint32_t pc_p2 = (pc >> 2) & GSHARE_PC_P2_ADDR_MASK;
                uint32_t pht_addr = ((gshare_global_history ^ pc_p1) << GSHARE_PC_P2_ADDR_WIDTH) | pc_p2;

                if(jump)
                {
                    if(gshare_pht[pht_addr] < 3)
                    {
                        gshare_pht[pht_addr]++;
                    }
                }
                else
                {
                    if(gshare_pht[pht_addr] > 0)
                    {
                        gshare_pht[pht_addr]--;
                    }
                }

                gshare_global_history_update(jump);
            }

            uint32_t local_bht[LOCAL_BHT_SIZE];
            uint32_t local_pht[LOCAL_PHT_SIZE];

            void local_update_prediction(uint32_t pc, bool jump)
            {
                uint32_t pc_p1 = (pc >> (2 + LOCAL_PC_P2_ADDR_WIDTH + LOCAL_PC_P3_ADDR_WIDTH)) & LOCAL_PC_P1_ADDR_MASK;
                uint32_t pc_p2 = (pc >> (2 + LOCAL_PC_P3_ADDR_WIDTH)) & LOCAL_PC_P2_ADDR_MASK;
                uint32_t pc_p3 = (pc >> 2) & LOCAL_PC_P3_ADDR_MASK;
                uint32_t bht_value = local_bht[pc_p1];
                uint32_t pht_addr = ((bht_value ^ pc_p2) << LOCAL_PC_P3_ADDR_WIDTH) | pc_p3;     

                local_bht[pc_p1] = ((local_bht[pc_p1] << 1) & LOCAL_BHT_WIDTH_MASK) | (jump ? 1 : 0);
                
                if(jump)
                {
                    if(local_pht[pht_addr] < 3)
                    {
                        local_pht[pht_addr]++;
                    }
                }
                else
                {
                    if(local_pht[pht_addr] > 0)
                    {
                        local_pht[pht_addr]--;
                    }
                }
            }

            bool local_get_prediction(uint32_t pc)
            {
                uint32_t pc_p1 = (pc >> (2 + LOCAL_PC_P2_ADDR_WIDTH + LOCAL_PC_P3_ADDR_WIDTH)) & LOCAL_PC_P1_ADDR_MASK;
                uint32_t pc_p2 = (pc >> (2 + LOCAL_PC_P3_ADDR_WIDTH)) & LOCAL_PC_P2_ADDR_MASK;
                uint32_t pc_p3 = (pc >> 2) & LOCAL_PC_P3_ADDR_MASK;
                uint32_t bht_value = local_bht[pc_p1];
                uint32_t pht_addr = ((bht_value ^ pc_p2) << LOCAL_PC_P3_ADDR_WIDTH) | pc_p3;
                return local_pht[pht_addr] >= 2;
            }

            uint32_t cpht[GSHARE_PHT_SIZE];
            
            void cpht_update_prediction(uint32_t pc, bool hit)
            {
                uint32_t pc_p1 = (pc >> (2 + GSHARE_PC_P2_ADDR_WIDTH)) & GSHARE_PC_P1_ADDR_MASK;
                uint32_t pc_p2 = (pc >> 2) & GSHARE_PC_P2_ADDR_MASK;
                uint32_t cpht_addr = ((gshare_global_history ^ pc_p1) << GSHARE_PC_P2_ADDR_WIDTH) | pc_p2;
                
                if(cpht[cpht_addr] <= 1)//gshare
                {
                    if(hit && (cpht[cpht_addr] > 0))
                    {
                        cpht[cpht_addr]--;
                    }
                    else if(!hit)
                    {
                        cpht[cpht_addr]++;
                    }
                }
                else//local
                {
                    if(hit && (cpht[cpht_addr] < 3))
                    {
                        cpht[cpht_addr]++;
                    }
                    else if(!hit)
                    {
                        cpht[cpht_addr]--;
                    }
                }
            }

            //gshare only when return value is true
            bool cpht_get_prediction(uint32_t pc)
            {
                uint32_t pc_p1 = (pc >> (2 + GSHARE_PC_P2_ADDR_WIDTH)) & GSHARE_PC_P1_ADDR_MASK;
                uint32_t pc_p2 = (pc >> 2) & GSHARE_PC_P2_ADDR_MASK;
                uint32_t cpht_addr = ((gshare_global_history ^ pc_p1) << GSHARE_PC_P2_ADDR_WIDTH) | pc_p2;
                return cpht[cpht_addr] <= 1;
            }

            enum class sync_request_type_t
            {
                update_prediction
            };

            typedef struct sync_request_t
            {
                sync_request_type_t req;
                uint32_t pc;
                uint32_t instruction;
                bool jump;
                uint32_t next_pc;
                bool hit;
            }sync_request_t;

            std::queue<sync_request_t> sync_request_q;

        public:
            virtual void reset()
            {
                gshare_global_history = 0;
                memset(gshare_pht, 0, sizeof(gshare_pht));
                memset(local_bht, 0, sizeof(local_bht));
                memset(local_pht, 0, sizeof(local_pht));
                memset(cpht, 0, sizeof(cpht));
                clear_queue(sync_request_q);
            }

            bool get_prediction(uint32_t pc, uint32_t instruction, bool *jump, uint32_t *next_pc)
            {
                auto op_data = instruction;
                auto opcode = op_data & 0x7f;
                auto rd = (op_data >> 7) & 0x1f;
                auto funct3 = (op_data >> 12) & 0x07;
                auto rs1 = (op_data >> 15) & 0x1f;
                auto rs2 = (op_data >> 20) & 0x1f;
                auto funct7 = (op_data >> 25) & 0x7f;
                auto imm_i = (op_data >> 20) & 0xfff;
                auto imm_s = (((op_data >> 7) & 0x1f)) | (((op_data >> 25) & 0x7f) << 5);
                auto imm_b = (((op_data >> 8) & 0x0f) << 1) | (((op_data >> 25) & 0x3f) << 5) | (((op_data >> 7) & 0x01) << 11) | (((op_data >> 31) & 0x01) << 12);
                auto imm_u = op_data & (~0xfff);
                auto imm_j = (((op_data >> 12) & 0xff) << 12) | (((op_data >> 20) & 0x01) << 11) | (((op_data >> 21) & 0x3ff) << 1) | (((op_data >> 31) & 0x01) << 20);

                auto need_jump_prediction = true;
                auto instruction_next_pc_valid = true;
                uint32_t instruction_next_pc = 0;

                switch(opcode)
                {
                    case 0x6f://jal
                        need_jump_prediction = false;
                        instruction_next_pc_valid = true;
                        instruction_next_pc = pc + sign_extend(imm_j, 21);
                        break;

                    case 0x67://jalr
                        need_jump_prediction = false;
                        instruction_next_pc_valid = false;
                        break;

                    case 0x63://beq bne blt bge bltu bgeu
                        need_jump_prediction = true;
                        instruction_next_pc_valid = cpht_get_prediction(pc) ? gshare_get_prediction(pc) : local_get_prediction(pc);
                        instruction_next_pc = instruction_next_pc_valid ? (pc + sign_extend(imm_b, 13)) : (pc + 4);
                        /*instruction_next_pc_valid = false;
                        instruction_next_pc = pc + sign_extend(imm_b, 13);*/

                        switch(funct3)
                        {
                            case 0x0://beq
                            case 0x1://bne
                            case 0x4://blt
                            case 0x5://bge
                            case 0x6://bltu
                            case 0x7://bgeu
                                break;

                            default://invalid
                                return false;
                        }

                        break;

                    default:
                        return false;
                }

                if(!need_jump_prediction)
                {
                    *jump = true;

                    if(!instruction_next_pc_valid)
                    {
                        return false;
                    }

                    *next_pc = instruction_next_pc;
                }
                else
                {
                    *jump = instruction_next_pc_valid;
                    *next_pc = instruction_next_pc;
                }

                return true;
            }

            void update_prediction(uint32_t pc, uint32_t instruction, bool jump, uint32_t next_pc, bool hit)
            {
                auto op_data = instruction;
                auto opcode = op_data & 0x7f;

                //condition branch instruction
                if(opcode == 0x63)
                {
                    if(cpht_get_prediction(pc))//gshare
                    {
                        gshare_update_prediction(pc, jump);
                    }
                    else//local
                    {
                        local_update_prediction(pc, jump);
                    }

                    cpht_update_prediction(pc, hit);
                }
            }

            void update_prediction_sync(uint32_t pc, uint32_t instruction, bool jump, uint32_t next_pc, bool hit)
            {
                sync_request_t t_req;

                t_req.req = sync_request_type_t::update_prediction;
                t_req.pc = pc;
                t_req.instruction = instruction;
                t_req.jump = jump;
                t_req.next_pc = next_pc;
                t_req.hit = hit;
                sync_request_q.push(t_req);
            }

            void sync()
            {
                sync_request_t t_req;

                while(!sync_request_q.empty())
                {
                    t_req = sync_request_q.front();
                    sync_request_q.pop();

                    switch(t_req.req)
                    {
                        case sync_request_type_t::update_prediction:
                            update_prediction(t_req.pc, t_req.instruction, t_req.jump, t_req.next_pc, t_req.hit);
                            break;
                    }
                }
            }
    };
}