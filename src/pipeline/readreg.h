#pragma once
#include "common.h"
#include "../component/port.h"
#include "../component/regfile.h"
#include "rename_readreg.h"
#include "readreg_issue.h"
#include "issue.h"

namespace pipeline
{
    typedef struct readreg_feedback_pack
    {
        
    }readreg_feedback_pack;

    class readreg
    {
        private:
            component::port<rename_readreg_pack> *rename_readreg_port;
            component::port<readreg_issue_pack> *readreg_issue_port;
            component::regfile<phy_regfile_item> *phy_regfile;

        public:
            readreg(component::port<rename_readreg_pack> *rename_readreg_port, component::port<readreg_issue_pack> *readreg_issue_port, component::regfile<phy_regfile_item> *phy_regfile);
            void run(issue_feedback_pack issue_pack);
    };
}