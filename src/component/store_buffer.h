#pragma once
#include "common.h"
#include "fifo.h"
#include "memory.h"
#include "../pipeline/commit.h"

namespace component
{
    typedef struct store_buffer_item_t
    {
        bool enable;
        bool committed;
        uint32_t rob_id;
        uint32_t pc;
        uint32_t addr;
        uint32_t data;
        uint32_t size;
    }store_buffer_item_t;

    typedef struct store_buffer_state_pack_t
    {
        uint32_t wptr;
        bool wstage;
    }store_buffer_state_pack_t;

    class store_buffer : public fifo<store_buffer_item_t>
    {
        private:
            enum class sync_request_type_t
            {
                set_item,
                push,
                pop,
                restore
            };
            
            typedef struct sync_request_t
            {
                sync_request_type_t req;
                
                union
                {
                    store_buffer_item_t store_buffer_item;
                    store_buffer_state_pack_t store_buffer_state_pack;
                }arg1;
            }sync_request_t;
            
            std::queue<sync_request_t> sync_request_q;

            memory *memory_if;

            bool check_id_valid(uint32_t id)
            {
                if(this->is_empty())
                {
                    return false;
                }
                else if(this->wstage == this->rstage)
                {
                    return (id >= this->rptr) && (id < this->wptr);
                }
                else
                {
                    return ((id >= this->rptr) && (id < this->size)) || (id < this->wptr);
                }
            }

            store_buffer_item_t get_item(uint32_t id)
            {
                assert(check_id_valid(id));
                return this->buffer[id];
            }
            
            void set_item(uint32_t id, store_buffer_item_t value)
            {
                assert(check_id_valid(id));
                this->buffer[id] = value;
            }

            bool get_front_id(uint32_t *front_id)
            {
                if(this->is_empty())
                {
                    return false;
                }
                
                *front_id = this->rptr;
                return true;
            }
            
            bool get_tail_id(uint32_t *tail_id)
            {
                if(this->is_empty())
                {
                    return false;
                }
                
                *tail_id = (wptr + this->size - 1) % this->size;
                return true;
            }
            
            bool get_next_id(uint32_t id, uint32_t *next_id)
            {
                assert(check_id_valid(id));
                *next_id = (id + 1) % this->size;
                return check_id_valid(*next_id);
            }
        
        public:
            store_buffer(uint32_t size, memory *memory_if) : fifo<store_buffer_item_t>(size)
            {
                this->memory_if = memory_if;
            }

            virtual void reset()
            {
                fifo<T>::reset();
                clear_queue(sync_request_q);
            }

            store_buffer_state_pack_t save()
            {
                store_buffer_state_pack_t pack;
                
                pack.wptr = wptr;
                pack.wstage = wstage;
                return pack;
            }

            void restore(store_buffer_state_pack_t pack)
            {
                wptr = pack.wptr;
                wstage = pack.wstage;
            }

            void restore_sync(store_buffer_state_pack_t pack)
            {
                sync_request_t item;

                item.req = sync_request_type_t::restore;
                item.arg1.store_buffer_state_pack = pack;
                sync_request_q.push(item);
            }
            
            void push_sync(store_buffer_item_t element)
            {
                sync_request_t item;

                item.req = sync_request_type_t::push;
                item.arg1.store_buffer_item = element;
                sync_request_q.push(item);
            }
            
            void pop_sync()
            {
                sync_request_t item;
                    
                item.req = sync_request_type_t::pop;
                sync_request_q.push(item);
            }

            void run(pipeline::commit_feedback_pack_t commit_feedback_pack)
            {
                if(commit_feedback_pack.enable && commit_feedback_pack.flush)
                {
                    reset();
                }
                else
                {
                    //handle write back
                    store_buffer_item_t item;

                    if(get_front(&item))
                    {
                        if(item.committed)
                        {
                            assert((item.size == 1) || (item.size == 2) || (item.size == 4));

                            switch(item.size)
                            {
                                case 1:
                                    memory_if->write8_sync(item.addr, (uint8_t)item.data);
                                    break;

                                case 2:
                                    memory_if->write16_sync(item.addr, (uint16_t)item.data);
                                    break;

                                case 4:
                                    memory_if->write32_sync(item.addr, item.data);
                                    break;
                            }
                        }
                    }

                    if(commit_feedback_pack.enable)
                    {
                        uint32_t cur_id;

                        if(get_front_id(&cur_id))
                        {
                            do
                            {
                                auto cur_item = get_item(cur_id);

                                for(auto i = 0;i < COMMIT_WIDTH;i++)
                                {
                                    if(commit_feedback_pack.committed_rob_id_valid[i] && (commit_feedback_pack.committed_rob_id[i] == cur_item.rob_id))
                                    {
                                        cur_item.committed = true;
                                    }
                                }

                                set_item(cur_id, cur_item);
                            }while(get_next_id(cur_id, &cur_id));
                        }
                    }
                }
            }
            
            void sync()
            {
                while(!sync_request_q.empty())
                {
                    auto item = sync_request_q.front();
                    sync_request_q.pop();
                    
                    switch(item.req)
                    {
                        case sync_request_type_t::push:
                            this->push(item.arg1.store_buffer_item);
                            break;
                            
                        case sync_request_type_t::pop:
                            T v;
                            this->pop(&v);
                            break;

                        case sync_request_type_t::restore:
                            this->restore(item.arg1.store_buffer_state_pack);
                            break;
                    }
                }
            }
    };
}
