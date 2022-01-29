#include "common.h"
#include "config.h"
#include "../test/test.h"
#include "../component/fifo.h"
#include "../component/port.h"
#include "../component/memory.h"
#include "../component/regfile.h"
#include "../pipeline/fetch.h"
#include "../pipeline/fetch_decode.h"
#include "../pipeline/decode.h"
#include "../pipeline/decode_rename.h"
#include "../pipeline/rename.h"
#include "../pipeline/rename_readreg.h"
#include "../pipeline/readreg.h"
#include "../pipeline/readreg_issue.h"
#include "../pipeline/issue.h"
#include "../pipeline/issue_execute.h"
#include "../pipeline/execute/alu.h"
#include "../pipeline/execute/bru.h"
#include "../pipeline/execute/csr.h"
#include "../pipeline/execute/div.h"
#include "../pipeline/execute/lsu.h"
#include "../pipeline/execute/mul.h"
#include "../pipeline/execute_wb.h"

#ifdef WIN32
    #include <windows.h>
#endif

static volatile bool ctrl_c_detected = false;

static uint64_t cpu_clock_cycle = 0;

static component::fifo<pipeline::fetch_decode_pack_t> fetch_decode_fifo(16);
static component::fifo<pipeline::decode_rename_pack_t> decode_rename_fifo(16);
static pipeline::rename_readreg_pack_t default_rename_readreg_pack;
static pipeline::execute_wb_pack_t default_execute_wb_pack;

static component::port<pipeline::rename_readreg_pack_t> rename_readreg_port(default_rename_readreg_pack);
static pipeline::readreg_issue_pack_t default_readreg_issue_pack;
static component::port<pipeline::readreg_issue_pack_t> readreg_issue_port(default_readreg_issue_pack);
static component::fifo<pipeline::issue_execute_pack_t> *issue_alu_fifo[ALU_UNIT_NUM];
static component::fifo<pipeline::issue_execute_pack_t> *issue_bru_fifo[BRU_UNIT_NUM];
static component::fifo<pipeline::issue_execute_pack_t> *issue_csr_fifo[CSR_UNIT_NUM];
static component::fifo<pipeline::issue_execute_pack_t> *issue_div_fifo[DIV_UNIT_NUM];
static component::fifo<pipeline::issue_execute_pack_t> *issue_lsu_fifo[LSU_UNIT_NUM];
static component::fifo<pipeline::issue_execute_pack_t> *issue_mul_fifo[MUL_UNIT_NUM];

static component::port<pipeline::execute_wb_pack_t> *alu_wb_port[ALU_UNIT_NUM];
static component::port<pipeline::execute_wb_pack_t> *bru_wb_port[BRU_UNIT_NUM];
static component::port<pipeline::execute_wb_pack_t> *csr_wb_port[CSR_UNIT_NUM];
static component::port<pipeline::execute_wb_pack_t> *div_wb_port[DIV_UNIT_NUM];
static component::port<pipeline::execute_wb_pack_t> *lsu_wb_port[LSU_UNIT_NUM];
static component::port<pipeline::execute_wb_pack_t> *mul_wb_port[MUL_UNIT_NUM];

static component::memory memory(0x80000000, 64 * 0x1000);
static component::rat rat(PHY_REG_NUM, ARCH_REG_NUM);
static component::rob rob(16);
static component::regfile<pipeline::phy_regfile_item_t> phy_regfile(PHY_REG_NUM);

static pipeline::fetch fetch_stage(&memory, &fetch_decode_fifo, 0x80000000);
static pipeline::decode decode_stage(&fetch_decode_fifo, &decode_rename_fifo);
static pipeline::rename rename_stage(&decode_rename_fifo, &rename_readreg_port, &rat, &rob);
static pipeline::readreg readreg_stage(&rename_readreg_port, &readreg_issue_port, &phy_regfile);
static pipeline::issue issue_stage(&readreg_issue_port, issue_alu_fifo, issue_bru_fifo, issue_csr_fifo, issue_div_fifo, issue_lsu_fifo, issue_mul_fifo);
static pipeline::execute::alu *execute_alu_stage[ALU_UNIT_NUM];

static pipeline::issue_feedback_pack_t t_issue_feedback_pack;
static pipeline::execute::bru_feedback_pack t_bru_feedback_pack;

static void init()
{
    memset(&default_rename_readreg_pack, 0, sizeof(default_rename_readreg_pack));
    memset(&default_readreg_issue_pack, 0, sizeof(default_readreg_issue_pack));
    memset(&default_execute_wb_pack, 0, sizeof(default_execute_wb_pack));

    for(auto i = 0;i < ALU_UNIT_NUM;i++)
    {
        issue_alu_fifo[i] = new component::fifo<pipeline::issue_execute_pack_t>(16);
    }
    
    for(auto i = 0;i < BRU_UNIT_NUM;i++)
    {
        issue_bru_fifo[i] = new component::fifo<pipeline::issue_execute_pack_t>(16);
    }
 
    for(auto i = 0;i < CSR_UNIT_NUM;i++)
    {
        issue_csr_fifo[i] = new component::fifo<pipeline::issue_execute_pack_t>(16);
    }

    for(auto i = 0;i < DIV_UNIT_NUM;i++)
    {
        issue_div_fifo[i] = new component::fifo<pipeline::issue_execute_pack_t>(16);
    }

    for(auto i = 0;i < LSU_UNIT_NUM;i++)
    {
        issue_lsu_fifo[i] = new component::fifo<pipeline::issue_execute_pack_t>(16);
    }

    for(auto i = 0;i < MUL_UNIT_NUM;i++)
    {
        issue_mul_fifo[i] = new component::fifo<pipeline::issue_execute_pack_t>(16);
    }

    for(auto i = 0;i < ALU_UNIT_NUM;i++)
    {
        alu_wb_port[i] = new component::port<pipeline::execute_wb_pack_t>(default_execute_wb_pack);
    }
    
    for(auto i = 0;i < BRU_UNIT_NUM;i++)
    {
        bru_wb_port[i] = new component::port<pipeline::execute_wb_pack_t>(default_execute_wb_pack);
    }
 
    for(auto i = 0;i < CSR_UNIT_NUM;i++)
    {
        csr_wb_port[i] = new component::port<pipeline::execute_wb_pack_t>(default_execute_wb_pack);
    }

    for(auto i = 0;i < DIV_UNIT_NUM;i++)
    {
        div_wb_port[i] = new component::port<pipeline::execute_wb_pack_t>(default_execute_wb_pack);
    }

    for(auto i = 0;i < LSU_UNIT_NUM;i++)
    {
        lsu_wb_port[i] = new component::port<pipeline::execute_wb_pack_t>(default_execute_wb_pack);
    }

    for(auto i = 0;i < MUL_UNIT_NUM;i++)
    {
        mul_wb_port[i] = new component::port<pipeline::execute_wb_pack_t>(default_execute_wb_pack);
    }

    for(auto i = 0;i < ALU_UNIT_NUM;i++)
    {
        execute_alu_stage[i] = new pipeline::execute::alu(issue_alu_fifo[i], alu_wb_port[i]);
    }

    std::memset(&t_issue_feedback_pack, 0, sizeof(t_issue_feedback_pack));
    std::memset(&t_bru_feedback_pack, 0, sizeof(t_bru_feedback_pack));

    rat.init_start();

    for(uint32_t i = 1;i < 32;i++)
    {
        rat.set_map(i, i);
    }

    rat.init_finish();
}

static bool pause_state = false;
static bool step_state = false;

static void cmd_quit()
{
    exit(0);
}

static void cmd_continue()
{
    pause_state = false;
    step_state = false;
}
static void cmd_step()
{
    pause_state = false;
    step_state = true;
}

static void cmd_print()
{
    std::cout << "FETCH:" << std::endl;
    fetch_stage.print("\t");
    std::cout << std::endl;
    std::cout << "FETCH -> DECODE:" << std::endl;
    fetch_decode_fifo.print("\t");
    std::cout << std::endl;
    std::cout << "DECODE -> RENAME:" << std::endl;
    decode_rename_fifo.print("\t");
    std::cout << std::endl;
    std::cout << "RENAME:" << std::endl;
    std::cout << "\tstall(from issue stage) = " << outbool(t_issue_feedback_pack.stall) << std::endl;
    std::cout << std::endl;
    std::cout << "RENAME -> READREG:" << std::endl;
    rename_readreg_port.print("\t");
    std::cout << std::endl;
    std::cout << "READREG:" << std::endl;
    std::cout << "\tstall(from issue stage) = " << outbool(t_issue_feedback_pack.stall) << std::endl;
    std::cout << std::endl;
    std::cout << "READREG -> ISSUE:" << std::endl;
    readreg_issue_port.print("\t");
    std::cout << std::endl;
    std::cout << "ISSUE:" << std::endl;
    issue_stage.print("\t");
    std::cout << std::endl;
    std::cout << "ISSUE -> EXECUTE:" << std::endl;

    for(auto i = 0;i < ALU_UNIT_NUM;i++)
    {
        std::cout << "\tALU_UNIT[" << i << "]:" << std::endl;
        issue_alu_fifo[i] -> print("\t\t");
    }

    std::cout << std::endl;

    for(auto i = 0;i < BRU_UNIT_NUM;i++)
    {
        std::cout << "\tBRU_UNIT[" << i << "]:" << std::endl;
        issue_bru_fifo[i] -> print("\t\t");
    }

    std::cout << std::endl;

    for(auto i = 0;i < CSR_UNIT_NUM;i++)
    {
        std::cout << "\tCSR_UNIT[" << i << "]:" << std::endl;
        issue_csr_fifo[i] -> print("\t\t");
    }

    std::cout << std::endl;

    for(auto i = 0;i < DIV_UNIT_NUM;i++)
    {
        std::cout << "\tDIV_UNIT[" << i << "]:" << std::endl;
        issue_div_fifo[i] -> print("\t\t");
    }

    std::cout << std::endl;

    for(auto i = 0;i < LSU_UNIT_NUM;i++)
    {
        std::cout << "\tLSU_UNIT[" << i << "]:" << std::endl;
        issue_lsu_fifo[i] -> print("\t\t");
    }

    std::cout << std::endl;

    for(auto i = 0;i < MUL_UNIT_NUM;i++)
    {
        std::cout << "\tMUL_UNIT[" << i << "]:" << std::endl;
        issue_mul_fifo[i] -> print("\t\t");
    }
   
    std::cout << std::endl;
    std::cout << "EXECUTE->WB:" << std::endl;

    for(auto i = 0;i < ALU_UNIT_NUM;i++)
    {
        std::cout << "\tALU_UNIT[" << i << "]:" << std::endl;
        alu_wb_port[i] -> print("\t\t");
    }

    std::cout << std::endl;

    for(auto i = 0;i < BRU_UNIT_NUM;i++)
    {
        std::cout << "\tBRU_UNIT[" << i << "]:" << std::endl;
        bru_wb_port[i] -> print("\t\t");
    }

    std::cout << std::endl;

    for(auto i = 0;i < CSR_UNIT_NUM;i++)
    {
        std::cout << "\tCSR_UNIT[" << i << "]:" << std::endl;
        csr_wb_port[i] -> print("\t\t");
    }

    std::cout << std::endl;

    for(auto i = 0;i < DIV_UNIT_NUM;i++)
    {
        std::cout << "\tDIV_UNIT[" << i << "]:" << std::endl;
        div_wb_port[i] -> print("\t\t");
    }

    std::cout << std::endl;

    for(auto i = 0;i < LSU_UNIT_NUM;i++)
    {
        std::cout << "\tLSU_UNIT[" << i << "]:" << std::endl;
        lsu_wb_port[i] -> print("\t\t");
    }

    std::cout << std::endl;

    for(auto i = 0;i < MUL_UNIT_NUM;i++)
    {
        std::cout << "\tMUL_UNIT[" << i << "]:" << std::endl;
        mul_wb_port[i] -> print("\t\t");
    }
   
    std::cout << std::endl;
}

typedef void (*cmd_handler)();

typedef struct cmd_desc_t
{
    std::string cmd_name;
    cmd_handler handler;
}cmd_desc_t;

static cmd_desc_t cmd_list[] = {
                               {"q", cmd_quit},
                               {"c", cmd_continue},
                               {"s", cmd_step},
                               {"p", cmd_print},
                               {"", NULL}
                               };

static bool cmd_handle(std::string cmd)
{
    if(cmd == "")
    {
        return true;
    }

    for(auto i = 0;;i++)
    {
        if(cmd_list[i].handler == NULL)
        {
            break;
        }

        if(cmd_list[i].cmd_name == cmd)
        {
            cmd_list[i].handler();
            return true;
        }
    }

    return false;
}

static void run()
{
    init();

    std::string last_cmd = "";

    step_state = true;

    while(1)
    {
        if(ctrl_c_detected || step_state)
        {
            pause_state = true;

            while(pause_state)
            {
                std::cout << "Cycle" << cpu_clock_cycle << ":Command>";
                std::string cmd;
                std::getline(std::cin, cmd);

                if(std::cin.fail() || std::cin.eof())
                {
                    std::cin.clear();
                    std::cout << std::endl;
                }

                if(cmd == "")
                {
                    cmd = last_cmd;
                }
                
                last_cmd = cmd;

                if(!cmd_handle(cmd))
                {
                    std::cout << "Invalid Command!" << std::endl;
                }
            }

            ctrl_c_detected = false;
        }

        for(auto i = 0;i < ALU_UNIT_NUM;i++)
        {
            execute_alu_stage[i]->run();
        }

        t_issue_feedback_pack = issue_stage.run();
        readreg_stage.run(t_issue_feedback_pack);
        rename_stage.run(t_issue_feedback_pack);
        decode_stage.run();
        fetch_stage.run(t_bru_feedback_pack);
        rat.sync();
        rob.sync();
        phy_regfile.sync();
        cpu_clock_cycle++;
    }
}

#ifdef WIN32
    static BOOL WINAPI console_handler(DWORD dwCtrlType)
    {
        if(dwCtrlType == CTRL_C_EVENT)
        {
            ctrl_c_detected = true;
            return TRUE;
        }

        return FALSE;
    }
#endif

int main()
{
    test::test();

    #ifdef WIN32
        if(!SetConsoleCtrlHandler(console_handler, TRUE))
        {
            std::cout << "SetControlCtrlHandler Failed!" << std::endl;
        }
    #endif

    run();
    return 0;
}
