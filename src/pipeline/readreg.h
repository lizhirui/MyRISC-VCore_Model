#pragma once
#include "common.h"
#include "../component/port.h"
#include "../component/regfile.h"
#include "../component/checkpoint_buffer.h"
#include "../component/rat.h"
#include "rename_readreg.h"
#include "readreg_issue.h"
#include "issue.h"
#include "execute.h"
#include "commit.h"

namespace pipeline
{
    typedef struct readreg_feedback_pack_t
    {
        
    }readreg_feedback_pack_t;

    class readreg
    {
        private:
            component::port<rename_readreg_pack_t> *rename_readreg_port;
            component::port<readreg_issue_pack_t> *readreg_issue_port;
            component::regfile<phy_regfile_item_t> *phy_regfile;
            component::checkpoint_buffer *checkpoint_buffer;
            component::rat *rat;

        public:
            readreg(component::port<rename_readreg_pack_t> *rename_readreg_port, component::port<readreg_issue_pack_t> *readreg_issue_port, component::regfile<phy_regfile_item_t> *phy_regfile, component::checkpoint_buffer *checkpoint_buffer, component::rat *rat);
            void run(issue_feedback_pack_t issue_pack, execute_feedback_pack_t execute_feedback_pack, wb_feedback_pack_t wb_feedback_pack, commit_feedback_pack_t commit_feedback_pack);
    };
}
