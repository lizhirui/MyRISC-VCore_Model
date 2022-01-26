#pragma once
#include "common.h"
#include "../component/fifo.h"
#include "../component/memory.h"
#include "execute/bru.h"
#include "fetch_decode.h"

namespace pipeline
{
    class fetch
    {
        private:
            component::memory *memory;
            component::fifo<fetch_decode_pack> *fetch_decode_fifo;
            uint32_t pc;
            bool jump_wait;

        public:
            fetch(component::memory *memory, component::fifo<fetch_decode_pack> *fetch_decode_fifo, uint32_t init_pc);
            void run(pipeline::execute::bru_feedback_pack bru_feedback_pack);
    };
}