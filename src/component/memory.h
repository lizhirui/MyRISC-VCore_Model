#pragma once
#include "common.h"

namespace component
{
    class memory : public if_reset_t
    {
        private:
            uint8_t *mem;
            uint32_t base;
            uint32_t size;
            bool test_mode;
            bool has_error;

            enum class sync_request_type_t
            {
                write8,
                write16,
                write32
            };

            typedef struct sync_request_t
            {
                sync_request_type_t req;
                uint32_t arg1;
                union
                {
                    uint8_t u8;
                    uint16_t u16;
                    uint32_t u32;
                }arg2;
            }sync_request_t;

            std::queue<sync_request_t> sync_request_q;

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

            virtual void reset()
            {
                clear_queue(sync_request_q);
            }

            bool check_align(uint32_t addr, uint32_t access_size)
            {
                return !(addr & (access_size - 1));
            }

            bool check_boundary(uint32_t addr, uint32_t access_size)
            {
                return (addr >= base) && ((addr - base) < size) && ((size - (addr - base)) >= access_size);
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

            void write8_sync(uint32_t addr, uint8_t value)
            {
                sync_request_t t_req;

                t_req.req = sync_request_type_t::write8;
                t_req.arg1 = addr;
                t_req.arg2.u8 = value;
                sync_request_q.push(t_req);
            }

            void write16(uint32_t addr, uint16_t value)
            {
                if(check(addr, 2))
                {
                    *(uint16_t *)(mem + (addr - this->base)) = value;
                }
            }

            void write16_sync(uint32_t addr, uint16_t value)
            {
                sync_request_t t_req;

                t_req.req = sync_request_type_t::write16;
                t_req.arg1 = addr;
                t_req.arg2.u16 = value;
                sync_request_q.push(t_req);
            }

            void write32(uint32_t addr, uint32_t value)
            {
                if(check(addr, 4))
                {
                    *(uint32_t *)(mem + (addr - this->base)) = value;
                }
            }

            void write32_sync(uint32_t addr, uint32_t value)
            {
                sync_request_t t_req;

                t_req.req = sync_request_type_t::write32;
                t_req.arg1 = addr;
                t_req.arg2.u32 = value;
                sync_request_q.push(t_req);
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

            void sync()
            {
                sync_request_t t_req;

                while(!sync_request_q.empty())
                {
                    t_req = sync_request_q.front();
                    sync_request_q.pop();

                    switch(t_req.req)
                    {
                        case sync_request_type_t::write8:
                            write8(t_req.arg1, t_req.arg2.u8);
                            break;

                        case sync_request_type_t::write16:
                            write16(t_req.arg1, t_req.arg2.u16);
                            break;

                        case sync_request_type_t::write32:
                            write32(t_req.arg1, t_req.arg2.u32);
                            break;
                    }
                }
            }
    };
}