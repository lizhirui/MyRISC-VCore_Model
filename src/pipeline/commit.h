#pragma once
#include "common.h"
#include "../component/port.h"
#include "../component/rat.h"
#include "../component/rob.h"
#include "../component/csrfile.h"
#include "wb_commit.h"

namespace pipeline
{
    typedef struct commit_feedback_pack_t
    {
        bool enable;
        uint32_t next_handle_rob_id;
        bool has_exception;
        uint32_t exception_pc;
        bool flush;
    }commit_feedback_pack_t;

    enum class state_t
    {
        normal,
        flush
    };

    class commit
    {
        private:
            component::port<wb_commit_pack_t> *wb_commit_port;
            component::rat *rat;
            component::rob *rob;
            component::csrfile *csr_file;

            state_t cur_state;

            component::rob_item_t rob_item;
            uint32_t rob_item_id;

        public:
            commit(component::port<wb_commit_pack_t> *wb_commit_port, component::rat *rat, component::rob *rob, component::csrfile *csr_file);
            commit_feedback_pack_t run();
    };
}