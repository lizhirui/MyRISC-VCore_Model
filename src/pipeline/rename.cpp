#include "common.h"
#include "config.h"
#include "pipeline_common.h"
#include "rename.h"
#include "issue.h"

namespace pipeline
{
    rename::rename(component::fifo<decode_rename_pack_t> *decode_rename_fifo, component::port<rename_readreg_pack_t> *rename_readreg_port, component::rat *rat, component::rob *rob, component::checkpoint_buffer *checkpoint_buffer)
    {
        this->decode_rename_fifo = decode_rename_fifo;
        this->rename_readreg_port = rename_readreg_port;
        this->rat = rat;
        this->rob = rob;
        this->checkpoint_buffer = checkpoint_buffer;
        this->busy = false;
    }

    void rename::reset()
    {
        this->busy = false;
    }

    rename_feedback_pack_t rename::run(issue_feedback_pack_t issue_pack, commit_feedback_pack_t commit_feedback_pack)
    {
        //rename_readreg_pack_t null_send_pack;
        bool stall = issue_pack.stall;

        rename_feedback_pack_t feedback_pack;

        feedback_pack.idle = decode_rename_fifo->is_empty();
        
        //memset(&null_send_pack, 0, sizeof(null_send_pack));

        if(!(commit_feedback_pack.enable && commit_feedback_pack.flush))
        {
            if(!stall)
            {
                //this->rename_readreg_port->set(null_send_pack);//bubble

                rename_readreg_pack_t send_pack;
                memset(&send_pack, 0, sizeof(send_pack));
                    
                uint32_t phy_reg_req_cnt = 0;
                uint32_t rob_req_cnt = 0;
                uint32_t new_phy_reg_id[RENAME_WIDTH];
                component::rob_item_t rob_item[RENAME_WIDTH];
                memset(rob_item, 0 ,sizeof(rob_item));
                uint32_t used_phy_reg_cnt = 0;
                component::checkpoint_t global_cp;
                rat->save(global_cp);

                //generate base send_pack
                for(uint32_t i = 0;i < RENAME_WIDTH;i++)
                {
                    if(!decode_rename_fifo->is_empty())
                    {
                        decode_rename_fifo->get_front(&rev_pack);

                        //count new physical registers requirement and rob requirement
                        if(rev_pack.enable && rev_pack.valid && rev_pack.need_rename)
                        {
                            phy_reg_req_cnt++;
                        }

                        if(rev_pack.enable)
                        {
                            rob_req_cnt++;
                        }

                        //start to rename
                        if((rat->get_free_phy_id(phy_reg_req_cnt, new_phy_reg_id) >= phy_reg_req_cnt) && rob->get_free_space() >= 1)
                        {
                            decode_rename_fifo->pop(&rev_pack);
                            send_pack.op_info[i].enable = rev_pack.enable;
                            send_pack.op_info[i].value = rev_pack.value;
                            send_pack.op_info[i].valid = rev_pack.valid;
                            send_pack.op_info[i].pc = rev_pack.pc;
                            send_pack.op_info[i].imm = rev_pack.imm;
                            send_pack.op_info[i].has_exception = rev_pack.has_exception;
                            send_pack.op_info[i].exception_id = rev_pack.exception_id;
                            send_pack.op_info[i].exception_value = rev_pack.exception_value;
                            send_pack.op_info[i].predicted = rev_pack.predicted;
                            send_pack.op_info[i].predicted_jump = rev_pack.predicted_jump;
                            send_pack.op_info[i].predicted_next_pc = rev_pack.predicted_next_pc;
                            send_pack.op_info[i].checkpoint_id_valid = rev_pack.checkpoint_id_valid;
                            send_pack.op_info[i].checkpoint_id = rev_pack.checkpoint_id;
                            send_pack.op_info[i].rs1 = rev_pack.rs1;
                            send_pack.op_info[i].arg1_src = rev_pack.arg1_src;
                            send_pack.op_info[i].rs1_need_map = rev_pack.rs1_need_map;
                            send_pack.op_info[i].rs2 = rev_pack.rs2;
                            send_pack.op_info[i].arg2_src = rev_pack.arg2_src;
                            send_pack.op_info[i].rs2_need_map = rev_pack.rs2_need_map;
                            send_pack.op_info[i].rd = rev_pack.rd;
                            send_pack.op_info[i].rd_enable = rev_pack.rd_enable;
                            send_pack.op_info[i].need_rename = rev_pack.need_rename;
                            send_pack.op_info[i].csr = rev_pack.csr;
                            send_pack.op_info[i].op = rev_pack.op;
                            send_pack.op_info[i].op_unit = rev_pack.op_unit;
                        
                            memcpy(&send_pack.op_info[i].sub_op, &rev_pack.sub_op, sizeof(rev_pack.sub_op));
                            //generate rob items
                            if(rev_pack.enable)
                            {
                                if(rev_pack.valid)
                                {
                                    if(rev_pack.need_rename)
                                    {
                                        rob_item[i].old_phy_reg_id_valid = true;
                                        assert(rat->get_phy_id(rev_pack.rd, &rob_item[i].old_phy_reg_id));
                                        send_pack.op_info[i].rd_phy = new_phy_reg_id[used_phy_reg_cnt++];
                                        rat->set_map_sync(rev_pack.rd, send_pack.op_info[i].rd_phy);
                                        rat->cp_set_map(global_cp, rev_pack.rd, send_pack.op_info[i].rd_phy);

                                        if(i > 0)
                                        {
                                            //old_phy_reg_id feedback
                                            for(auto j = i - 1;;j--)
                                            {
                                                if(send_pack.op_info[j].enable && send_pack.op_info[j].valid && send_pack.op_info[j].need_rename)
                                                {
                                                    if(rev_pack.rd == send_pack.op_info[j].rd)
                                                    {
                                                        rob_item[i].old_phy_reg_id = send_pack.op_info[j].rd_phy;
                                                        break;
                                                    }
                                                }

                                                if(j == 0)
                                                {
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        rob_item[i].old_phy_reg_id_valid = false;
                                        rob_item[i].old_phy_reg_id = 0;
                                    }
                                }

                                rob_item[i].finish = false;
                                //fill rob item
                                rob_item[i].new_phy_reg_id = send_pack.op_info[i].rd_phy;
                                rob_item[i].pc = rev_pack.pc;
                                rob_item[i].inst_value = rev_pack.value;
                                rob_item[i].has_exception = rev_pack.has_exception;
                                rob_item[i].exception_id = rev_pack.exception_id;
                                rob_item[i].exception_value = rev_pack.exception_value;
                                rob_item[i].is_mret = rev_pack.op == op_t::mret;
                                rob_item[i].is_csr = rev_pack.op_unit == op_unit_t::csr;
                                rob_item[i].csr_addr = rev_pack.csr;
                                //write to rob
                                assert(rob->get_new_id(&send_pack.op_info[i].rob_id));
                                uint32_t t;
                                assert(rob->push(rob_item[i], &t));

                                //start to map source registers
                                if(rev_pack.rs1_need_map)
                                {
                                    assert(rat->get_phy_id(rev_pack.rs1, &send_pack.op_info[i].rs1_phy));
                                }

                                if(rev_pack.rs2_need_map)
                                {
                                    assert(rat->get_phy_id(rev_pack.rs2, &send_pack.op_info[i].rs2_phy));
                                }

                                //source registers feedback
                                if(rev_pack.valid)
                                {
                                    for(uint32_t j = 0;j < i;j++)
                                    {
                                        if(send_pack.op_info[j].enable && send_pack.op_info[j].valid && send_pack.op_info[j].rd_enable)
                                        {
                                            if(send_pack.op_info[i].rs1_need_map && (send_pack.op_info[i].rs1 == send_pack.op_info[j].rd))
                                            {
                                                send_pack.op_info[i].rs1_phy = send_pack.op_info[j].rd_phy;
                                            }

                                            if(send_pack.op_info[i].rs2_need_map && (send_pack.op_info[i].rs2 == send_pack.op_info[j].rd))
                                            {
                                                send_pack.op_info[i].rs2_phy = send_pack.op_info[j].rd_phy;
                                            }
                                        }
                                    }
                                }

                                if(rev_pack.valid && rev_pack.predicted && rev_pack.checkpoint_id_valid)
                                {
                                    component::checkpoint_t cp;
                                    global_cp.clone(cp);
                                    component::checkpoint_t origin_cp = checkpoint_buffer->get_item(rev_pack.checkpoint_id);
                                    cp.global_history = origin_cp.global_history;
                                    cp.local_history = origin_cp.local_history;
                                    checkpoint_buffer->set_item_sync(rev_pack.checkpoint_id, cp);
                                }

                                /*if(rev_pack.valid && (rev_pack.op_unit == op_unit_t::bru))
                                {
                                    break;
                                }*/
                            }
                        }
                        else if(rat->get_free_phy_id(phy_reg_req_cnt, new_phy_reg_id) < phy_reg_req_cnt)
                        {
                            phy_regfile_full_add();

                            if(rob->get_free_space() < (rob_req_cnt))
                            {
                                rob_full_add();
                            }

                            assert(true);//phy_regfile is full
                            break;
                        }
                        else
                        {
                            rob_full_add();
                            assert(true);//is busy
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                rename_readreg_port->set(send_pack);
            }
        }
        else
        {
            rename_readreg_pack_t send_pack;
            memset(&send_pack, 0, sizeof(send_pack));
            rename_readreg_port->set(send_pack);
            busy = false;
            memset(&rev_pack, 0, sizeof(rev_pack));
        }

        return feedback_pack;
    }

    /*void rename::run(issue_feedback_pack_t issue_pack, commit_feedback_pack_t commit_feedback_pack)
    {
        rename_readreg_pack_t null_send_pack;
        bool stall = issue_pack.stall;
        
        memset(&null_send_pack, 0, sizeof(null_send_pack));

        if(!(commit_feedback_pack.enable && commit_feedback_pack.flush))
        {
            if(!stall)
            {
                this->rename_readreg_port->set(null_send_pack);//bubble

                if(!this->busy)
                {
                    if(!decode_rename_fifo->is_empty())
                    {
                        decode_rename_fifo->pop(&rev_pack);
                        this->busy = true;
                    }
                }

                if(this->busy)
                {
                    rename_readreg_pack_t send_pack;
                    memset(&send_pack, 0, sizeof(send_pack));
                    
                    uint32_t phy_reg_req_cnt = 0;
                    uint32_t rob_req_cnt = 0;
                    uint32_t new_phy_reg_id[RENAME_WIDTH];
                    component::rob_item_t rob_item[RENAME_WIDTH];
                    memset(rob_item, 0 ,sizeof(rob_item));
                    uint32_t used_phy_reg_cnt = 0;

                    //generate base send_pack
                    for(uint32_t i = 0;i < RENAME_WIDTH;i++)
                    {
                        send_pack.op_info[i].enable = rev_pack.op_info[i].enable;
                        send_pack.op_info[i].value = rev_pack.op_info[i].value;
                        send_pack.op_info[i].valid = rev_pack.op_info[i].valid;
                        send_pack.op_info[i].pc = rev_pack.op_info[i].pc;
                        send_pack.op_info[i].imm = rev_pack.op_info[i].imm;
                        send_pack.op_info[i].has_exception = rev_pack.op_info[i].has_exception;
                        send_pack.op_info[i].exception_id = rev_pack.op_info[i].exception_id;
                        send_pack.op_info[i].exception_value = rev_pack.op_info[i].exception_value;
                        send_pack.op_info[i].predicted = rev_pack.op_info[i].predicted;
                        send_pack.op_info[i].predicted_jump = rev_pack.op_info[i].predicted_jump;
                        send_pack.op_info[i].predicted_next_pc = rev_pack.op_info[i].predicted_next_pc;
                        send_pack.op_info[i].checkpoint_id_valid = rev_pack.op_info[i].checkpoint_id_valid;
                        send_pack.op_info[i].checkpoint_id = rev_pack.op_info[i].checkpoint_id;
                        send_pack.op_info[i].rs1 = rev_pack.op_info[i].rs1;
                        send_pack.op_info[i].arg1_src = rev_pack.op_info[i].arg1_src;
                        send_pack.op_info[i].rs1_need_map = rev_pack.op_info[i].rs1_need_map;
                        send_pack.op_info[i].rs2 = rev_pack.op_info[i].rs2;
                        send_pack.op_info[i].arg2_src = rev_pack.op_info[i].arg2_src;
                        send_pack.op_info[i].rs2_need_map = rev_pack.op_info[i].rs2_need_map;
                        send_pack.op_info[i].rd = rev_pack.op_info[i].rd;
                        send_pack.op_info[i].rd_enable = rev_pack.op_info[i].rd_enable;
                        send_pack.op_info[i].need_rename = rev_pack.op_info[i].need_rename;
                        send_pack.op_info[i].csr = rev_pack.op_info[i].csr;
                        send_pack.op_info[i].op = rev_pack.op_info[i].op;
                        send_pack.op_info[i].op_unit = rev_pack.op_info[i].op_unit;
                        
                        memcpy(&send_pack.op_info[i].sub_op, &rev_pack.op_info[i].sub_op, sizeof(rev_pack.op_info[i].sub_op));

                        //count new physical registers requirement and rob requirement
                        if(rev_pack.op_info[i].enable && rev_pack.op_info[i].valid && rev_pack.op_info[i].need_rename)
                        {
                            phy_reg_req_cnt++;
                        }

                        if(rev_pack.op_info[i].enable)
                        {
                            rob_req_cnt++;
                        }

                        //start to rename
                        if((rat->get_free_phy_id(phy_reg_req_cnt, new_phy_reg_id) >= phy_reg_req_cnt) && rob->get_free_space() >= (rob_req_cnt))
                        {
                            //generate rob items
                            if(rev_pack.op_info[i].enable)
                            {
                                if(rev_pack.op_info[i].valid)
                                {
                                    if(rev_pack.op_info[i].need_rename)
                                    {
                                        rob_item[i].old_phy_reg_id_valid = true;
                                        assert(rat->get_phy_id(rev_pack.op_info[i].rd, &rob_item[i].old_phy_reg_id));
                                        send_pack.op_info[i].rd_phy = new_phy_reg_id[used_phy_reg_cnt++];
                                        rat->set_map_sync(rev_pack.op_info[i].rd, send_pack.op_info[i].rd_phy);

                                        if(i > 0)
                                        {
                                            //old_phy_reg_id feedback
                                            for(auto j = i - 1;;j--)
                                            {
                                                if(rev_pack.op_info[j].enable && rev_pack.op_info[j].valid && rev_pack.op_info[j].need_rename)
                                                {
                                                    if(rev_pack.op_info[i].rd == rev_pack.op_info[j].rd)
                                                    {
                                                        rob_item[i].old_phy_reg_id = send_pack.op_info[j].rd_phy;
                                                    }
                                                }

                                                if(j == 0)
                                                {
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        rob_item[i].old_phy_reg_id_valid = false;
                                        rob_item[i].old_phy_reg_id = 0;
                                    }
                                }

                                rob_item[i].finish = false;
                                //fill rob item
                                rob_item[i].new_phy_reg_id = send_pack.op_info[i].rd_phy;
                                rob_item[i].pc = rev_pack.op_info[i].pc;
                                rob_item[i].inst_value = rev_pack.op_info[i].value;
                                rob_item[i].has_exception = rev_pack.op_info[i].has_exception;
                                rob_item[i].exception_id = rev_pack.op_info[i].exception_id;
                                rob_item[i].exception_value = rev_pack.op_info[i].exception_value;
                                //write to rob
                                assert(rob->get_new_id(&send_pack.op_info[i].rob_id));
                                uint32_t t;
                                assert(rob->push(rob_item[i], &t));

                                //start to map source registers
                                if(rev_pack.op_info[i].rs1_need_map)
                                {
                                    assert(rat->get_phy_id(rev_pack.op_info[i].rs1, &send_pack.op_info[i].rs1_phy));
                                }

                                if(rev_pack.op_info[i].rs2_need_map)
                                {
                                    assert(rat->get_phy_id(rev_pack.op_info[i].rs2, &send_pack.op_info[i].rs2_phy));
                                }

                                //source registers feedback
                                if(rev_pack.op_info[i].valid)
                                {
                                    for(uint32_t j = 0;j < i;j++)
                                    {
                                        if(rev_pack.op_info[j].enable && rev_pack.op_info[j].valid && rev_pack.op_info[j].rd_enable)
                                        {
                                            if(rev_pack.op_info[i].rs1_need_map && (rev_pack.op_info[i].rs1 == rev_pack.op_info[j].rd))
                                            {
                                                send_pack.op_info[i].rs1_phy = send_pack.op_info[j].rd_phy;
                                            }

                                            if(rev_pack.op_info[i].rs2_need_map && (rev_pack.op_info[i].rs2 == rev_pack.op_info[j].rd))
                                            {
                                                send_pack.op_info[i].rs2_phy = send_pack.op_info[j].rd_phy;
                                            }
                                        }
                                    }
                                }
                            }

                            rename_readreg_port->set(send_pack);
                            this->busy = false;
                        }
                        else if(rat->get_free_phy_id(phy_reg_req_cnt, new_phy_reg_id) < phy_reg_req_cnt)
                        {
                            phy_regfile_full_add();

                            if(rob->get_free_space() < (rob_req_cnt))
                            {
                                rob_full_add();
                            }

                            assert(true);//phy_regfile is full
                        }
                        else
                        {
                            rob_full_add();
                            assert(true);//is busy
                        }
                    }
                }
            }
        }
        else
        {
            rename_readreg_pack_t send_pack;
            memset(&send_pack, 0, sizeof(send_pack));
            rename_readreg_port->set(send_pack);
            busy = false;
            memset(&rev_pack, 0, sizeof(rev_pack));
        }
    }*/

    /*void rename::run(issue_feedback_pack_t issue_pack, commit_feedback_pack_t commit_feedback_pack)
    {
        rename_readreg_pack_t null_send_pack;
        bool stall = issue_pack.stall;
        
        memset(&null_send_pack, 0, sizeof(null_send_pack));

        if(!(commit_feedback_pack.enable && commit_feedback_pack.flush))
        {
            if(!stall)
            {
                this->rename_readreg_port->set(null_send_pack);//bubble

                if(!this->busy)
                {
                    if(!decode_rename_fifo->is_empty())
                    {
                        decode_rename_fifo->pop(&rev_pack);
                        this->busy = true;
                    }
                }

                if(this->busy)
                {
                    rename_readreg_pack_t send_pack;
                    memset(&send_pack, 0, sizeof(send_pack));

                    //generate base send_pack
                    for(uint32_t i = 0;i < RENAME_WIDTH;i++)
                    {
                        send_pack.op_info[i].enable = rev_pack.op_info[i].enable;
                        send_pack.op_info[i].value = rev_pack.op_info[i].value;
                        send_pack.op_info[i].valid = rev_pack.op_info[i].valid;
                        send_pack.op_info[i].pc = rev_pack.op_info[i].pc;
                        send_pack.op_info[i].imm = rev_pack.op_info[i].imm;
                        send_pack.op_info[i].has_exception = rev_pack.op_info[i].has_exception;
                        send_pack.op_info[i].exception_id = rev_pack.op_info[i].exception_id;
                        send_pack.op_info[i].exception_value = rev_pack.op_info[i].exception_value;
                        send_pack.op_info[i].predicted = rev_pack.op_info[i].predicted;
                        send_pack.op_info[i].predicted_jump = rev_pack.op_info[i].predicted_jump;
                        send_pack.op_info[i].predicted_next_pc = rev_pack.op_info[i].predicted_next_pc;
                        send_pack.op_info[i].checkpoint_id_valid = rev_pack.op_info[i].checkpoint_id_valid;
                        send_pack.op_info[i].checkpoint_id = rev_pack.op_info[i].checkpoint_id;
                        send_pack.op_info[i].rs1 = rev_pack.op_info[i].rs1;
                        send_pack.op_info[i].arg1_src = rev_pack.op_info[i].arg1_src;
                        send_pack.op_info[i].rs1_need_map = rev_pack.op_info[i].rs1_need_map;
                        send_pack.op_info[i].rs2 = rev_pack.op_info[i].rs2;
                        send_pack.op_info[i].arg2_src = rev_pack.op_info[i].arg2_src;
                        send_pack.op_info[i].rs2_need_map = rev_pack.op_info[i].rs2_need_map;
                        send_pack.op_info[i].rd = rev_pack.op_info[i].rd;
                        send_pack.op_info[i].rd_enable = rev_pack.op_info[i].rd_enable;
                        send_pack.op_info[i].need_rename = rev_pack.op_info[i].need_rename;
                        send_pack.op_info[i].csr = rev_pack.op_info[i].csr;
                        send_pack.op_info[i].op = rev_pack.op_info[i].op;
                        send_pack.op_info[i].op_unit = rev_pack.op_info[i].op_unit;
                        
                        memcpy(&send_pack.op_info[i].sub_op, &rev_pack.op_info[i].sub_op, sizeof(rev_pack.op_info[i].sub_op));


                    }

                    //count new physical registers requirement and rob requirement
                    uint32_t phy_reg_req_cnt = 0;
                    uint32_t rob_req_cnt = 0;
            
                    for(uint32_t i = 0;i < RENAME_WIDTH;i++)
                    {
                        if(rev_pack.op_info[i].enable && rev_pack.op_info[i].valid && rev_pack.op_info[i].need_rename)
                        {
                            phy_reg_req_cnt++;
                        }

                        if(rev_pack.op_info[i].enable)
                        {
                            rob_req_cnt++;
                        }
                    }

                    //start to rename
                    uint32_t new_phy_reg_id[RENAME_WIDTH];

                    if((rat->get_free_phy_id(phy_reg_req_cnt, new_phy_reg_id) >= phy_reg_req_cnt) && rob->get_free_space() >= (rob_req_cnt))
                    {
                        //generate rob items&
                        component::rob_item_t rob_item[RENAME_WIDTH];
                        memset(rob_item, 0 ,sizeof(rob_item));

                        for(uint32_t i = 0;i < RENAME_WIDTH;i++)
                        {
                            if(rev_pack.op_info[i].enable)
                            {
                                if(rev_pack.op_info[i].valid)
                                {
                                    if(rev_pack.op_info[i].need_rename)
                                    {
                                        rob_item[i].old_phy_reg_id_valid = true;
                                        assert(rat->get_phy_id(rev_pack.op_info[i].rd, &rob_item[i].old_phy_reg_id));
                                    }
                                    else
                                    {
                                        rob_item[i].old_phy_reg_id_valid = false;
                                        rob_item[i].old_phy_reg_id = 0;
                                    }
                                }

                                rob_item[i].finish = false;
                            }
                        }

                        for(uint32_t i = 0,j = 0;i < RENAME_WIDTH;i++)
                        {
                            if(rev_pack.op_info[i].enable && rev_pack.op_info[i].valid && rev_pack.op_info[i].need_rename)
                            {
                                send_pack.op_info[i].rd_phy = new_phy_reg_id[j++];
                                rat->set_map_sync(rev_pack.op_info[i].rd, send_pack.op_info[i].rd_phy);
                            }
                        }

                        //old_phy_reg_id feedback
                        for(auto i = 1;i < RENAME_WIDTH;i++)
                        {
                            if(rev_pack.op_info[i].enable && rev_pack.op_info[i].valid && rev_pack.op_info[i].need_rename)
                            {
                                for(auto j = i - 1;j >= 0;j--)
                                {
                                    if(rev_pack.op_info[j].enable && rev_pack.op_info[j].valid && rev_pack.op_info[j].need_rename)
                                    {
                                        if(rev_pack.op_info[i].rd == rev_pack.op_info[j].rd)
                                        {
                                            rob_item[i].old_phy_reg_id = send_pack.op_info[j].rd_phy;
                                        }
                                    }
                                }
                            }
                        }

                        //fill rob item 
                        for(auto i = 0;i < RENAME_WIDTH;i++)
                        {
                            rob_item[i].new_phy_reg_id = send_pack.op_info[i].rd_phy;
                            rob_item[i].pc = rev_pack.op_info[i].pc;
                            rob_item[i].inst_value = rev_pack.op_info[i].value;
                            rob_item[i].has_exception = rev_pack.op_info[i].has_exception;
                            rob_item[i].exception_id = rev_pack.op_info[i].exception_id;
                            rob_item[i].exception_value = rev_pack.op_info[i].exception_value;
                        }

                        //write to rob
                        for(uint32_t i = 0;i < RENAME_WIDTH;i++)
                        {
                            if(rev_pack.op_info[i].enable)
                            {
                                assert(rob->push(rob_item[i], &send_pack.op_info[i].rob_id));
                            }
                        }

                        //start to map source registers
                        for(uint32_t i = 0;i < RENAME_WIDTH;i++)
                        {
                            if(rev_pack.op_info[i].enable && rev_pack.op_info[i].valid)
                            {
                                if(rev_pack.op_info[i].rs1_need_map)
                                {
                                    assert(rat->get_phy_id(rev_pack.op_info[i].rs1, &send_pack.op_info[i].rs1_phy));
                                }

                                if(rev_pack.op_info[i].rs2_need_map)
                                {
                                    assert(rat->get_phy_id(rev_pack.op_info[i].rs2, &send_pack.op_info[i].rs2_phy));
                                }
                            }
                        }

                        //source registers feedback
                        if(rev_pack.op_info[0].enable && rev_pack.op_info[0].valid && rev_pack.op_info[0].rd_enable && rev_pack.op_info[1].enable && rev_pack.op_info[1].valid)
                        {
                            if(rev_pack.op_info[1].rs1_need_map && (rev_pack.op_info[1].rs1 == rev_pack.op_info[0].rd))
                            {
                                send_pack.op_info[1].rs1_phy = send_pack.op_info[0].rd_phy;
                            }

                            if(rev_pack.op_info[1].rs2_need_map && (rev_pack.op_info[1].rs2 == rev_pack.op_info[0].rd))
                            {
                                send_pack.op_info[1].rs2_phy = send_pack.op_info[0].rd_phy;
                            }
                        }

                        rename_readreg_port->set(send_pack);
                        this->busy = false;
                    }
                    else if(rat->get_free_phy_id(phy_reg_req_cnt, new_phy_reg_id) < phy_reg_req_cnt)
                    {
                        phy_regfile_full_add();

                        if(rob->get_free_space() < (rob_req_cnt))
                        {
                            rob_full_add();
                        }

                        assert(true);//phy_regfile is full
                    }
                    else
                    {
                        rob_full_add();
                        assert(true);//is busy
                    }
                }
            }
        }
        else
        {
            rename_readreg_pack_t send_pack;
            memset(&send_pack, 0, sizeof(send_pack));
            rename_readreg_port->set(send_pack);
            busy = false;
            memset(&rev_pack, 0, sizeof(rev_pack));
        }
    }*/
}