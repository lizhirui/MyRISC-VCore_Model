#pragma once
#include "common.h"
#include "../slave_base.h"

namespace component
{
    namespace slave
    {
        class memory : public slave_base
        {
            private:
                uint8_t *mem;

                virtual void _init()
                {
                    mem = new uint8_t[this->size]();
                }

            public:
                ~memory()
                {
                    delete[] mem;
                }

                virtual void _write8(uint32_t addr, uint8_t value)
                {
                    mem[addr] = value;
                }

                virtual void _write16(uint32_t addr, uint16_t value)
                {
                    *(uint16_t *)(mem + addr) = value;
                }

                virtual void _write32(uint32_t addr, uint32_t value)
                {
                    *(uint32_t *)(mem + addr) = value;
                }

                virtual uint8_t _read8(uint32_t addr)
                {
                    return mem[addr];
                }

                virtual uint16_t _read16(uint32_t addr)
                {
                    return *(uint16_t *)(mem + addr);
                }

                virtual uint32_t _read32(uint32_t addr)
                {
                    return *(uint32_t *)(mem + addr);
                }
        };
    }
}