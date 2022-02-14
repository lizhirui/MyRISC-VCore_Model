#pragma once
#include "common.h"
#include "../component/fifo.h"
#include "../component/memory.h"
#include "execute/bru.h"
#include "fetch_decode.h"
#include "commit.h"

namespace pipeline
{
    class fetch : public if_print_t
    {
        private:
            component::memory *memory;
            component::fifo<fetch_decode_pack_t> *fetch_decode_fifo;
            uint32_t pc;
            bool jump_wait;

        public:
            fetch(component::memory *memory, component::fifo<fetch_decode_pack_t> *fetch_decode_fifo, uint32_t init_pc);
            void run(pipeline::execute::bru_feedback_pack_t bru_feedback_pack, commit_feedback_pack_t commit_feedback_pack);
            uint32_t get_pc();
            virtual void print(std::string indent);
    };
}