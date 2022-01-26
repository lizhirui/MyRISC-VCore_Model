#include "common.h"
#include "pipeline_common.h"
#include "decode.h"
#include "../component/fifo.h"
#include "fetch_decode.h"
#include "decode_rename.h"

namespace pipeline
{
    decode::decode(component::fifo<fetch_decode_pack> *fetch_decode_fifo, component::fifo<decode_rename_pack> *decode_rename_fifo)
    {
        this->fetch_decode_fifo = fetch_decode_fifo;
        this->decode_rename_fifo = decode_rename_fifo;
    }

    void decode::run()
    {
        if(!this->fetch_decode_fifo->is_empty() && !this->decode_rename_fifo->is_full())
        {
            fetch_decode_pack rev_pack;
            decode_rename_pack send_pack;
            this->fetch_decode_fifo->pop(&rev_pack);
            std::memset(&send_pack, 0, sizeof(send_pack));

            for(auto i = 0;i < FETCH_WIDTH;i++)
            {
                if(rev_pack.op_info[i].enable)
                {
                    auto op_data = rev_pack.op_info[i].value;
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

                    auto op_info = send_pack.op_info[i];
                    op_info.enable = true;
                    op_info.valid = true;
                    op_info.pc = rev_pack.op_info[i].pc;

                    switch(opcode)
                    {
                        case 0x37://lui
                            op_info.op = op::lui;
                            op_info.op_unit = op_unit::alu;
                            op_info.sub_op.alu_op = alu_op::lui;
                            op_info.arg1_src = arg_src::disable;
                            op_info.arg2_src = arg_src::disable;
                            op_info.imm = imm_u;
                            op_info.rd = rd;
                            op_info.rd_enable = true;
                            break;

                        case 0x17://auipc
                            op_info.op = op::lui;
                            op_info.op_unit = op_unit::alu;
                            op_info.sub_op.alu_op = alu_op::lui;
                            op_info.arg1_src = arg_src::disable;
                            op_info.arg2_src = arg_src::disable;
                            op_info.imm = imm_u;
                            op_info.rd = rd;
                            op_info.rd_enable = true;
                            break;

                        case 0x6f://jal
                            op_info.op = op::jal;
                            op_info.op_unit = op_unit::bru;
                            op_info.sub_op.bru_op = bru_op::jal;
                            op_info.arg1_src = arg_src::disable;
                            op_info.arg2_src = arg_src::disable;
                            op_info.imm = sign_extend(imm_j, 21);
                            break;

                        case 0x67://jalr
                            op_info.op = op::jalr;
                            op_info.op_unit = op_unit::bru;
                            op_info.sub_op.bru_op = bru_op::jalr;
                            op_info.arg1_src = arg_src::reg;
                            op_info.rs1 = rs1;
                            op_info.arg2_src = arg_src::disable;
                            op_info.imm = sign_extend(imm_i, 12);
                            break;

                        case 0x63://beq bne blt bge bltu bgeu
                            op_info.op_unit = op_unit::bru;
                            op_info.arg1_src = arg_src::reg;
                            op_info.rs1 = rs1;
                            op_info.arg2_src = arg_src::reg;
                            op_info.rs2 = rs2;
                            op_info.imm = sign_extend(imm_b, 13);

                            switch(funct3)
                            {
                                case 0x0://beq
                                    op_info.op = op::beq;
                                    op_info.sub_op.bru_op = bru_op::beq;
                                    break;

                                case 0x1://bne
                                    op_info.op = op::bne;
                                    op_info.sub_op.bru_op = bru_op::bne;
                                    break;

                                case 0x4://blt
                                    op_info.op = op::blt;
                                    op_info.sub_op.bru_op = bru_op::blt;
                                    break;

                                case 0x5://bge
                                    op_info.op = op::bge;
                                    op_info.sub_op.bru_op = bru_op::bge;
                                    break;

                                case 0x6://bltu
                                    op_info.op = op::bltu;
                                    op_info.sub_op.bru_op = bru_op::bltu;
                                    break;

                                case 0x7://bgeu
                                    op_info.op = op::bgeu;
                                    op_info.sub_op.bru_op = bru_op::bgeu;
                                    break;

                                default://invalid
                                    op_info.valid = false;
                                    break;
                            }

                            break;

                        case 0x03://lb lh lw lbu lhu
                            op_info.op_unit = op_unit::lsu;
                            op_info.arg1_src = arg_src::reg;
                            op_info.rs1 = rs1;
                            op_info.arg2_src = arg_src::disable;
                            op_info.imm = sign_extend(imm_i, 12);
                            op_info.rd = rd;
                            op_info.rd_enable = true;

                            switch(funct3)
                            {
                                case 0x0://lb
                                    op_info.op = op::lb;
                                    op_info.sub_op.lsu_op = lsu_op::lb;
                                    break;

                                case 0x1://lh
                                    op_info.op = op::lh;
                                    op_info.sub_op.lsu_op = lsu_op::lh;
                                    break;

                                case 0x2://lw
                                    op_info.op = op::lw;
                                    op_info.sub_op.lsu_op = lsu_op::lw;
                                    break;

                                case 0x4://lbu
                                    op_info.op = op::lbu;
                                    op_info.sub_op.lsu_op = lsu_op::lbu;
                                    break;

                                case 0x5://lhu
                                    op_info.op = op::lhu;
                                    op_info.sub_op.lsu_op = lsu_op::lhu;
                                    break;

                                default://invalid
                                    op_info.valid = false;
                                    break;
                            }

                            break;

                        case 0x23://sb sh sw
                            op_info.op_unit = op_unit::lsu;
                            op_info.arg1_src = arg_src::reg;
                            op_info.rs1 = rs1;
                            op_info.arg2_src = arg_src::reg;
                            op_info.rs2 = rs2;
                            op_info.imm = sign_extend(imm_s, 12);

                            switch(funct3)
                            {
                                case 0x0://sb
                                    op_info.op = op::sb;
                                    op_info.sub_op.lsu_op = lsu_op::sb;
                                    break;

                                case 0x1://sh
                                    op_info.op = op::sh;
                                    op_info.sub_op.lsu_op = lsu_op::sh;
                                    break;

                                case 0x2://sw
                                    op_info.op = op::sw;
                                    op_info.sub_op.lsu_op = lsu_op::sw;
                                    break;

                                default://invalid
                                    op_info.valid = false;
                                    break;
                            }

                            break;

                        case 0x13://addi slti sltiu xori ori andi slli srli srai
                            op_info.op_unit = op_unit::alu;
                            op_info.arg1_src = arg_src::reg;
                            op_info.rs1 = rs1;
                            op_info.arg2_src = arg_src::imm;
                            op_info.imm = sign_extend(imm_i, 12);
                            op_info.rd = rd;
                            op_info.rd_enable = true;

                            switch(funct3)
                            {
                                case 0x0://addi
                                    op_info.op = op::addi;
                                    op_info.sub_op.alu_op = alu_op::add;
                                    break;

                                case 0x2://slti
                                    op_info.op = op::slti;
                                    op_info.sub_op.alu_op = alu_op::slt;
                                    break;

                                case 0x3://sltiu
                                    op_info.op = op::sltiu;
                                    op_info.sub_op.alu_op = alu_op::sltu;
                                    break;

                                case 0x4://xori
                                    op_info.op = op::xori;
                                    op_info.sub_op.alu_op = alu_op::_xor;
                                    break;

                                case 0x6://ori
                                    op_info.op = op::ori;
                                    op_info.sub_op.alu_op = alu_op::_or;
                                    break;

                                case 0x7://andi
                                    op_info.op = op::andi;
                                    op_info.sub_op.alu_op = alu_op::_and;
                                    break;

                                case 0x1://slli
                                    if(funct7 == 0x00)//slli
                                    {
                                        op_info.op = op::slli;
                                        op_info.sub_op.alu_op = alu_op::sll;
                                    }
                                    else//invalid
                                    {
                                        op_info.valid = false;
                                    }

                                    break;

                                case 0x5://srli srai
                                    if(funct7 == 0x00)//srli
                                    {
                                        op_info.op = op::srli;
                                        op_info.sub_op.alu_op = alu_op::srl;
                                    }
                                    else if(funct7 == 0x20)//srai
                                    {
                                        op_info.op = op::srai;
                                        op_info.sub_op.alu_op = alu_op::sra;
                                    }
                                    else//invalid
                                    {
                                        op_info.valid = false;
                                    }

                                    break;

                                default://invalid
                                    op_info.valid = false;
                                    break;
                            }

                            break;

                        case 0x33://add sub sll slt sltu xor srl sra or and mul mulh mulhsu mulhu div divu rem remu
                            op_info.op_unit = op_unit::alu;
                            op_info.arg1_src = arg_src::reg;
                            op_info.rs1 = rs1;
                            op_info.arg2_src = arg_src::reg;
                            op_info.rs2 = rs2;
                            op_info.rd = rd;
                            op_info.rd_enable = true;

                            switch(funct3)
                            {
                                case 0x0://add sub mul
                                    if(funct7 == 0x00)//add
                                    {
                                        op_info.op = op::add;
                                        op_info.sub_op.alu_op = alu_op::add;
                                    }
                                    else if(funct7 == 0x20)//sub
                                    {
                                        op_info.op = op::sub;
                                        op_info.sub_op.alu_op = alu_op::sub;
                                    }
                                    else if(funct7 == 0x01)//mul
                                    {
                                        op_info.op = op::mul;
                                        op_info.op_unit = op_unit::mul;
                                        op_info.sub_op.mul_op = mul_op::mul;
                                    }
                                    else//invalid
                                    {
                                        op_info.valid = false;
                                    }

                                    break;

                                case 0x1://sll mulh
                                    if(funct7 == 0x00)//sll
                                    {
                                        op_info.op = op::sll;
                                        op_info.sub_op.alu_op = alu_op::sll;
                                    }
                                    else if(funct7 == 0x01)//mulh
                                    {
                                        op_info.op = op::mulh;
                                        op_info.op_unit = op_unit::mul;
                                        op_info.sub_op.mul_op = mul_op::mulh;
                                    }
                                    else//invalid
                                    {
                                        op_info.valid = false;
                                    }

                                    break;

                                case 0x2://slt mulhsu
                                    if(funct7 == 0x00)//slt
                                    {
                                        op_info.op = op::slt;
                                        op_info.sub_op.alu_op = alu_op::slt;
                                    }
                                    else if(funct7 == 0x01)//mulhsu
                                    {
                                        op_info.op = op::mulhsu;
                                        op_info.op_unit = op_unit::mul;
                                        op_info.sub_op.mul_op = mul_op::mulhsu;
                                    }
                                    else//invalid
                                    {
                                        op_info.valid = false;
                                    }

                                    break;

                                case 0x3://sltu mulhu
                                    if(funct7 == 0x00)//sltu
                                    {
                                        op_info.op = op::sltu;
                                        op_info.sub_op.alu_op = alu_op::sltu;
                                    }
                                    else if(funct7 == 0x01)//mulhu
                                    {
                                        op_info.op = op::mulhu;
                                        op_info.op_unit = op_unit::mul;
                                        op_info.sub_op.mul_op = mul_op::mulhu;
                                    }
                                    else//invalid
                                    {
                                        op_info.valid = false;
                                    }

                                    break;

                                case 0x4://xor div
                                    if(funct7 == 0x00)//xor
                                    {
                                        op_info.op = op::_xor;
                                        op_info.sub_op.alu_op = alu_op::_xor;
                                    }
                                    else if(funct7 == 0x01)//div
                                    {
                                        op_info.op = op::div;
                                        op_info.op_unit = op_unit::div;
                                        op_info.sub_op.div_op = div_op::div;
                                    }
                                    else//invalid
                                    {
                                        op_info.valid = false;
                                    }

                                    break;

                                case 0x5://srl sra divu
                                    if(funct7 == 0x00)//srl
                                    {
                                        op_info.op = op::srl;
                                        op_info.sub_op.alu_op = alu_op::srl;
                                    }
                                    else if(funct7 == 0x20)//sra
                                    {
                                        op_info.op = op::sra;
                                        op_info.sub_op.alu_op = alu_op::sra;
                                    }
                                    else if(funct7 == 0x01)//divu
                                    {
                                        op_info.op = op::divu;
                                        op_info.op_unit = op_unit::div;
                                        op_info.sub_op.div_op = div_op::divu;
                                    }
                                    else//invalid
                                    {
                                        op_info.valid = false;
                                    }

                                    break;

                                case 0x6://or rem
                                    if(funct7 == 0x00)//or
                                    {
                                        op_info.op = op::_or;
                                        op_info.sub_op.alu_op = alu_op::_or;
                                    }
                                    else if(funct7 == 0x01)//rem
                                    {
                                        op_info.op = op::rem;
                                        op_info.op_unit = op_unit::div;
                                        op_info.sub_op.div_op = div_op::rem;
                                    }
                                    else//invalid
                                    {
                                        op_info.valid = false;
                                    }

                                    break;

                                case 0x7://and remu
                                    if(funct7 == 0x00)//and
                                    {
                                        op_info.op = op::_and;
                                        op_info.sub_op.alu_op = alu_op::_and;
                                    }
                                    else if(funct7 == 0x01)//remu
                                    {
                                        op_info.op = op::remu;
                                        op_info.op_unit = op_unit::div;
                                        op_info.sub_op.div_op = div_op::remu;
                                    }
                                    else//invalid
                                    {
                                        op_info.valid = false;
                                    }

                                    break;

                                default://invalid
                                    op_info.valid = false;
                                    break;
                            }

                            break;

                        case 0x0f://fence fence.i
                            switch(funct3)
                            {
                                case 0x0://fence
                                    if((rd == 0x00) && (rs1 == 0x00) && (((op_data >> 28) & 0x0f) == 0x00))//fence
                                    {
                                        op_info.op = op::fence;
                                        op_info.op_unit = op_unit::alu;
                                        op_info.sub_op.alu_op = alu_op::fence;
                                        op_info.arg1_src = arg_src::disable;
                                        op_info.arg2_src = arg_src::disable;
                                        op_info.imm = imm_i;
                                    }
                                    else//invalid
                                    {
                                        op_info.valid = false;
                                    }

                                    break;

                                case 0x1://fence.i
                                    if((rd == 0x00) && (rs1 == 0x00) && (imm_i == 0x00))//fence.i
                                    {
                                        op_info.op = op::fence_i;
                                        op_info.op_unit = op_unit::alu;
                                        op_info.sub_op.alu_op = alu_op::fence_i;
                                        op_info.arg1_src = arg_src::disable;
                                        op_info.arg2_src = arg_src::disable;
                                    }
                                    else//invalid
                                    {
                                        op_info.valid = false;
                                    }

                                    break;

                                default://invalid
                                    op_info.valid = false;
                                    break;
                            }
                            break;

                        case 0x73://ecall ebreak csrrw csrrs csrrc csrrwi csrrsi csrrci
                            switch(funct3)
                            {
                                case 0x0://ecall ebreak
                                    if((rd == 0x00) && (rs1 == 0x00))//ecall ebreak
                                    {
                                        switch(imm_i)
                                        {
                                            case 0x00://ecall
                                                op_info.op = op::ecall;
                                                op_info.op_unit = op_unit::alu;
                                                op_info.sub_op.alu_op = alu_op::ecall;
                                                op_info.arg1_src = arg_src::disable;
                                                op_info.arg2_src = arg_src::disable;
                                                break;

                                            case 0x01://ebreak
                                                op_info.op = op::ebreak;
                                                op_info.op_unit = op_unit::alu;
                                                op_info.sub_op.alu_op = alu_op::ebreak;
                                                op_info.arg1_src = arg_src::disable;
                                                op_info.arg2_src = arg_src::disable;
                                                break;

                                            default://invalid
                                                op_info.valid = false;
                                                break;
                                        }
                                    }
                                    else//invalid
                                    {
                                        op_info.valid = false;
                                    }

                                    break;

                                case 0x1://csrrw
                                    op_info.op = op::csrrw;
                                    op_info.op_unit = op_unit::csr;
                                    op_info.sub_op.csr_op = csr_op::csrrw;
                                    op_info.arg1_src = arg_src::reg;
                                    op_info.rs1 = rs1;
                                    op_info.arg2_src = arg_src::disable;
                                    op_info.csr = imm_i;
                                    op_info.rd = rd;
                                    op_info.rd_enable = true;
                                    break;

                                case 0x2://csrrs
                                    op_info.op = op::csrrs;
                                    op_info.op_unit = op_unit::csr;
                                    op_info.sub_op.csr_op = csr_op::csrrs;
                                    op_info.arg1_src = arg_src::reg;
                                    op_info.rs1 = rs1;
                                    op_info.arg2_src = arg_src::disable;
                                    op_info.csr = imm_i;
                                    op_info.rd = rd;
                                    op_info.rd_enable = true;
                                    break;

                                case 0x3://csrrc
                                    op_info.op = op::csrrc;
                                    op_info.op_unit = op_unit::csr;
                                    op_info.sub_op.csr_op = csr_op::csrrc;
                                    op_info.arg1_src = arg_src::reg;
                                    op_info.rs1 = rs1;
                                    op_info.arg2_src = arg_src::disable;
                                    op_info.csr = imm_i;
                                    op_info.rd = rd;
                                    op_info.rd_enable = true;
                                    break;

                                case 0x5://csrrwi
                                    op_info.op = op::csrrwi;
                                    op_info.op_unit = op_unit::csr;
                                    op_info.sub_op.csr_op = csr_op::csrrw;
                                    op_info.arg1_src = arg_src::imm;
                                    op_info.imm = rs1;//zimm
                                    op_info.arg2_src = arg_src::disable;
                                    op_info.csr = imm_i;
                                    op_info.rd = rd;
                                    op_info.rd_enable = true;
                                    break;

                                case 0x6://csrrsi
                                    op_info.op = op::csrrsi;
                                    op_info.op_unit = op_unit::csr;
                                    op_info.sub_op.csr_op = csr_op::csrrs;
                                    op_info.arg1_src = arg_src::imm;
                                    op_info.imm = rs1;//zimm
                                    op_info.arg2_src = arg_src::disable;
                                    op_info.csr = imm_i;
                                    op_info.rd = rd;
                                    op_info.rd_enable = true;
                                    break;

                                case 0x7://csrrci
                                    op_info.op = op::csrrci;
                                    op_info.op_unit = op_unit::csr;
                                    op_info.sub_op.csr_op = csr_op::csrrc;
                                    op_info.arg1_src = arg_src::imm;
                                    op_info.imm = rs1;//zimm
                                    op_info.arg2_src = arg_src::disable;
                                    op_info.csr = imm_i;
                                    op_info.rd = rd;
                                    op_info.rd_enable = true;
                                    break;

                                default://invalid
                                    op_info.valid = false;
                                    break;
                            }
                            break;

                        default://invalid
                            op_info.valid = false;
                            break;
                    }

                    op_info.rs1_need_map = (op_info.arg1_src == arg_src::reg) && (op_info.rs1 > 0);
                    op_info.rs2_need_map = (op_info.arg2_src == arg_src::reg) && (op_info.rs2 > 0);
                    op_info.need_rename = op_info.rd_enable && (op_info.rd > 0);
                    send_pack.op_info[i] = op_info;
                }
            }
            
            decode_rename_fifo->push(send_pack);
        }
    }
}