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
        class csr
        {
            private:
                component::fifo<issue_execute_pack_t> *issue_csr_fifo;
                component::port<execute_wb_pack_t> *csr_wb_port;

            public:
                csr(component::fifo<issue_execute_pack_t> *issue_csr_fifo, component::port<execute_wb_pack_t> *csr_wb_port);
                void run();
        };
    }
}