#pragma once
#include "common.h"
#include "../component/fifo.h"
#include "../component/memory.h"
#include "../component/checkpoint_buffer.h"
#include "../component/branch_predictor.h"
#include "execute/bru.h"
#include "fetch_decode.h"
#include "commit.h"

namespace pipeline
{
    class fetch : public if_print_t, public if_reset_t
    {
        private:
            component::memory *memory;
            component::fifo<fetch_decode_pack_t> *fetch_decode_fifo;
            component::checkpoint_buffer *checkpoint_buffer;
            component::branch_predictor *branch_predictor;
            uint32_t init_pc;
            uint32_t pc;
            bool jump_wait;

        public:
            fetch(component::memory *memory, component::fifo<fetch_decode_pack_t> *fetch_decode_fifo, component::checkpoint_buffer *checkpoint_buffer, component::branch_predictor *branch_predictor, uint32_t init_pc);
            virtual void reset();
            void run(pipeline::execute::bru_feedback_pack_t bru_feedback_pack, commit_feedback_pack_t commit_feedback_pack);
            uint32_t get_pc();
            virtual void print(std::string indent);
            virtual json get_json();
    };
}