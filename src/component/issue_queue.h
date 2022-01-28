#pragma once
#include "common.h"
#include "fifo.h"

namespace component
{
    template<typename T>
    class issue_queue : public fifo<T>
    {
        private:
            enum class sync_request_type_t
            {
                pop,
                set_item
            };
            
            typedef struct sync_request_t
            {
                sync_request_type_t req;
                uint32_t arg1;
                T arg2;
            }sync_request_t;
            
            std::queue<sync_request_t> sync_q;
            
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
        
        public:
            issue_queue(uint32_t size) : fifo<T>(size)
            {
                            
            }
            
            uint32_t get_size()
            {
                return this->is_full() ? this->size : ((this->wptr + this->size - this->rptr) % this->size);
            }
            
            T get_item(uint32_t id)
            {
                assert(check_id_valid(id));
                return this->buffer[id];
            }
            
            void set_item(uint32_t id, T value)
            {
                assert(check_id_valid(id));
                this->buffer[id] = value;
            }
            
            void set_item_sync(uint32_t id, T value)
            {
                sync_request_t item;
                
                item.req = sync_request_type_t::set_item;
                item.arg1 = id;
                item.arg2 = value;
                sync_q.push(item);
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
                
                *tail_id = this->wptr - 1;
                return true;
            }
            
            bool get_next_id(uint32_t id, uint32_t *next_id)
            {
                assert(check_id_valid(id));
                *next_id = (id + 1) % this->size;
                return check_id_valid(*next_id);
            }
            
            bool get_front(T *value)
            {
                *value = this->buffer[this->rptr];
                return !this->is_empty();
            }
            
            void pop_sync()
            {
                sync_request_t item;
                    
                item.req = sync_request_type_t::pop;
                sync_q.push(item);
            }
            
            void sync()
            {
                while(!sync_q.empty())
                {
                    auto item = sync_q.front();
                    sync_q.pop();
                    
                    switch(item.req)
                    {
                        case sync_request_type_t::set_item:
                            set_item(item.arg1, item.arg2);
                            break;
                            
                        case sync_request_type_t::pop:
                            T v;
                            this->pop(&v);
                            break;
                    }
                }
            }
    };
}
