#pragma once
#include "common.h"
#include "../component/fifo.h"
#include "fetch_decode.h"
#include "decode_rename.h"

namespace pipeline
{
    class decode
    {
        private:
            component::fifo<fetch_decode_pack> *fetch_decode_fifo;
            component::fifo<decode_rename_pack> *decode_rename_fifo;

        public:
            decode(component::fifo<fetch_decode_pack> *fetch_decode_fifo, component::fifo<decode_rename_pack> *decode_rename_fifo);
            void run();
    };
}