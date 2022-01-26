#pragma once
#include "common.h"

namespace component
{
    class memory
    {
        private:
            uint8_t *mem;
            uint32_t base;
            uint32_t size;
            bool test_mode;
            bool has_error;

        private:
            bool check(uint32_t addr, uint32_t access_size)
            {
                if(!test_mode)
                {
                    assert(!(addr & (access_size - 1)));//align check
                    assert(addr >= base);//boundary check
                    assert((addr - base) < size);//boundary check
                    assert((size - (addr - base)) >= access_size);//boundary check
                }
                else
                {
                    if(addr & (access_size - 1))
                    {
                        has_error = true;
                    }
                    else if(!(addr >= base))
                    {
                        has_error = true;
                    }
                    else if(!((addr - base) < size))
                    {
                        has_error = true;
                    }
                    else if(!((size - (addr - base)) >= access_size))
                    {
                        has_error = true;
                    }
                }

                return !has_error;
            }

        public:
            memory(uint32_t base, uint32_t size)
            {
                mem = new uint8_t[size]();
                this -> base = base;
                this -> size = size;
                test_mode = false;
                has_error = false;
            }

            ~memory()
            {
                delete[] mem;
            }

            void entry_test_mode()
            {
                test_mode = true;
                has_error = false;
            }

            bool get_error()
            {
                return has_error;
            }

            void clear_error()
            {
                has_error = false;
            }

            void write8(uint32_t addr, uint8_t value)
            {
                if(check(addr, 1))
                {
                    mem[addr - this->base] = value;
                }
            }

            void write16(uint32_t addr, uint16_t value)
            {
                if(check(addr, 2))
                {
                    *(uint16_t *)(mem + (addr - this->base)) = value;
                }
            }

            void write32(uint32_t addr, uint32_t value)
            {
                if(check(addr, 4))
                {
                    *(uint32_t *)(mem + (addr - this->base)) = value;
                }
            }

            uint8_t read8(uint32_t addr)
            {
                if(check(addr, 1))
                {
                    return mem[addr - this->base];
                }

                return 0;
            }

            uint16_t read16(uint32_t addr)
            {
                if(check(addr, 2))
                {
                    return *(uint16_t *)(mem + (addr - this->base));
                }

                return 0;
            }

            uint32_t read32(uint32_t addr)
            {
                if(check(addr, 4))
                {
                    return *(uint32_t *)(mem + (addr - this->base));
                }

                return 0;
            }
    };
}