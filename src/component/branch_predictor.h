#pragma once
#include "common.h"
#include "config.h"

namespace component
{
    class branch_predictor : public if_reset_t
    {
        public:
            virtual void reset()
            {
            
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
                        instruction_next_pc_valid = true;
                        instruction_next_pc = pc + sign_extend(imm_b, 13);

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
                    *jump = true;
                    *next_pc = instruction_next_pc;
                }

                return true;
            }

            void update_prediction(uint32_t pc, uint32_t instruction, bool jump, uint32_t next_pc)
            {
                
            }

            void update_prediction_sync(uint32_t pc, uint32_t instruction, bool jump, uint32_t next_pc)
            {
                
            }

            void sync()
            {
                
            }
    };
}