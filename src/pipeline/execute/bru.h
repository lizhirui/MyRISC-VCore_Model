#pragma once
#include "common.h"
#include "../../component/fifo.h"
#include "../../component/port.h"
#include "../../component/csrfile.h"
#include "../../component/csr_all.h"
#include "../issue_execute.h"
#include "../execute_wb.h"
#include "../commit.h"

namespace pipeline
{
    namespace execute
    {
        typedef struct bru_feedback_pack_t : if_print_t
        {
            bool enable;
            bool jump;
            uint32_t next_pc;

            virtual json get_json()
            {
                json t;
                t["enable"] = enable;
                t["jump"] = jump;
                t["next_pc"] = next_pc;
                return t;
            }
        }bru_feedback_pack_t;

        class bru
        {
            private:
                component::fifo<issue_execute_pack_t> *issue_bru_fifo;
                component::port<execute_wb_pack_t> *bru_wb_port;
                component::csrfile *csr_file;

            public:
                bru(component::fifo<issue_execute_pack_t> *issue_bru_fifo, component::port<execute_wb_pack_t> *bru_wb_port, component::csrfile *csr_file);
                bru_feedback_pack_t run(commit_feedback_pack_t commit_feedback_pack);
        };
    }
}