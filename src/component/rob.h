#pragma once
#include "common.h"
#include "fifo.h"

namespace component
{
    typedef struct rob_item
    {
        uint32_t old_phy_reg_id;
        bool finish;
    }rob_item;

    class rob : fifo<rob_item>
    {
        private:
            enum class sync_request_type
            {
                pop
            };

            typedef struct sync_request
            {
                sync_request_type req;
            }sync_request;

            std::queue<sync_request> sync_request_q;

        public:
            rob(uint32_t size) : fifo<rob_item>(size)
            {
                
            }

            bool push(rob_item element, uint32_t *item_id)
            {
                *item_id = this->wptr;
                return fifo<rob_item>::push(element);
            }

            rob_item get_item(uint32_t item_id)
            {
                return this->buffer[item_id];
            }

            void set_item(uint32_t item_id, rob_item item)
            {
                this->buffer[item_id] = item;
            }

            rob_item get_front()
            {
                return this->buffer[rptr];
            }

            bool pop()
            {
                rob_item t;
                return fifo<rob_item>::pop(&t);
            }

            void pop_sync()
            {
                sync_request t_req;

                t_req.req = sync_request_type::pop;
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
                        case sync_request_type::pop:
                            pop();
                            break;
                    }
                }
            }
    };
}