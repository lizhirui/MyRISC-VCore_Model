#include "common.h"
#include "issue.h"

namespace pipeline
{
    issue::issue(component::port<readreg_issue_pack_t> *readreg_issue_port, component::fifo<issue_execute_pack_t> **issue_alu_fifo, component::fifo<issue_execute_pack_t> **issue_bru_fifo, component::fifo<issue_execute_pack_t> **issue_csr_fifo, component::fifo<issue_execute_pack_t> **issue_div_fifo, component::fifo<issue_execute_pack_t> **issue_lsu_fifo, component::fifo<issue_execute_pack_t> **issue_mul_fifo) : issue_q(component::issue_queue<issue_queue_item_t>(ISSUE_QUEUE_SIZE))
    {
        this->readreg_issue_port = readreg_issue_port;
        this->issue_alu_fifo = issue_alu_fifo;
        this->issue_bru_fifo = issue_bru_fifo;
        this->issue_csr_fifo = issue_csr_fifo;
        this->issue_div_fifo = issue_div_fifo;
        this->issue_lsu_fifo = issue_lsu_fifo;
        this->issue_mul_fifo = issue_mul_fifo;
    }
    
    issue_feedback_pack_t issue::run(wb_feedback_pack_t wb_feedback_pack, commit_feedback_pack_t commit_feedback_pack)
    {
        auto rev_pack = readreg_issue_port->get();
        issue_feedback_pack_t feedback_pack;

        memset(&feedback_pack, 0, sizeof(feedback_pack));
        
        if(!(commit_feedback_pack.enable && commit_feedback_pack.flush))
        {
            //handle output
            if(!issue_q.is_empty())
            {
                issue_queue_item_t items[ISSUE_WIDTH];
                memset(&items, 0, sizeof(items));
                uint32_t id = 0;
            
                //get up to 2 items from issue_queue
                assert(this->issue_q.get_front_id(&id));
            
                for(uint32_t i = 0;i < ISSUE_WIDTH;i++)
                {
                    items[i] = this->issue_q.get_item(id);
                
                    if(!this->issue_q.get_next_id(id, &id))
                    {
                        break;
                    }
                }
            
                //generate output to execute stage
                for(uint32_t i = 0; i < ISSUE_WIDTH;i++)
                {
                    if(items[i].enable)
                    {
                        //wait src load
                        if(!(items[i].src1_loaded && items[i].src2_loaded))
                        {
                            break;
                        }

                        issue_execute_pack_t send_pack;
                        memset(&send_pack, 0, sizeof(send_pack));
                    
                        send_pack.enable = items[i].enable;
                        send_pack.value = items[i].value;
                        send_pack.valid = items[i].valid;
                        send_pack.rob_id = items[i].rob_id;
                        send_pack.pc = items[i].pc;
                        send_pack.imm = items[i].imm;
                        send_pack.has_exception = items[i].has_exception;
                        send_pack.exception_id = items[i].exception_id;
                        send_pack.exception_value = items[i].exception_value;
                    
                        send_pack.rs1 = items[i].rs1;
                        send_pack.arg1_src = items[i].arg1_src;
                        send_pack.rs1_need_map = items[i].rs1_need_map;
                        send_pack.rs1_phy = items[i].rs1_phy;
                        send_pack.src1_value = items[i].src1_value;
                        send_pack.src1_loaded = items[i].src1_loaded;
                    
                        send_pack.rs2 = items[i].rs2;
                        send_pack.arg2_src = items[i].arg2_src;
                        send_pack.rs2_need_map = items[i].rs2_need_map;
                        send_pack.rs2_phy = items[i].rs2_phy;
                        send_pack.src2_value = items[i].src2_value;
                        send_pack.src2_loaded = items[i].src2_loaded;
                    
                        send_pack.rd = items[i].rd;
                        send_pack.rd_enable = items[i].rd_enable;
                        send_pack.need_rename = items[i].need_rename;
                        send_pack.rd_phy = items[i].rd_phy;
                    
                        send_pack.csr = items[i].csr;
                        send_pack.op = items[i].op;
                        send_pack.op_unit = items[i].op_unit;
                        memcpy(&send_pack.sub_op, &items[i].sub_op, sizeof(items[i].sub_op));
                    
                        //ready to dispatch
                        uint32_t *unit_index = NULL;
                        uint32_t unit_cnt = 0;
                        component::fifo<issue_execute_pack_t> **unit_fifo = NULL;
                    
                        switch(send_pack.op_unit)
                        {
                            case op_unit_t::alu:
                                unit_index = &alu_index;
                                unit_cnt = ALU_UNIT_NUM;
                                unit_fifo = issue_alu_fifo;
                                break;
                            
                            case op_unit_t::bru:
                                unit_index = &bru_index;
                                unit_cnt = BRU_UNIT_NUM;
                                unit_fifo = issue_bru_fifo;
                                break;
                            
                            case op_unit_t::csr:
                                unit_index = &csr_index;
                                unit_cnt =  CSR_UNIT_NUM;
                                unit_fifo = issue_csr_fifo;
                                break;
                            
                            case op_unit_t::div:
                                unit_index = &div_index;
                                unit_cnt = DIV_UNIT_NUM;
                                unit_fifo = issue_div_fifo;
                                break;
                            
                            case op_unit_t::lsu:
                                unit_index = &lsu_index;
                                unit_cnt = LSU_UNIT_NUM;
                                unit_fifo = issue_lsu_fifo;
                                break;
                            
                            case op_unit_t::mul:
                                unit_index = &mul_index;
                                unit_cnt = MUL_UNIT_NUM;
                                unit_fifo = issue_mul_fifo;
                                break;
                        }
                    
                        //RR dispatch with full check
                        auto selected_index = *unit_index;
                        bool found = false;
                    
                        while(1)
                        {
                            if(!unit_fifo[selected_index]->is_full())
                            {
                                found = true;
                                break;
                            }
                        
                            selected_index = (selected_index + 1) % unit_cnt;
                        
                            if(selected_index == *unit_index)
                            {
                                break;
                            }
                        }
                    
                        if(found)
                        {
                            *unit_index = (selected_index + 1) % unit_cnt;
                            assert(unit_fifo[selected_index]->push(send_pack));
                            this->issue_q.pop_sync();//handle ok, pop this item
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }    

            //handle queue(feedback pack activates blocking items)
            for(auto i = 0;i < EXECUTE_UNIT_NUM;i++)
            {
                if(wb_feedback_pack.channel[i].enable)
                {
                    uint32_t cur_item_id = 0;

                    if(issue_q.get_front_id(&cur_item_id))
                    {
                        do
                        {
                            auto cur_item = issue_q.get_item(cur_item_id);
                            auto modified = false;

                            if(!cur_item.src1_loaded && cur_item.rs1_need_map && (cur_item.rs1_phy == wb_feedback_pack.channel[i].phy_id))
                            {
                                cur_item.src1_loaded = true;
                                cur_item.src1_value = wb_feedback_pack.channel[i].value;
                                modified = true;
                            }

                            if(!cur_item.src2_loaded && cur_item.rs2_need_map && (cur_item.rs2_phy == wb_feedback_pack.channel[i].phy_id))
                            {
                                cur_item.src2_loaded = true;
                                cur_item.src2_value = wb_feedback_pack.channel[i].value;
                                modified = true;
                            }

                            if(modified)
                            {
                                issue_q.set_item_sync(cur_item_id, cur_item);
                            }
                        }while(issue_q.get_next_id(cur_item_id, &cur_item_id));
                    }
                }
            }
        
            //handle input
            if(!this->busy)
            {
                this->busy = true;
                this->last_index = 0;//from item 0
            }
        
            auto finish = true;
        
            for(;this->last_index < ISSUE_WIDTH;this->last_index++)
            {
                if(!rev_pack.op_info[this->last_index].enable)
                {
                    continue;
                }
            
                //if issue_queue is full, pause to handle this input until next cycle
                if(this->issue_q.is_full())
                {
                    finish = false;
                    break;
                }
            
                issue_queue_item_t t_item;
                memset(&t_item, 0, sizeof(t_item));
                auto cur_op = rev_pack.op_info[this->last_index];
                t_item.enable = cur_op.enable;
                t_item.value = cur_op.value;
                t_item.valid = cur_op.valid;
                t_item.rob_id = cur_op.rob_id;
                t_item.pc = cur_op.pc;
                t_item.imm = cur_op.imm;
            
                t_item.rs1 = cur_op.rs1;
                t_item.arg1_src = cur_op.arg1_src;
                t_item.rs1_need_map = cur_op.rs1_need_map;
                t_item.rs1_phy = cur_op.rs1_phy;
                t_item.src1_value = cur_op.src1_value;
                t_item.src1_loaded = cur_op.src1_loaded;
            
                t_item.rs2 = cur_op.rs2;
                t_item.arg2_src = cur_op.arg2_src;
                t_item.rs2_need_map = cur_op.rs2_need_map;
                t_item.rs2_phy = cur_op.rs2_phy;
                t_item.src2_value = cur_op.src2_value;
                t_item.src2_loaded = cur_op.src2_loaded;
            
                t_item.rd = cur_op.rd;
                t_item.rd_enable = cur_op.rd_enable;
                t_item.need_rename = cur_op.need_rename;
                t_item.rd_phy = cur_op.rd_phy;

                t_item.csr = cur_op.csr;
                t_item.op = cur_op.op;
                t_item.op_unit = cur_op.op_unit;
                memcpy(&t_item.sub_op, &cur_op.sub_op, sizeof(t_item.sub_op));
            
                issue_q.push(t_item);
            }
        
            if(finish)
            {
                this->busy = false;
                this->last_index = 0;
            }
        
            feedback_pack.stall = this->busy;
            issue_q.sync();
        }
        else
        {
            for(auto i = 0;i < ALU_UNIT_NUM;i++)
            {
                issue_alu_fifo[i]->flush();
            }

            for(auto i = 0;i < BRU_UNIT_NUM;i++)
            {
                issue_bru_fifo[i]->flush();
            }

            for(auto i = 0;i < CSR_UNIT_NUM;i++)
            {
                issue_csr_fifo[i]->flush();
            }

            for(auto i = 0;i < DIV_UNIT_NUM;i++)
            {
                issue_div_fifo[i]->flush();
            }

            for(auto i = 0;i < LSU_UNIT_NUM;i++)
            {
                issue_lsu_fifo[i]->flush();
            }

            for(auto i = 0;i < MUL_UNIT_NUM;i++)
            {
                issue_mul_fifo[i]->flush();
            }
        }

        return feedback_pack;
    }

    void issue::print(std::string indent)
    {
        issue_q.print(indent);
    }
}
