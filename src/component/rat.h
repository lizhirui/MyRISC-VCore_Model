#pragma once
#include "common.h"

namespace component
{
    class rat : if_print_t
    {
        private:
            uint32_t phy_reg_num;
            uint32_t arch_reg_num;
            uint32_t *phy_map_table;
            uint64_t *phy_map_table_valid;
            uint64_t *phy_map_table_visible;
            uint32_t bitmap_size;
            bool init_rat;

            enum class sync_request_type_t
            {
                set_map,
                release_map
            };

            typedef struct sync_request
            {
                sync_request_type_t req;
                uint32_t arg1;
                uint32_t arg2;
            }sync_request;

            std::queue<sync_request> sync_request_q;

            void set_valid(uint32_t phy_id, bool v)
            {
                assert(phy_id < phy_reg_num);
                
                if(v)
                {
                    phy_map_table_valid[phy_id / bitsizeof(phy_map_table_valid[0])] |= 1ULL << (phy_id % bitsizeof(phy_map_table_valid[0]));
                }
                else
                {
                    phy_map_table_valid[phy_id / bitsizeof(phy_map_table_valid[0])] &= ~(1ULL << (phy_id % bitsizeof(phy_map_table_valid[0])));
                }
            }

            bool get_valid(uint32_t phy_id)
            {
                assert(phy_id < phy_reg_num);
                return phy_map_table_valid[phy_id / bitsizeof(phy_map_table_valid[0])] & (1ULL << (phy_id % bitsizeof(phy_map_table_valid[0])));
            }

            void set_visible(uint32_t phy_id, bool v)
            {
                assert(phy_id < phy_reg_num);
                
                if(v)
                {
                    phy_map_table_visible[phy_id / bitsizeof(phy_map_table_visible[0])] |= 1ULL << (phy_id % bitsizeof(phy_map_table_visible[0]));
                }
                else
                {
                    phy_map_table_visible[phy_id / bitsizeof(phy_map_table_visible[0])] &= ~(1ULL << (phy_id % bitsizeof(phy_map_table_visible[0])));
                }
            }

            bool get_visible(uint32_t phy_id)
            {
                assert(phy_id < phy_reg_num);
                return phy_map_table_visible[phy_id / bitsizeof(phy_map_table_visible[0])] & (1ULL << (phy_id % bitsizeof(phy_map_table_visible[0])));
            }

        public:
            rat(uint32_t phy_reg_num, uint32_t arch_reg_num)
            {
                this -> phy_reg_num = phy_reg_num;         
                this -> arch_reg_num = arch_reg_num;
                phy_map_table = new uint32_t[phy_reg_num];
                bitmap_size = (phy_reg_num + bitsizeof(phy_map_table_valid[0]) - 1) / (bitsizeof(phy_map_table_valid[0]));
                phy_map_table_valid = new uint64_t[bitmap_size];
                phy_map_table_visible = new uint64_t[bitmap_size];
                init_rat = false;
            }

            ~rat()
            {
                delete[] phy_map_table;
                delete[] phy_map_table_valid;
                delete[] phy_map_table_visible;
            }

            void init_start()
            {
                memset(phy_map_table, 0, sizeof(uint32_t) * phy_reg_num);
                memset(phy_map_table_valid, 0, sizeof(phy_map_table_valid[0]) * bitmap_size);
                memset(phy_map_table_visible, 0, sizeof(phy_map_table_visible[0]) * bitmap_size);
                
                while(!sync_request_q.empty())
                {
                    sync_request_q.pop();
                }

                init_rat = true;
            }

            void init_finish()
            {
                init_rat = false;
            }

            uint32_t get_free_phy_id(uint32_t num, uint32_t *ret)
            {
                uint32_t ret_cnt = 0;

                for(uint32_t i = 0;i < phy_reg_num;i++)
                {
                    if(!get_valid(i))
                    {
                        ret[ret_cnt++] = i;

                        if(ret_cnt >= num)
                        {
                            break;
                        }
                    }
                }

                return ret_cnt;
            }

            bool get_phy_id(uint32_t arch_id, uint32_t *phy_id)
            {
                int cnt = 0;
                assert((arch_id > 0) && (arch_id < arch_reg_num));

                for(uint32_t i = 0;i < phy_reg_num;i++)
                {
                    if(get_valid(i) && get_visible(i) && (phy_map_table[i] == arch_id))
                    {
                        *phy_id = i;
                        cnt++;
                    }
                }

                assert(cnt <= 1);
                return cnt == 1;
            }

            uint32_t set_map(uint32_t arch_id, uint32_t phy_id)
            {
                uint32_t old_phy_id;
                assert(phy_id < phy_reg_num);
                assert((arch_id > 0) && (arch_id < arch_reg_num));
                bool ret = get_phy_id(arch_id, &old_phy_id);

                if(!init_rat)
                {
                    assert(ret);
                    assert(!get_valid(phy_id));
                }

                phy_map_table[phy_id] = arch_id;
                set_valid(phy_id, true);
                set_visible(phy_id, true);

                if(ret)
                {
                    set_visible(old_phy_id, false);
                }

                return old_phy_id;
            }

            void release_map(uint32_t phy_id)
            {
                 assert(phy_id < phy_reg_num);
                 assert(get_valid(phy_id));
                 assert(!get_visible(phy_id));
                 phy_map_table[phy_id] = 0;
                 set_valid(phy_id, false);
            }

            /*
            void mask_map(uint32_t phy_id)
            {
                assert(phy_id < phy_reg_num);
                assert(phy_map_table_valid & (1 << phy_id));
                assert(phy_map_table_visible & (1 << phy_id));
                phy_map_table_visible &= ~(1 << phy_id);
            }*/

            void set_map_sync(uint32_t arch_id, uint32_t phy_id)
            {
                sync_request t_req;

                t_req.req = sync_request_type_t::set_map;
                t_req.arg1 = arch_id;
                t_req.arg2 = phy_id;
                sync_request_q.push(t_req);
            }

            void release_map_sync(uint32_t phy_id)
            {
                sync_request t_req;

                t_req.req = sync_request_type_t::release_map;
                t_req.arg1 = phy_id;
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
                        case sync_request_type_t::set_map:
                            set_map(t_req.arg1, t_req.arg2);
                            break;

                        case sync_request_type_t::release_map:
                            release_map(t_req.arg1);
                            break;
                    }
                }
            }

            virtual void print(std::string indent)
            {
                auto col = 6;

                std::cout << indent << "Register Allocation Table:" << std::endl;

                for(auto i = 0;i < col;i++)
                {
                    if(i == 0)
                    {
                        std::cout << indent;
                    }
                    else
                    {
                        std::cout << "\t\t";
                    }

                    std::cout << "Phy_ID\tArch_ID\tVisible\tValid";
                }
                
                std::cout << std::endl;

                auto numbycol = (phy_reg_num + col - 1) / col;
                
                for(uint32_t i = 0;i < numbycol;i++)
                {
                    for(auto j = 0;j < col;j++)
                    {
                        auto phy_id = j * numbycol + i;

                        if(phy_id < phy_reg_num)
                        {
                            if(j == 0)
                            {
                                std::cout << indent;
                            }
                            else
                            {
                                std::cout << "\t\t";
                            }

                            std::cout << phy_id << "\t" << phy_map_table[phy_id] << "\t" << outbool(get_visible(phy_id)) << "\t" << outbool(get_valid(phy_id));
                        }
                    }

                    std::cout << std::endl;
                }
            }
    };
}