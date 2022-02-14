#include "common.h"
#include "config.h"
#include "../test/test.h"
#include "../component/fifo.h"
#include "../component/port.h"
#include "../component/memory.h"
#include "../component/regfile.h"
#include "../component/csrfile.h"
#include "../component/csr_all.h"
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
#include "../pipeline/wb.h"
#include "../pipeline/wb_commit.h"
#include "../pipeline/commit.h"
#include "asio.hpp"
#include <thread>

#ifdef WIN32
    #include <windows.h>
#endif

static std::atomic<bool> ctrl_c_detected = false;

static uint64_t cpu_clock_cycle = 0;

static component::fifo<pipeline::fetch_decode_pack_t> fetch_decode_fifo(16);
static component::fifo<pipeline::decode_rename_pack_t> decode_rename_fifo(16);
static pipeline::rename_readreg_pack_t default_rename_readreg_pack;
static pipeline::readreg_issue_pack_t default_readreg_issue_pack;
static pipeline::execute_wb_pack_t default_execute_wb_pack;
static pipeline::wb_commit_pack_t default_wb_commit_pack;

static component::port<pipeline::rename_readreg_pack_t> rename_readreg_port(default_rename_readreg_pack);
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

static component::port<pipeline::wb_commit_pack_t> wb_commit_port(default_wb_commit_pack);

static component::memory memory(0x80000000, 64 * 0x1000);
static component::rat rat(PHY_REG_NUM, ARCH_REG_NUM);
static component::rob rob(16);
static component::regfile<pipeline::phy_regfile_item_t> phy_regfile(PHY_REG_NUM);
static component::csrfile csr_file;

static pipeline::fetch fetch_stage(&memory, &fetch_decode_fifo, 0x80000000);
static pipeline::decode decode_stage(&fetch_decode_fifo, &decode_rename_fifo);
static pipeline::rename rename_stage(&decode_rename_fifo, &rename_readreg_port, &rat, &rob);
static pipeline::readreg readreg_stage(&rename_readreg_port, &readreg_issue_port, &phy_regfile);
static pipeline::issue issue_stage(&readreg_issue_port, issue_alu_fifo, issue_bru_fifo, issue_csr_fifo, issue_div_fifo, issue_lsu_fifo, issue_mul_fifo);
static pipeline::execute::alu *execute_alu_stage[ALU_UNIT_NUM];
static pipeline::execute::bru *execute_bru_stage[BRU_UNIT_NUM];
static pipeline::execute::csr *execute_csr_stage[CSR_UNIT_NUM];
static pipeline::execute::div *execute_div_stage[DIV_UNIT_NUM];
static pipeline::execute::lsu *execute_lsu_stage[LSU_UNIT_NUM];
static pipeline::execute::mul *execute_mul_stage[MUL_UNIT_NUM];
static pipeline::wb wb_stage(alu_wb_port, bru_wb_port, csr_wb_port, div_wb_port, lsu_wb_port, mul_wb_port, &wb_commit_port, &phy_regfile);
static pipeline::commit commit_stage(&wb_commit_port, &rat, &rob, &csr_file, &phy_regfile);

static pipeline::issue_feedback_pack_t t_issue_feedback_pack;
static pipeline::execute::bru_feedback_pack_t t_bru_feedback_pack;
static pipeline::wb_feedback_pack_t t_wb_feedback_pack;
static pipeline::commit_feedback_pack_t t_commit_feedback_pack;

static void init()
{
    std::ifstream binfile("../../../testprgenv/main/test.bin", std::ios::binary);

    if(!binfile || !binfile.is_open())
    {
        std::cout << "test.bin Open Failed!" << std::endl;
        exit(1);
    }

    char buf[4096];
    size_t n = 0;

    while(!binfile.eof())
    {
        binfile.read((char *)&buf, sizeof(buf));

        for(auto i = 0;i < sizeof(buf);i++)
        {
            memory.write8(0x80000000 + i, buf[i]);
        }

        n += sizeof(buf);
    }

    std::cout << "test.bin Read OK!Bytes(" << sizeof(buf) << "Byte-Block):" << n << std::endl;

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

    for(auto i = 0;i < BRU_UNIT_NUM;i++)
    {
        execute_bru_stage[i] = new pipeline::execute::bru(issue_bru_fifo[i], bru_wb_port[i], &csr_file);
    }

    for(auto i = 0;i < CSR_UNIT_NUM;i++)
    {
        execute_csr_stage[i] = new pipeline::execute::csr(issue_csr_fifo[i], csr_wb_port[i], &csr_file);
    }

    for(auto i = 0;i < DIV_UNIT_NUM;i++)
    {
        execute_div_stage[i] = new pipeline::execute::div(issue_div_fifo[i], div_wb_port[i]);
    }

    for(auto i = 0;i < LSU_UNIT_NUM;i++)
    {
        execute_lsu_stage[i] = new pipeline::execute::lsu(issue_lsu_fifo[i], lsu_wb_port[i], &memory);
    }

    for(auto i = 0;i < MUL_UNIT_NUM;i++)
    {
        execute_mul_stage[i] = new pipeline::execute::mul(issue_mul_fifo[i], mul_wb_port[i]);
    }

    std::memset(&t_issue_feedback_pack, 0, sizeof(t_issue_feedback_pack));
    std::memset(&t_bru_feedback_pack, 0, sizeof(t_bru_feedback_pack));
    std::memset(&t_wb_feedback_pack, 0, sizeof(t_wb_feedback_pack));
    std::memset(&t_commit_feedback_pack, 0, sizeof(t_commit_feedback_pack));

    rat.init_start();

    for(uint32_t i = 1;i < 32;i++)
    {
        rat.set_map(i, i);
        rat.commit_map(i);
        pipeline::phy_regfile_item_t t_item;
        t_item.value = 0;
        t_item.valid = true;
        phy_regfile.write(i, t_item);
    }

    rat.init_finish();

    csr_file.map(CSR_MVENDORID, true, std::make_shared<component::csr::mvendorid>());
    csr_file.map(CSR_MARCHID, true, std::make_shared<component::csr::marchid>());
    csr_file.map(CSR_MIMPID, true, std::make_shared<component::csr::mimpid>());
    csr_file.map(CSR_MHARTID, true, std::make_shared<component::csr::mhartid>());
    csr_file.map(CSR_MCONFIGPTR, true, std::make_shared<component::csr::mconfigptr>());
    csr_file.map(CSR_MSTATUS, false, std::make_shared<component::csr::mstatus>());
    csr_file.map(CSR_MISA, false, std::make_shared<component::csr::misa>());
    csr_file.map(CSR_MIE, false, std::make_shared<component::csr::mie>());
    csr_file.map(CSR_MTVEC, false, std::make_shared<component::csr::mtvec>());
    csr_file.map(CSR_MCOUNTEREN, false, std::make_shared<component::csr::mcounteren>());
    csr_file.map(CSR_MSTATUSH, false, std::make_shared<component::csr::mstatush>());
    csr_file.map(CSR_MSCRATCH, false, std::make_shared<component::csr::mscratch>());
    csr_file.map(CSR_MEPC, false, std::make_shared<component::csr::mepc>());
    csr_file.map(CSR_MCAUSE, false, std::make_shared<component::csr::mcause>());
    csr_file.map(CSR_MTVAL, false, std::make_shared<component::csr::mtval>());
    csr_file.map(CSR_MIP, false, std::make_shared<component::csr::mip>());

    for(auto i = 0;i < 16;i++)
    {
        csr_file.map(0x3A0 + i, false, std::make_shared<component::csr::pmpcfg>(i));
    }

    for(auto i = 0;i < 64;i++)
    {
        csr_file.map(0x3B0 + i, false, std::make_shared<component::csr::pmpaddr>(i));
    }

    wb_stage.init();
}

static bool pause_state = false;
static bool step_state = false;
static bool wait_commit = false;
static bool gui_mode = false;

static void cmd_quit()
{
    exit(0);
}

static void cmd_continue()
{
    pause_state = false;
    step_state = false;
    wait_commit = false;
}

static void cmd_step()
{
    pause_state = false;
    step_state = true;
    wait_commit = false;
}

static void cmd_stepcommit()
{
    pause_state = false;
    step_state = true;
    wait_commit = true;
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
    std::cout << "WB->COMMIT:" << std::endl;
    wb_commit_port.print("\t");
    std::cout << std::endl;
}

static void cmd_rat()
{
    rat.print("");
    std::cout << std::endl;
}

static void cmd_rob()
{
    rob.print("");
    std::cout << std::endl;
}

static void cmd_csr()
{
    csr_file.print("");
    std::cout << std::endl;
}

static void cmd_arch()
{
    std::cout << "Archtecture Registers:" << std::endl;

    for(auto i = 0;i < ARCH_REG_NUM;i++)
    {
        if(i == 0)
        {
            std::cout << "x0 = 0x" << fillzero(8) << outhex(0) << "(0)" << std::endl;
        }
        else
        {
            uint32_t phy_id;
            rat.get_commit_phy_id(i, &phy_id);
            auto v = phy_regfile.read(phy_id);
            assert(v.valid);
            std::cout << "x" << i << " = 0x" << fillzero(8) << outhex(v.value) << "(" << v.value << ") -> " << phy_id << std::endl;
        }
    }

    std::cout << std::endl;
}

static uint32_t get_current_pc()
{
    if(!rob.is_empty())
    {
        return rob.get_front().pc;
    }

    pipeline::decode_rename_pack_t drpack;

    if(decode_rename_fifo.get_front(&drpack) && (drpack.op_info[0].enable || drpack.op_info[1].enable))
    {
        return drpack.op_info[0].enable ? drpack.op_info[0].pc : drpack.op_info[1].pc;
    }

    pipeline::fetch_decode_pack_t fdpack;

    if(fetch_decode_fifo.get_front(&fdpack) && (fdpack.op_info[0].enable || fdpack.op_info[1].enable))
    {
        return fdpack.op_info[0].enable ? fdpack.op_info[0].pc : fdpack.op_info[1].pc;
    }

    return fetch_stage.get_pc();
}

static asio::io_context tcp_server_thread_ioc;
void tcp_server_thread_entry(asio::ip::tcp::acceptor &&listener);

static void cmd_gui()
{
    try
    {
        asio::ip::tcp::acceptor listener(tcp_server_thread_ioc, {asio::ip::address::from_string("127.0.0.1"), 10240});
        listener.set_option(asio::ip::tcp::no_delay(true));
        listener.listen();
        std::cout << "Server Bind on 127.0.0.1:10240" << std::endl;
        std::thread server_thread(tcp_server_thread_entry, std::move(listener));
        server_thread.detach();
        gui_mode = true;
    }
    catch(const std::exception &e)
    {
        std::cout << "Server Bind Port Failed:" << e.what() << std::endl;
    }
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
                               {"sc", cmd_stepcommit},
                               {"p", cmd_print},
                               {"rat", cmd_rat},
                               {"rob", cmd_rob},
                               {"csr", cmd_csr},
                               {"arch", cmd_arch},
                               {"gui", cmd_gui},
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

static asio::io_context recv_ioc;
static asio::io_context send_ioc;
std::atomic<bool> recv_thread_stop = false;
std::atomic<bool> recv_thread_stopped = false;
std::atomic<bool> server_thread_stopped = false;
std::atomic<bool> program_stop = false;

static void run()
{
    init();

    std::string last_cmd = "";

    step_state = true;

    while(1)
    {
        if(ctrl_c_detected || (step_state && ((!wait_commit) || (wait_commit && rob.get_committed()))))
        {
            pause_state = true;

            if(gui_mode)
            {
                std::cout << "Wait GUI Command" << std::endl;
            }

            while(pause_state)
            {
                if(!gui_mode)
                {
                    std::cout << "Cycle" << cpu_clock_cycle << ",0x" << fillzero(8) << outhex(get_current_pc()) << ":Command>";
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
                else
                {
                    try
                    {
                        recv_ioc.run_one();
                    }
                    catch(const std::exception &ex)
                    {
                        std::cout << ex.what() << std::endl;
                    }

                    if(program_stop)
                    {
                        break;
                    }
                }
            }

            if(program_stop)
            {
                break;
            }

            ctrl_c_detected = false;
        }

        rob.set_committed(false);
        t_commit_feedback_pack = commit_stage.run();
        t_wb_feedback_pack = wb_stage.run(t_commit_feedback_pack);

        for(auto i = 0;i < ALU_UNIT_NUM;i++)
        {
            execute_alu_stage[i]->run(t_commit_feedback_pack);
        }

        for(auto i = 0;i < BRU_UNIT_NUM;i++)
        {
            t_bru_feedback_pack = execute_bru_stage[i]->run(t_commit_feedback_pack);
        }

        for(auto i = 0;i < CSR_UNIT_NUM;i++)
        {
            execute_csr_stage[i]->run(t_commit_feedback_pack);
        }

        for(auto i = 0;i < DIV_UNIT_NUM;i++)
        {
            execute_div_stage[i]->run(t_commit_feedback_pack);
        }

        for(auto i = 0;i < LSU_UNIT_NUM;i++)
        {
            execute_lsu_stage[i]->run(t_commit_feedback_pack);
        }

        for(auto i = 0;i < MUL_UNIT_NUM;i++)
        {
            execute_mul_stage[i]->run(t_commit_feedback_pack);
        }

        t_issue_feedback_pack = issue_stage.run(t_wb_feedback_pack, t_commit_feedback_pack);
        readreg_stage.run(t_issue_feedback_pack, t_wb_feedback_pack, t_commit_feedback_pack);
        rename_stage.run(t_issue_feedback_pack, t_commit_feedback_pack);
        decode_stage.run(t_commit_feedback_pack);
        fetch_stage.run(t_bru_feedback_pack, t_commit_feedback_pack);
        rat.sync();
        rob.sync();
        phy_regfile.sync();
        csr_file.sync();
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

static std::string socket_cmd_quit(std::vector<std::string> args)
{
    if(args.size() != 0)
    {
        return "argerror";
    }

    recv_thread_stop = true;
    program_stop = true;
    return "ok";
}

static std::string socket_cmd_continue(std::vector<std::string> args)
{
    if(args.size() != 0)
    {
        return "argerror";
    }

    pause_state = false;
    step_state = false;
    wait_commit = false;
    return "ok";
}

static std::string socket_cmd_pause(std::vector<std::string> args)
{
    if(args.size() != 0)
    {
        return "argerror";
    }

    return "ok";
}

static std::string socket_cmd_step(std::vector<std::string> args)
{
    if(args.size() != 0)
    {
        return "argerror";
    }

    pause_state = false;
    step_state = true;
    wait_commit = false;
    return "ok";
}

static std::string socket_cmd_stepcommit(std::vector<std::string> args)
{
    if(args.size() != 0)
    {
        return "argerror";
    }

    pause_state = false;
    step_state = true;
    wait_commit = true;
    return "ok";
}

static std::string socket_cmd_read_memory(std::vector<std::string> args)
{
    if(args.size() != 2)
    {
        return "argerror";
    }

    uint32_t address = 0;
    uint32_t size = 0;
    std::stringstream address_str(args[0]);
    std::stringstream size_str(args[1]);
    address_str.unsetf(std::ios::dec);
    address_str.setf(std::ios::hex);
    address_str >> address;
    size_str >> size;

    if(size > 0x1000)
    {
        return "sizetoobig";
    }

    std::stringstream result;

    result << std::hex << address;

    for(auto addr = address;addr < (address + size);addr++)
    {
        if(!memory.check_boundary(addr, 1))
        {
            break;
        }

        result << "," << std::hex << (uint32_t)memory.read8(addr);
    }

    return result.str();
}

static std::string socket_cmd_read_archreg(std::vector<std::string> args)
{
    if(args.size() != 0)
    {
        return "argerror";
    }

    std::stringstream result;

    for(auto i = 0;i < ARCH_REG_NUM;i++)
    {
        if(i == 0)
        {
            result << std::hex << 0;
        }
        else
        {
            uint32_t phy_id;
            rat.get_commit_phy_id(i, &phy_id);
            auto v = phy_regfile.read(phy_id);
            assert(v.valid);
            result << "," << std::hex << v.value;
        }
    }

    return result.str();
}

static std::string socket_cmd_read_csr(std::vector<std::string> args)
{
    if(args.size() != 0)
    {
        return "argerror";
    }

    return csr_file.get_info_packet();
}

static std::string socket_cmd_get_pc(std::vector<std::string> args)
{
    if(args.size() != 0)
    {
        return "argerror";
    }

    std::stringstream result;
    result << outhex(get_current_pc());
    return result.str();
}

static std::string socket_cmd_get_cycle(std::vector<std::string> args)
{
    if(args.size() != 0)
    {
        return "argerror";
    }

    std::stringstream result;
    result << cpu_clock_cycle;
    return result.str();
}

typedef std::string (*socket_cmd_handler)(std::vector<std::string> args);

typedef struct socket_cmd_desc_t
{
    std::string cmd_name;
    socket_cmd_handler handler;
}socket_cmd_desc_t;

static socket_cmd_desc_t socket_cmd_list[] = {
                                      {"quit", socket_cmd_quit},
                                      {"continue", socket_cmd_continue},
                                      {"pause", socket_cmd_pause},
                                      {"step", socket_cmd_step},
                                      {"stepcommit", socket_cmd_stepcommit},
                                      {"read_memory", socket_cmd_read_memory},
                                      {"read_archreg", socket_cmd_read_archreg},
                                      {"read_csr", socket_cmd_read_csr},
                                      {"get_pc", socket_cmd_get_pc},
                                      {"get_cycle", socket_cmd_get_cycle},
                                      {"", NULL}
                                      };

void send_cmd_result(asio::ip::tcp::socket &soc, std::string result)
{
    char *buffer = new char[result.length() + 4];
    *(uint32_t *)buffer = (uint32_t)result.length();
    memcpy(buffer + 4, result.data(), result.length());
    soc.send(asio::buffer(buffer, result.length() + 4));
    delete[] buffer;
}

void socket_cmd_handle(asio::ip::tcp::socket &soc, std::string rev_str)
{
    std::stringstream stream(rev_str);
    std::vector<std::string> cmd_arg_list;
    
    while(!stream.eof())
    {
        std::string t;
        stream >> t;
        cmd_arg_list.push_back(t);
    }

    if(cmd_arg_list.size() >= 2)
    {
        auto prefix = cmd_arg_list[0];
        auto cmd = cmd_arg_list[1];
        cmd_arg_list.erase(cmd_arg_list.begin());
        cmd_arg_list.erase(cmd_arg_list.begin());
        std::string ret = "notfound";

        for(auto i = 0;;i++)
        {
            if(socket_cmd_list[i].handler == NULL)
            {
                break;
            }

            if(socket_cmd_list[i].cmd_name == cmd)
            {
                ret = socket_cmd_list[i].handler(cmd_arg_list);
                break;
            }
        }

        asio::post(send_ioc, [&soc, prefix, cmd, ret]{send_cmd_result(soc, prefix + " " + cmd + " " + ret);});
    }
}

void tcp_server_thread_receive_entry(asio::ip::tcp::socket &soc)
{
    uint32_t length = 0;
    size_t rev_length = 0;
    char *packet_payload = nullptr;
    char packet_length[4];
    std::string rev_str = "";

    while(!recv_thread_stop)
    {
        try
        {
            if(packet_payload == nullptr)
            {
                rev_length += soc.receive(asio::buffer(packet_length + rev_length, sizeof(packet_length) - rev_length));
                auto finish = rev_length == sizeof(packet_length);

                if(finish)
                {
                    length = *(uint32_t *)packet_length;
                    rev_str.resize(length);
                    packet_payload = rev_str.data();
                    rev_length = 0;
                }
            }
            else
            {
                rev_length += soc.receive(asio::buffer(packet_payload + rev_length, length - rev_length));
                auto finish = rev_length == length;

                if(finish)
                {
                    packet_payload = nullptr;
                    rev_length = 0;

                    std::stringstream stream(rev_str);
                    std::vector<std::string> cmd_arg_list;
    
                    while(!stream.eof())
                    {
                        std::string t;
                        stream >> t;
                        cmd_arg_list.push_back(t);
                    }

                    if((cmd_arg_list.size() >= 2) && (cmd_arg_list[1] == std::string("pause")))
                    {
                        ctrl_c_detected = true;
                    }
                    
                    asio::post(recv_ioc, [&soc, rev_str]{socket_cmd_handle(soc, rev_str);});
                }
            }
        }
        catch(const std::exception &ex)
        {
            std::cout << ex.what() << std::endl;
            break;
        }
    }

    recv_thread_stopped = true;
}

void tcp_server_thread_entry(asio::ip::tcp::acceptor &&listener)
{
    try
    {
        while(!program_stop)
        {
            std::cout << "Wait GUI Connection" << std::endl;
            auto soc = listener.accept();
            soc.set_option(asio::ip::tcp::no_delay(true));
            std::cout << "GUI Connected" << std::endl;
            recv_thread_stop = false;
            recv_thread_stopped = false;
            std::thread rev_thread(tcp_server_thread_receive_entry, std::ref(soc));

            try
            {
                while(1)
                {
                    send_ioc.run_one_for(std::chrono::milliseconds(1000));

                    if(recv_thread_stopped)
                    {
                        rev_thread.join();
                        break;
                    }
                }
            }
            catch(const std::exception &ex)
            {
                std::cout << ex.what() << std::endl;
                recv_thread_stop = true;
                rev_thread.join();
            }
            
            soc.shutdown(asio::socket_base::shutdown_both);
            soc.close();
            std::cout << "GUI Disconnected" << std::endl;
        }
        
        listener.close();
        server_thread_stopped = true;
        /*#ifdef WIN32
            ExitProcess(0);
        #else
            kill(getpid(), SIGINT);
        #endif*/
    }
    catch(const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

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
    while(!server_thread_stopped);
    return 0;
}
