#pragma once
#include "common.h"
#include "fifo.h"

namespace component
{
    typedef struct rob_item_t : public if_print_t
    {
        uint32_t new_phy_reg_id;
        uint32_t old_phy_reg_id;
        bool old_phy_reg_id_valid;
        bool finish;
        uint32_t pc;
        uint32_t inst_value;
        bool has_exception;
        riscv_exception_t exception_id;
        uint32_t exception_value;

        virtual void print(std::string indent)
        {
            std::string blank = "    ";
            std::cout << indent << "new_phy_reg_id = " << new_phy_reg_id;
            std::cout << blank << "old_phy_reg_id = " << old_phy_reg_id;
            std::cout << blank << "old_phy_reg_id_valid = " << outbool(old_phy_reg_id_valid);
            std::cout << blank << "finish = " << outbool(finish);
            std::cout << blank << "pc = 0x" << fillzero(8) << outhex(pc);
            std::cout << blank << "inst_value = 0x" << fillzero(8) << outhex(inst_value);
            std::cout << blank << "has_exception = " << outbool(has_exception);
            std::cout << blank << "exception_id = " << outenum(exception_id);
            std::cout << blank << "exception_value = 0x" << fillzero(8) << outhex(exception_value) << std::endl;
        }
    }rob_item_t;

    class rob : public fifo<rob_item_t>
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
                rob_item_t arg2;
            }sync_request_t;

            std::queue<sync_request_t> sync_request_q;

            bool committed = false;

            bool check_new_id_valid(uint32_t id)
            {
                if(this->is_full())
                {
                    return false;
                }
                else if(this->wstage == this->rstage)
                {
                    return !((id >= this->rptr) && (id < this->wptr));
                }
                else
                {
                    return !(((id >= this->rptr) && (id < this->size)) || (id < this->wptr));
                }
            }

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
            rob(uint32_t size) : fifo<rob_item_t>(size)
            {
                
            }

            bool get_committed()
            {
                return committed;
            }

            void set_committed(bool value)
            {
                committed = value;
            }

            bool push(rob_item_t element, uint32_t *item_id)
            {
                *item_id = this->wptr;
                return fifo<rob_item_t>::push(element);
            }

            rob_item_t get_item(uint32_t item_id)
            {
                return this->buffer[item_id];
            }

            void set_item(uint32_t item_id, rob_item_t item)
            {
                this->buffer[item_id] = item;
            }

            void set_item_sync(uint32_t item_id, rob_item_t item)
            {
                sync_request_t t_req;

                t_req.req = sync_request_type_t::set_item;
                t_req.arg1 = item_id;
                t_req.arg2 = item;
                sync_request_q.push(t_req);
            }

            rob_item_t get_front()
            {
                return this->buffer[rptr];
            }

            uint32_t get_size()
            {
                return this->is_full() ? this->size : ((this->wptr + this->size - this->rptr) % this->size);
            }

            uint32_t get_free_space()
            {
                return this->size - get_size();
            }

            bool get_new_id(uint32_t *new_id)
            {
                if(this->is_full())
                {
                    return false;
                }

                *new_id = this->wptr;
                return true;
            }

            bool get_next_new_id(uint32_t cur_new_id, uint32_t *next_new_id)
            {
                if(!check_new_id_valid(cur_new_id))
                {
                    return false;
                }

                *next_new_id = (cur_new_id + 1) % this->size;
                return check_new_id_valid(*next_new_id);
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

            bool get_next_id(uint32_t id, uint32_t *next_id)
            {
                assert(check_id_valid(id));
                *next_id = (id + 1) % this->size;
                return check_id_valid(*next_id);
            }

            bool pop()
            {
                rob_item_t t;
                return fifo<rob_item_t>::pop(&t);
            }

            void pop_sync()
            {
                sync_request_t t_req;

                t_req.req = sync_request_type_t::pop;
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
                        case sync_request_type_t::pop:
                            pop();
                            break;

                        case sync_request_type_t::set_item:
                            set_item(t_req.arg1, t_req.arg2);
                            break;
                    }
                }
            }
    };
}