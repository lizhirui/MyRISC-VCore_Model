#pragma once
#include "common.h"

namespace component
{
    template<typename T>
    class regfile
    {
        private:
            enum class sync_request_type
            {
                write
            };

            typedef struct sync_request
            {
                sync_request_type req;
                uint32_t arg1;
                T arg2;
            }sync_request;

            std::queue<sync_request> sync_request_q;
            
            T *reg_data;
            uint32_t size;

        public:
            regfile(uint32_t size)
            {
                this->size = size;
                reg_data = new T[size];
                memset(reg_data, 0, sizeof(reg_data[0]) * size);
            }

            ~regfile()
            {
                delete[] reg_data;
            }

            void write(uint32_t addr, T value)
            {
                assert(addr < size);
                reg_data[addr] = value;
            }

            T read(uint32_t addr)
            {
                assert(addr < size);
                return reg_data[addr];
            }

            void write_sync(uint32_t addr, T value)
            {
                sync_request t_req;

                t_req.req = sync_request_type::write;
                t_req.arg1 = addr;
                t_req.arg2 = value;
                sync_request_q.push(t_req);
            }

            void sync()
            {
                sync_request t_req;

                while(!sync_request_q.empty())
                {
                    t_req = sync_request_q.front();
                    sync_request_q.pop();

                    switch(t_req.req)
                    {
                        case sync_request_type::write:
                            write(t_req.arg1, t_req.arg2);
                            break;
                    }
                }
            }
    };
}