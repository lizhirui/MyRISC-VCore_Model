#pragma once
#include "common.h"

namespace pipeline
{
    enum class op_unit
    {
        alu,
        mul,
        div,
        lsu,
        bru,
        csr
    };

    enum class arg_src
    {
        reg,
        imm,
        disable
    };

    enum class op
    {
        add,
        addi,
        _and,
        andi,
        auipc,
        csrrc,
        csrrci,
        csrrs,
        csrrsi,
        csrrw,
        csrrwi,
        ebreak,
        ecall,
        fence,
        fence_i,
        lui,
        _or,
        ori,
        sll,
        slli,
        slt,
        slti,
        sltiu,
        sltu,
        sra,
        srai,
        srl,
        srli,
        sub,
        _xor,
        xori,
        beq,
        bge,
        bgeu,
        blt,
        bltu,
        bne,
        jal,
        jalr,
        div,
        divu,
        rem,
        remu,
        lb,
        lbu,
        lh,
        lhu,
        lw,
        sb,
        sh,
        sw,
        mul,
        mulh,
        mulhsu,
        mulhu
    };

    enum class alu_op
    {
        add,
        _and,
        auipc,
        ebreak,
        ecall,
        fence,
        fence_i,
        lui,
        _or,
        sll,
        slt,
        sltu,
        sra,
        srl,
        sub,
        _xor
    };

    enum class bru_op
    {
        beq,
        bge,
        bgeu,
        blt,
        bltu,
        bne,
        jal,
        jalr
    };

    enum class div_op
    {
        div,
        divu,
        rem,
        remu
    };

    enum class lsu_op
    {
        lb,
        lbu,
        lh,
        lhu,
        lw,
        sb,
        sh,
        sw
    };

    enum class mul_op
    {
        mul,
        mulh,
        mulhsu,
        mulhu
    };

    enum class csr_op
    {
        csrrc,
        csrrs,
        csrrw
    };

    typedef struct phy_regfile_item
    {
        uint32_t value;
        bool valid;
    }phy_regfile_item;
}