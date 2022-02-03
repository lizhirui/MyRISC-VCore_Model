#pragma once
#include "common.h"
#include "../component/fifo.h"
#include "fetch_decode.h"
#include "decode_rename.h"
#include "commit.h"

namespace pipeline
{
    class decode
    {
        private:
            component::fifo<fetch_decode_pack_t> *fetch_decode_fifo;
            component::fifo<decode_rename_pack_t> *decode_rename_fifo;

        public:
            decode(component::fifo<fetch_decode_pack_t> *fetch_decode_fifo, component::fifo<decode_rename_pack_t> *decode_rename_fifo);
            void run(commit_feedback_pack_t commit_feedback_pack);
    };
}