#pragma once
#include "common.h"
#include "config.h"
#include "fifo.h"

namespace component
{
    static const uint32_t phy_reg_num_bitmap_size = (PHY_REG_NUM + bitsizeof(uint64_t) - 1) / (bitsizeof(uint64_t));

    typedef struct checkpoint_t : public if_print_t
    {
        uint64_t rat_phy_map_table_valid[phy_reg_num_bitmap_size];
        uint64_t rat_phy_map_table_visible[phy_reg_num_bitmap_size];
        uint64_t rat_phy_map_table_commit[phy_reg_num_bitmap_size];
        uint64_t phy_regfile_data_valid[phy_reg_num_bitmap_size];

        //only for global_cp
        uint32_t rat_phy_map_table[PHY_REG_NUM];

        void clone(checkpoint_t &cp)
        {
            memcpy(&cp.rat_phy_map_table_valid, &rat_phy_map_table_valid, sizeof(rat_phy_map_table_valid));
            memcpy(&cp.rat_phy_map_table_visible, &rat_phy_map_table_visible, sizeof(rat_phy_map_table_visible));
        }

        virtual void print(std::string indent)
        {
            
        }

        virtual json get_json()
        {
            json ret;
            return ret;
        }
    }checkpoint_t;

    class checkpoint_buffer : public fifo<checkpoint_t>
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
                checkpoint_t arg2;
            }sync_request_t;

            std::queue<sync_request_t> sync_request_q;

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
            checkpoint_buffer(uint32_t size) : fifo<checkpoint_t>(size)
            {
                
            }

            virtual void reset()
            {
                fifo<checkpoint_t>::reset();
                clear_queue(sync_request_q);
            }

            bool push(checkpoint_t element, uint32_t *item_id)
            {
                *item_id = this->wptr;
                return fifo<checkpoint_t>::push(element);
            }

            checkpoint_t get_item(uint32_t item_id)
            {
                return this->buffer[item_id];
            }

            void set_item(uint32_t item_id, checkpoint_t &item)
            {
                this->buffer[item_id] = item;
            }

            void set_item_sync(uint32_t item_id, checkpoint_t &item)
            {
                sync_request_t t_req;

                t_req.req = sync_request_type_t::set_item;
                t_req.arg1 = item_id;
                t_req.arg2 = item;
                sync_request_q.push(t_req);
            }

            checkpoint_t get_front()
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

            bool get_tail_id(uint32_t *tail_id)
            {
                if(this->is_empty())
                {
                    return false;
                }

                *tail_id = (wptr + this->size - 1) % this->size;
                return true;
            }

            bool get_prev_id(uint32_t id, uint32_t *prev_id)
            {
                assert(check_id_valid(id));
                *prev_id = (id + this->size - 1) % this->size;
                return check_id_valid(*prev_id);
            }

            bool get_next_id(uint32_t id, uint32_t *next_id)
            {
                assert(check_id_valid(id));
                *next_id = (id + 1) % this->size;
                return check_id_valid(*next_id);
            }

            bool pop()
            {
                checkpoint_t t;
                return fifo<checkpoint_t>::pop(&t);
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

            virtual json get_json()
            {
                json ret = json::array();
                return ret;
            }
    };
}