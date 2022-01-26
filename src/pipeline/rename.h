#pragma once
#include "common.h"
#include "../component/fifo.h"
#include "../component/port.h"
#include "../component/rat.h"
#include "../component/rob.h"
#include "decode_rename.h"
#include "rename_readreg.h"
#include "issue.h"

namespace pipeline
{
    class rename
    {
        private:
            component::fifo<decode_rename_pack_t> *decode_rename_fifo;
            component::port<rename_readreg_pack_t> *rename_readreg_port;
            component::rat *rat;
            component::rob *rob;
            bool busy;

        public:
            rename(component::fifo<decode_rename_pack_t> *decode_rename_fifo, component::port<rename_readreg_pack_t> *rename_readreg_port, component::rat *rat, component::rob *rob);
            void run(issue_feedback_pack_t issue_pack);
    };
}