#pragma once
#include "common.h"
#include "checkpoint_buffer.h"

namespace component
{
    class rat : public if_print_t
    {
        private:
            enum class sync_request_type_t
            {
                set_map,
                release_map,
                commit_map,
                restore_map,
                restore
            };

            typedef struct sync_request_t
            {
                sync_request_type_t req;
                uint32_t arg1;
                uint32_t arg2;
                checkpoint_t cp;
            }sync_request_t;

            std::queue<sync_request_t> sync_request_q;

            uint32_t phy_reg_num;
            uint32_t arch_reg_num;
            uint32_t *phy_map_table;
            uint64_t *phy_map_table_valid;
            uint64_t *phy_map_table_visible;
            uint64_t *phy_map_table_commit;
            uint32_t bitmap_size;
            bool init_rat;

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

            void set_commit(uint32_t phy_id, bool v)
            {
                assert(phy_id < phy_reg_num);
                
                if(v)
                {
                    phy_map_table_commit[phy_id / bitsizeof(phy_map_table_commit[0])] |= 1ULL << (phy_id % bitsizeof(phy_map_table_commit[0]));
                }
                else
                {
                    phy_map_table_commit[phy_id / bitsizeof(phy_map_table_commit[0])] &= ~(1ULL << (phy_id % bitsizeof(phy_map_table_commit[0])));
                }
            }

            bool get_commit(uint32_t phy_id)
            {
                assert(phy_id < phy_reg_num);
                return phy_map_table_commit[phy_id / bitsizeof(phy_map_table_commit[0])] & (1ULL << (phy_id % bitsizeof(phy_map_table_commit[0])));
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
                phy_map_table_commit = new uint64_t[bitmap_size];
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
                memset(phy_map_table_commit, 0, sizeof(phy_map_table_commit[0]) * bitmap_size);
                clear_queue(sync_request_q);
                init_rat = true;
            }

            void init_finish()
            {
                init_rat = false;
            }

            void cp_set_valid(checkpoint_t &cp, uint32_t phy_id, bool v)
            {
                assert(phy_id < phy_reg_num);
                
                if(v)
                {
                    cp.rat_phy_map_table_valid[phy_id / bitsizeof(cp.rat_phy_map_table_valid[0])] |= 1ULL << (phy_id % bitsizeof(cp.rat_phy_map_table_valid[0]));
                }
                else
                {
                    cp.rat_phy_map_table_valid[phy_id / bitsizeof(cp.rat_phy_map_table_valid[0])] &= ~(1ULL << (phy_id % bitsizeof(cp.rat_phy_map_table_valid[0])));
                }
            }

            bool cp_get_valid(checkpoint_t &cp, uint32_t phy_id)
            {
                assert(phy_id < phy_reg_num);
                return cp.rat_phy_map_table_valid[phy_id / bitsizeof(cp.rat_phy_map_table_valid[0])] & (1ULL << (phy_id % bitsizeof(cp.rat_phy_map_table_valid[0])));
            }

            void cp_set_visible(checkpoint_t &cp, uint32_t phy_id, bool v)
            {
                assert(phy_id < phy_reg_num);
                
                if(v)
                {
                    cp.rat_phy_map_table_visible[phy_id / bitsizeof(cp.rat_phy_map_table_visible[0])] |= 1ULL << (phy_id % bitsizeof(cp.rat_phy_map_table_visible[0]));
                }
                else
                {
                    cp.rat_phy_map_table_visible[phy_id / bitsizeof(cp.rat_phy_map_table_visible[0])] &= ~(1ULL << (phy_id % bitsizeof(cp.rat_phy_map_table_visible[0])));
                }
            }

            bool cp_get_visible(checkpoint_t &cp, uint32_t phy_id)
            {
                assert(phy_id < phy_reg_num);
                return cp.rat_phy_map_table_visible[phy_id / bitsizeof(cp.rat_phy_map_table_visible[0])] & (1ULL << (phy_id % bitsizeof(cp.rat_phy_map_table_visible[0])));
            }

            void cp_set_commit(checkpoint_t &cp, uint32_t phy_id, bool v)
            {
                assert(phy_id < phy_reg_num);
                
                if(v)
                {
                    cp.rat_phy_map_table_commit[phy_id / bitsizeof(cp.rat_phy_map_table_commit[0])] |= 1ULL << (phy_id % bitsizeof(cp.rat_phy_map_table_commit[0]));
                }
                else
                {
                    cp.rat_phy_map_table_commit[phy_id / bitsizeof(cp.rat_phy_map_table_commit[0])] &= ~(1ULL << (phy_id % bitsizeof(cp.rat_phy_map_table_commit[0])));
                }
            }

            bool cp_get_commit(checkpoint_t &cp, uint32_t phy_id)
            {
                assert(phy_id < phy_reg_num);
                return cp.rat_phy_map_table_commit[phy_id / bitsizeof(cp.rat_phy_map_table_commit[0])] & (1ULL << (phy_id % bitsizeof(cp.rat_phy_map_table_commit[0])));
            }

            bool cp_get_phy_id(checkpoint_t &cp, uint32_t arch_id, uint32_t *phy_id)
            {
                int cnt = 0;
                assert((arch_id > 0) && (arch_id < arch_reg_num));

                for(uint32_t i = 0;i < phy_reg_num;i++)
                {
                    if(cp_get_valid(cp, i) && cp_get_visible(cp, i) && (cp.rat_phy_map_table[i] == arch_id))
                    {
                        *phy_id = i;
                        cnt++;
                    }
                }

                assert(cnt <= 1);
                return cnt == 1;
            }

            void cp_set_map(checkpoint_t &cp, uint32_t arch_id, uint32_t phy_id)
            {
                uint32_t old_phy_id;
                assert(phy_id < phy_reg_num);
                assert((arch_id > 0) && (arch_id < arch_reg_num));
                assert(!cp_get_valid(cp, phy_id));
                bool ret = cp_get_phy_id(cp, arch_id, &old_phy_id);
                cp.rat_phy_map_table[phy_id] = arch_id;
                cp_set_valid(cp, phy_id, true);
                cp_set_visible(cp, phy_id, true);
                cp_set_commit(cp, phy_id, false);

                if(ret)
                {
                    cp_set_visible(cp, old_phy_id, false);
                }
            }

            void save(checkpoint_t &cp)
            {
                memcpy(cp.rat_phy_map_table_valid, phy_map_table_valid, sizeof(cp.rat_phy_map_table_valid));
                memcpy(cp.rat_phy_map_table_visible, phy_map_table_visible, sizeof(cp.rat_phy_map_table_visible));
                //memcpy(cp.rat_phy_map_table_commit, phy_map_table_commit, sizeof(cp.rat_phy_map_table_commit));

                //only for global_cp
                memcpy(cp.rat_phy_map_table, phy_map_table, sizeof(cp.rat_phy_map_table));
            }

            void restore(checkpoint_t &cp)
            {
                memcpy(phy_map_table_valid, cp.rat_phy_map_table_valid, sizeof(cp.rat_phy_map_table_valid));
                memcpy(phy_map_table_visible, cp.rat_phy_map_table_visible, sizeof(cp.rat_phy_map_table_visible));
                //memcpy(phy_map_table_commit, cp.rat_phy_map_table_commit, sizeof(cp.rat_phy_map_table_commit));
            }

            void restore_sync(checkpoint_t &cp)
            {
                sync_request_t t_req;

                t_req.req = sync_request_type_t::restore;
                t_req.cp = cp;
                sync_request_q.push(t_req);
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

            void get_commit_phy_id(uint32_t arch_id, uint32_t *phy_id)
            {
                int cnt = 0;
                assert((arch_id > 0) && (arch_id < arch_reg_num));

                for(uint32_t i = 0;i < phy_reg_num;i++)
                {
                    if(get_valid(i) && get_commit(i) && (phy_map_table[i] == arch_id))
                    {
                        *phy_id = i;
                        cnt++;
                    }
                }

                assert(cnt == 1);
            }

            void commit_map(uint32_t phy_id)
            {
                assert(phy_id < phy_reg_num);
                assert(get_valid(phy_id));
                assert(!get_commit(phy_id));
                set_commit(phy_id, true);
            }

            void cp_commit_map(checkpoint_t &cp, uint32_t phy_id)
            {
                assert(phy_id < phy_reg_num);
                assert(cp_get_valid(cp, phy_id));
                assert(!cp_get_commit(cp, phy_id));
                cp_set_commit(cp, phy_id, true);
            }

            void commit_map_sync(uint32_t phy_id)
            {
                sync_request_t t_req;

                t_req.req = sync_request_type_t::commit_map;
                t_req.arg1 = phy_id;
                sync_request_q.push(t_req);
            }

            uint32_t set_map(uint32_t arch_id, uint32_t phy_id)
            {
                uint32_t old_phy_id;
                assert(phy_id < phy_reg_num);
                assert((arch_id > 0) && (arch_id < arch_reg_num));
                assert(!get_valid(phy_id));
                bool ret = get_phy_id(arch_id, &old_phy_id);

                if(!init_rat)
                {
                    assert(ret);
                    assert(!get_valid(phy_id));
                }

                phy_map_table[phy_id] = arch_id;
                set_valid(phy_id, true);
                set_visible(phy_id, true);
                set_commit(phy_id, false);

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

            void cp_release_map(checkpoint_t &cp, uint32_t phy_id)
            {
                assert(phy_id < phy_reg_num);
                assert(cp_get_valid(cp, phy_id));
                assert(!cp_get_visible(cp, phy_id));
                cp_set_valid(cp, phy_id, false);
            }

            void restore_map(uint32_t new_phy_id, uint32_t old_phy_id)
            {
                assert(new_phy_id < phy_reg_num);
                assert(old_phy_id < phy_reg_num);
                assert(get_valid(new_phy_id));
                assert(get_valid(old_phy_id));
                assert(get_visible(new_phy_id));
                assert(!get_visible(old_phy_id));
                phy_map_table[new_phy_id] = 0;
                set_valid(new_phy_id, false);
                set_visible(new_phy_id, false);
                set_valid(old_phy_id, true);
                set_visible(old_phy_id, true);
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
                sync_request_t t_req;

                assert(phy_id < phy_reg_num);
                assert((arch_id > 0) && (arch_id < arch_reg_num));
                t_req.req = sync_request_type_t::set_map;
                t_req.arg1 = arch_id;
                t_req.arg2 = phy_id;
                sync_request_q.push(t_req);
            }

            void release_map_sync(uint32_t phy_id)
            {
                sync_request_t t_req;

                t_req.req = sync_request_type_t::release_map;
                t_req.arg1 = phy_id;
                sync_request_q.push(t_req);
            }

            void restore_map_sync(uint32_t new_phy_id, uint32_t old_phy_id)
            {
                sync_request_t t_req;
                
                t_req.req = sync_request_type_t::restore_map;
                t_req.arg1 = new_phy_id;
                t_req.arg2 = old_phy_id;
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
                        case sync_request_type_t::set_map:
                            set_map(t_req.arg1, t_req.arg2);
                            break;

                        case sync_request_type_t::release_map:
                            release_map(t_req.arg1);
                            break;

                        case sync_request_type_t::commit_map:
                            commit_map(t_req.arg1);
                            break;
                        
                        case sync_request_type_t::restore_map:
                            restore_map(t_req.arg1, t_req.arg2);
                            break;

                        case sync_request_type_t::restore:
                            restore(t_req.cp);
                            break;
                    }
                }
            }

            virtual void print(std::string indent)
            {
                auto col = 5;

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

                    std::cout << "Phy_ID\tArch_ID\tVisible\tCommit\tValid";
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

                            std::cout << phy_id << "\t" << phy_map_table[phy_id] << "\t" << outbool(get_visible(phy_id)) << "\t" << outbool(get_commit(phy_id)) << "\t" << outbool(get_valid(phy_id));
                        }
                    }

                    std::cout << std::endl;
                }
            }
    };
}