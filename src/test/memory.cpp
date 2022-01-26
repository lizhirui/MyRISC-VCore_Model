#include "common.h"
#include "memory.h"
#include "../component/memory.h"

namespace test
{
    namespace memory
    {
        void test()
        {
            component::memory u_pack(0x80000000, 16);

            u_pack.entry_test_mode();
            assert(!u_pack.get_error());
            u_pack.write8(0x80000000, 0x55);
            assert(!u_pack.get_error());
            u_pack.write16(0x80000000 + 2, 0xABCD);
            assert(!u_pack.get_error());
            u_pack.write32(0x80000000 + 4, 0x01234567);
            assert(!u_pack.get_error());
            assert(u_pack.read8(0x80000000) == 0x55);
            assert(!u_pack.get_error());
            assert(u_pack.read16(0x80000000 + 2) == 0xABCD);
            assert(!u_pack.get_error());
            assert(u_pack.read32(0x80000000 + 4) == 0x01234567);
            assert(!u_pack.get_error());
            u_pack.write16(0x80000000 + 1, 0x1234);
            assert(u_pack.get_error());
            u_pack.clear_error();
            u_pack.write32(0x80000000 + 2, 0x12345678);
            assert(u_pack.get_error());
            u_pack.clear_error();
            u_pack.read16(0x80000000 + 1);
            assert(u_pack.get_error());
            u_pack.clear_error();
            u_pack.read32(0x80000000 + 2);
            assert(u_pack.get_error());
            u_pack.clear_error();
            u_pack.read32(0);
            assert(u_pack.get_error());
            u_pack.clear_error();
            u_pack.read8(0x7fffffff);
            assert(u_pack.get_error());
            u_pack.clear_error();
            u_pack.read16(0x7ffffffe);
            assert(u_pack.get_error());
            u_pack.clear_error();
            u_pack.read32(0x7ffffffc);
            assert(u_pack.get_error());
            u_pack.clear_error();
        }
    }
}