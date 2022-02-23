#pragma once
#include "common.h"
#include "../../component/fifo.h"
#include "../../component/port.h"
#include "../../component/memory.h"
#include "../../component/store_buffer.h"
#include "../issue_execute.h"
#include "../execute_wb.h"
#include "../commit.h"

namespace pipeline
{
    namespace execute
    {
        class lsu : if_reset_t
        {
            private:
                component::fifo<issue_execute_pack_t> *issue_lsu_fifo;
                component::port<execute_wb_pack_t> *lsu_wb_port;
                component::memory *memory;
                component::store_buffer *store_buffer;

                bool busy;
                issue_execute_pack_t hold_rev_pack;

            public:
                lsu(component::fifo<issue_execute_pack_t> *issue_lsu_fifo, component::port<execute_wb_pack_t> *lsu_wb_port, component::memory *memory, component::store_buffer *store_buffer);
                virtual void reset();
                void run(commit_feedback_pack_t commit_feedback_pack);
        };
    }
}