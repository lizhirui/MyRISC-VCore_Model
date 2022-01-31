#pragma once
#include "common.h"
#include "../../component/fifo.h"
#include "../../component/port.h"
#include "../issue_execute.h"
#include "../execute_wb.h"

namespace pipeline
{
    namespace execute
    {
        typedef struct bru_feedback_pack_t
        {
            bool enable;
            bool jump;
            uint32_t next_pc;
        }bru_feedback_pack_t;

        class bru
        {
            private:
                component::fifo<issue_execute_pack_t> *issue_bru_fifo;
                component::port<execute_wb_pack_t> *bru_wb_port;

            public:
                bru(component::fifo<issue_execute_pack_t> *issue_bru_fifo, component::port<execute_wb_pack_t> *bru_wb_port);
                bru_feedback_pack_t run();
        };
    }
}