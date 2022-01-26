#pragma once
#include "common.h"

namespace pipeline
{
    namespace execute
    {
        typedef struct bru_feedback_pack
        {
            bool enable;
            bool jump;
            uint32_t next_pc;
        }bru_feedback_pack;
    }
}