#pragma once
#include "common.h"
#include "checkpoint_buffer.h"

namespace component
{
    template<typename T>
    class regfile : public if_reset_t
    {
        private:
            enum class sync_request_type_t
            {
                write,
                restore
            };

            typedef struct sync_request_t
            {
                sync_request_type_t req;
                uint32_t arg1;
                T arg2;
                bool arg3;
                checkpoint_t cp;
            }sync_request_t;

            std::queue<sync_request_t> sync_request_q;
            
            T *reg_data;
            uint64_t *reg_data_valid;
            uint32_t size;
            uint32_t bitmap_size;

        public:
            regfile(uint32_t size)
            {
                this->size = size;
                reg_data = new T[size];
                bitmap_size = (size + bitsizeof(reg_data_valid[0]) - 1) / (bitsizeof(reg_data_valid[0]));
                reg_data_valid = new uint64_t[bitmap_size];
                memset(reg_data, 0, sizeof(reg_data[0]) * size);
                memset(reg_data_valid, 0, sizeof(reg_data_valid) * bitmap_size);
            }

            ~regfile()
            {
                delete[] reg_data;
                delete[] reg_data_valid;
            }

            virtual void reset()
            {
                memset(reg_data, 0, sizeof(reg_data[0]) * size);
                memset(reg_data_valid, 0, sizeof(reg_data_valid) * bitmap_size);
                clear_queue(sync_request_q);
            }

            void write(uint32_t addr, T value, bool valid)
            {
                assert(addr < size);
                reg_data[addr] = value;

                if(valid)
                {
                    reg_data_valid[addr / bitsizeof(reg_data_valid[0])] |= 1ULL << (addr % bitsizeof(reg_data_valid[0]));
                }
                else
                {
                    reg_data_valid[addr / bitsizeof(reg_data_valid[0])] &= ~(1ULL << (addr % bitsizeof(reg_data_valid[0])));
                }
            }

            T read(uint32_t addr)
            {
                assert(addr < size);
                return reg_data[addr];
            }

            bool cp_get_data_valid(checkpoint_t &cp, uint32_t addr)
            {
                assert(addr < size);
                return (cp.phy_regfile_data_valid[addr / bitsizeof(cp.phy_regfile_data_valid[0])] & (1ULL << (addr % bitsizeof(cp.phy_regfile_data_valid[0])))) != 0;
            }

            void cp_set_data_valid(checkpoint_t &cp, uint32_t addr, bool valid)
            {
                assert(addr < size);

                if(valid)
                {
                    cp.phy_regfile_data_valid[addr / bitsizeof(cp.phy_regfile_data_valid[0])] |= 1ULL << (addr % bitsizeof(cp.phy_regfile_data_valid[0]));
                }
                else
                {
                    cp.phy_regfile_data_valid[addr / bitsizeof(cp.phy_regfile_data_valid[0])] &= ~(1ULL << (addr % bitsizeof(cp.phy_regfile_data_valid[0])));
                }
            }

            bool read_data_valid(uint32_t addr)
            {
                assert(addr < size);
                return ((reg_data_valid[addr / bitsizeof(reg_data_valid[0])]) & (1ULL << (addr % bitsizeof(reg_data_valid[0])))) != 0;
            }

            void write_sync(uint32_t addr, T value, bool valid)
            {
                sync_request_t t_req;

                t_req.req = sync_request_type_t::write;
                t_req.arg1 = addr;
                t_req.arg2 = value;
                t_req.arg3 = valid;
                sync_request_q.push(t_req);
            }

            /*void save(checkpoint_t &cp)
            {
                memcpy(cp.phy_regfile_data_valid, reg_data_valid, sizeof(cp.phy_regfile_data_valid));
            }*/

            void restore(checkpoint_t &cp)
            {
                memcpy(reg_data_valid, cp.phy_regfile_data_valid, sizeof(cp.phy_regfile_data_valid));
            }

            void restore_sync(checkpoint_t &cp)
            {
                sync_request_t t_req;

                t_req.req = sync_request_type_t::restore;
                t_req.cp = cp;
                sync_request_q.push(t_req);
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
                        case sync_request_type_t::write:
                            write(t_req.arg1, t_req.arg2, t_req.arg3);
                            break;

                        case sync_request_type_t::restore:
                            restore(t_req.cp);
                            break;
                    }
                }
            }
    };
}