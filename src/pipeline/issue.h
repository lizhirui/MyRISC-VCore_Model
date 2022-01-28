#pragma once
#include "common.h"
#include "config.h"
#include "../component/port.h"
#include "../component/fifo.h"
#include "../component/issue_queue.h"
#include "readreg_issue.h"
#include "issue_execute.h"

namespace pipeline
{
    typedef struct issue_feedback_pack_t
    {
        bool stall;
    }issue_feedback_pack_t;
    
    class issue
    {
        private:
            typedef struct issue_queue_item_t
            {
                bool enable;//this item has op
                bool valid;//this item has valid op
                uint32_t rob_id;
                uint32_t pc;
                uint32_t imm;

                uint32_t rs1;
                arg_src_t arg1_src;
                bool rs1_need_map;
                uint32_t rs1_phy;
                uint32_t src1_value;
                bool src1_loaded;

                uint32_t rs2;
                arg_src_t arg2_src;
                bool rs2_need_map;
                uint32_t rs2_phy;
                uint32_t src2_value;
                bool src2_loaded;

                uint32_t rd;
                bool rd_enable;
                bool need_rename;
                uint32_t rd_phy;

                uint32_t csr;
                op_t op;
                op_unit_t op_unit;
                
                union
                {
                    alu_op_t alu_op;
                    bru_op_t bru_op;
                    div_op_t div_op;
                    lsu_op_t lsu_op;
                    mul_op_t mul_op;
                    csr_op_t csr_op;
                }sub_op;
            }issue_queue_item_t;
            
            component::port<readreg_issue_pack_t> *readreg_issue_port;
            component::fifo<issue_execute_pack_t> **issue_alu_fifo;
            component::fifo<issue_execute_pack_t> **issue_bru_fifo;
            component::fifo<issue_execute_pack_t> **issue_csr_fifo;
            component::fifo<issue_execute_pack_t> **issue_div_fifo;
            component::fifo<issue_execute_pack_t> **issue_lsu_fifo;
            component::fifo<issue_execute_pack_t> **issue_mul_fifo;
            
            component::issue_queue<issue_queue_item_t> issue_q;
            bool busy = false;
            uint32_t last_index = 0;
            uint32_t alu_index = 0;
            uint32_t bru_index = 0;
            uint32_t csr_index = 0;
            uint32_t div_index = 0;
            uint32_t lsu_index = 0;
            uint32_t mul_index = 0;
               
            public:
            issue(component::port<readreg_issue_pack_t> *readreg_issue_port, component::fifo<issue_execute_pack_t> **issue_alu_fifo, component::fifo<issue_execute_pack_t> **issue_bru_fifo, component::fifo<issue_execute_pack_t> **issue_csr_fifo, component::fifo<issue_execute_pack_t> **issue_div_fifo, component::fifo<issue_execute_pack_t> **issue_lsu_fifo, component::fifo<issue_execute_pack_t> **issue_mul_fifo);
            issue_feedback_pack_t run();
    };
}
