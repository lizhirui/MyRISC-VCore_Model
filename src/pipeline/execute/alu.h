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
        class alu
        {
            private:
                component::fifo<issue_execute_pack_t> *issue_alu_fifo;
                component::port<execute_wb_pack_t> *alu_wb_port;

            public:
                alu(component::fifo<issue_execute_pack_t> *issue_alu_fifo, component::port<execute_wb_pack_t> *alu_wb_port);
                void run();
        };
    }
}