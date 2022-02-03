#pragma once
#include "common.h"
#include "../../component/fifo.h"
#include "../../component/port.h"
#include "../issue_execute.h"
#include "../execute_wb.h"
#include "../commit.h"

namespace pipeline
{
    namespace execute
    {
        class div
        {
            private:
                component::fifo<issue_execute_pack_t> *issue_div_fifo;
                component::port<execute_wb_pack_t> *div_wb_port;

            public:
                div(component::fifo<issue_execute_pack_t> *issue_div_fifo, component::port<execute_wb_pack_t> *div_wb_port);
                void run(commit_feedback_pack_t commit_feedback_pack);
        };
    }
}