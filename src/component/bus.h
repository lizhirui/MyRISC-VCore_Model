#pragma once
#include "common.h"
#include "slave_base.h"

namespace component
{
    typedef struct slave_info_t
    {
        uint32_t base;
        uint32_t size;
        std::shared_ptr<slave_base> slave;
    }slave_info_t;

    class bus : public if_reset_t
    {
        private:
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
            std::vector<slave_info_t> slave_info_list;

            int find_slave_info(uint32_t addr)
            {
                for(auto i = 0;i < slave_info_list.size();i++)
                {
                    if((addr >= slave_info_list[i].base) && (addr < (slave_info_list[i].base + slave_info_list[i].size)))
                    {
                        return i;
                    }
                }

                return -1;
            }

            bool check_addr_override(uint32_t base, uint32_t size)
            {
                for(auto i = 0;i < slave_info_list.size();i++)
                {
                    if(((base >= slave_info_list[i].base) && (base < (slave_info_list[i].base + slave_info_list[i].size))) || ((base < slave_info_list[i].base) && ((base + size) > slave_info_list[i].base)))
                    {
                        return true;
                    }
                }

                return false;
            }

        public:
            void map(uint32_t base, uint32_t size, std::shared_ptr<slave_base> slave)
            {
                assert(!check_addr_override(base, size));
                slave_info_t slave_info;
                slave_info.base = base;
                slave_info.size = size;
                slave_info.slave = slave;
                slave->init(size);
                slave_info_list.push_back(slave_info);
            }

            virtual void reset()
            {
                clear_queue(sync_request_q);
            }

            bool check_align(uint32_t addr, uint32_t access_size)
            {
                return !(addr & (access_size - 1));
            }
            
            void write8(uint32_t addr, uint8_t value)
            {
                if(auto slave_index = find_slave_info(addr);slave_index >= 0)
                {
                    slave_info_list[slave_index].slave->write8(addr - slave_info_list[slave_index].base, value);
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
                if(auto slave_index = find_slave_info(addr);slave_index >= 0)
                {
                    slave_info_list[slave_index].slave->write16(addr - slave_info_list[slave_index].base, value);
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
                if(auto slave_index = find_slave_info(addr);slave_index >= 0)
                {
                    slave_info_list[slave_index].slave->write32(addr - slave_info_list[slave_index].base, value);
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
                if(auto slave_index = find_slave_info(addr);slave_index >= 0)
                {
                    return slave_info_list[slave_index].slave->read8(addr - slave_info_list[slave_index].base);
                }
                
                return 0;
            }

            uint16_t read16(uint32_t addr)
            {
                if(auto slave_index = find_slave_info(addr);slave_index >= 0)
                {
                    return slave_info_list[slave_index].slave->read16(addr - slave_info_list[slave_index].base);
                }
                
                return 0;
            }

            uint32_t read32(uint32_t addr)
            {
                if(auto slave_index = find_slave_info(addr);slave_index >= 0)
                {
                    return slave_info_list[slave_index].slave->read32(addr - slave_info_list[slave_index].base);
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