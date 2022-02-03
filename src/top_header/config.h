#pragma once
#include "common.h"
const uint32_t FETCH_WIDTH = 2;
const uint32_t DECODE_WIDTH = 2;
const uint32_t RENAME_WIDTH = 2;
const uint32_t READREG_WIDTH = 2;
const uint32_t ISSUE_WIDTH = 2;
const uint32_t COMMIT_WIDTH = 2;

const uint32_t PHY_REG_NUM = 512;
const uint32_t ARCH_REG_NUM = 32;

const uint32_t ISSUE_QUEUE_SIZE = 16;
const uint32_t ALU_UNIT_NUM = 2;
const uint32_t BRU_UNIT_NUM = 1;
const uint32_t CSR_UNIT_NUM = 1;
const uint32_t DIV_UNIT_NUM = 1;
const uint32_t LSU_UNIT_NUM = 1;
const uint32_t MUL_UNIT_NUM = 2;

const uint32_t EXECUTE_UNIT_NUM = ALU_UNIT_NUM + BRU_UNIT_NUM + CSR_UNIT_NUM + DIV_UNIT_NUM + LSU_UNIT_NUM + MUL_UNIT_NUM;
