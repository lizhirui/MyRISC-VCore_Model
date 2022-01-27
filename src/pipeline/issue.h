#pragma once
#include "common.h"
#include "../component/port.h"
#include "readreg_issue.h"

namespace pipeline
{
    typedef struct issue_feedback_pack_t
    {
        bool stall;
    }issue_feedback_pack_t;
    
    class issue
    {
        private:
            component::port<readreg_issue_pack_t> *readreg_issue_port;
            
            //component::
            
        
        public:
            issue();
            issue_feedback_pack_t run();
    };
}
