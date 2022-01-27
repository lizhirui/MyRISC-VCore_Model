#include "common.h"
#include "test.h"
#include "fifo.h"
#include "port.h"
#include "memory.h"
#include "rat.h"
#include "regfile.h"
#include "issue_queue.h"

namespace test
{
    void test()
    {
        fifo::test();
        port::test();
        memory::test();
        rat::test();
        regfile::test();
        issue_queue::test();
        std::cout << "Self-Test Passed" << std::endl;
    }
}
