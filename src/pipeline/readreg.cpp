#include "common.h"
#include "config.h"
#include "pipeline_common.h"
#include "readreg.h"
#include "issue.h"
#include "wb.h"

namespace pipeline
{
    readreg::readreg(component::port<rename_readreg_pack_t> *rename_readreg_port, component::port<readreg_issue_pack_t> *readreg_issue_port, component::regfile<phy_regfile_item_t> *phy_regfile, component::checkpoint_buffer *checkpoint_buffer, component::rat *rat)
    {
        this->rename_readreg_port = rename_readreg_port;
        this->readreg_issue_port = readreg_issue_port;
        this->phy_regfile = phy_regfile;
        this->checkpoint_buffer = checkpoint_buffer;
        this->rat = rat;
    }

    void readreg::run(issue_feedback_pack_t issue_pack, execute_feedback_pack_t execute_feedback_pack, wb_feedback_pack_t wb_feedback_pack, commit_feedback_pack_t commit_feedback_pack)
    {
        rename_readreg_pack_t rev_pack;
        bool stall = issue_pack.stall;       

        if(!(commit_feedback_pack.enable && commit_feedback_pack.flush))
        {
            if(!stall)
            {   
                rev_pack = this->rename_readreg_port->get();

                readreg_issue_pack_t send_pack;
                memset(&send_pack, 0, sizeof(send_pack));

                //generate base send_pack
                for(uint32_t i = 0;i < READREG_WIDTH;i++)
                {
                    send_pack.op_info[i].enable = rev_pack.op_info[i].enable;
                    send_pack.op_info[i].value = rev_pack.op_info[i].value;
                    send_pack.op_info[i].valid = rev_pack.op_info[i].valid;
                    send_pack.op_info[i].rob_id = rev_pack.op_info[i].rob_id;
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
                    send_pack.op_info[i].rs1_phy = rev_pack.op_info[i].rs1_phy;

                    send_pack.op_info[i].rs2 = rev_pack.op_info[i].rs2;
                    send_pack.op_info[i].arg2_src = rev_pack.op_info[i].arg2_src;
                    send_pack.op_info[i].rs2_need_map = rev_pack.op_info[i].rs2_need_map;
                    send_pack.op_info[i].rs2_phy = rev_pack.op_info[i].rs2_phy;

                    send_pack.op_info[i].rd = rev_pack.op_info[i].rd;
                    send_pack.op_info[i].rd_enable = rev_pack.op_info[i].rd_enable;
                    send_pack.op_info[i].need_rename = rev_pack.op_info[i].need_rename;
                    send_pack.op_info[i].rd_phy = rev_pack.op_info[i].rd_phy;

                    send_pack.op_info[i].csr = rev_pack.op_info[i].csr;
                    send_pack.op_info[i].op = rev_pack.op_info[i].op;
                    send_pack.op_info[i].op_unit = rev_pack.op_info[i].op_unit;
                    memcpy(&send_pack.op_info[i].sub_op, &rev_pack.op_info[i].sub_op, sizeof(rev_pack.op_info[i].sub_op));
                }

                for(uint32_t i = 0;i < READREG_WIDTH;i++)
                {
                    if(rev_pack.op_info[i].enable && rev_pack.op_info[i].valid)
                    {
                        if(rev_pack.op_info[i].rs1_need_map)
                        {
                            auto reg_item = phy_regfile->read(rev_pack.op_info[i].rs1_phy);
                            auto reg_data_valid = phy_regfile->read_data_valid(rev_pack.op_info[i].rs1_phy);

                            if(reg_data_valid)
                            {
                                send_pack.op_info[i].src1_value = reg_item.value;
                                send_pack.op_info[i].src1_loaded = true;
                            }
                            else
                            {
                                for(auto j = 0;j < EXECUTE_UNIT_NUM;j++)
                                {
                                    if(execute_feedback_pack.channel[j].enable && (rev_pack.op_info[i].rs1_phy == execute_feedback_pack.channel[j].phy_id))
                                    {
                                        send_pack.op_info[i].src1_value = execute_feedback_pack.channel[j].value;
                                        send_pack.op_info[i].src1_loaded = true;
                                        break;
                                    }
                                    else if(wb_feedback_pack.channel[j].enable && (rev_pack.op_info[i].rs1_phy == wb_feedback_pack.channel[j].phy_id))
                                    {
                                        send_pack.op_info[i].src1_value = wb_feedback_pack.channel[j].value;
                                        send_pack.op_info[i].src1_loaded = true;
                                        break;
                                    }
                                }
                            }
                        }
                        else if(rev_pack.op_info[i].arg1_src == arg_src_t::imm)
                        {
                            send_pack.op_info[i].src1_value = rev_pack.op_info[i].imm;
                            send_pack.op_info[i].src1_loaded = true;
                        }
                        else
                        {
                            send_pack.op_info[i].src1_value = 0;
                            send_pack.op_info[i].src1_loaded = true;
                        }

                        if(rev_pack.op_info[i].rs2_need_map)
                        {
                            auto reg_item = phy_regfile->read(rev_pack.op_info[i].rs2_phy);
                            auto reg_data_valid = phy_regfile->read_data_valid(rev_pack.op_info[i].rs2_phy);

                            if(reg_data_valid)
                            {
                                send_pack.op_info[i].src2_value = reg_item.value;
                                send_pack.op_info[i].src2_loaded = true;
                            }
                            else
                            {
                                for(auto j = 0;j < EXECUTE_UNIT_NUM;j++)
                                {
                                    if(execute_feedback_pack.channel[j].enable && (rev_pack.op_info[i].rs2_phy == execute_feedback_pack.channel[j].phy_id))
                                    {
                                        send_pack.op_info[i].src2_value = execute_feedback_pack.channel[j].value;
                                        send_pack.op_info[i].src2_loaded = true;
                                        break;
                                    }
                                    else if(wb_feedback_pack.channel[j].enable && (rev_pack.op_info[i].rs2_phy == wb_feedback_pack.channel[j].phy_id))
                                    {
                                        send_pack.op_info[i].src2_value = wb_feedback_pack.channel[j].value;
                                        send_pack.op_info[i].src2_loaded = true;
                                        break;
                                    }
                                }
                            }
                        }
                        else if(rev_pack.op_info[i].arg2_src == arg_src_t::imm)
                        {
                            send_pack.op_info[i].src2_value = rev_pack.op_info[i].imm;
                            send_pack.op_info[i].src2_loaded = true;
                        }
                        else
                        {
                            send_pack.op_info[i].src2_value = 0;
                            send_pack.op_info[i].src2_loaded = true;
                        }

                        /*if(rev_pack.op_info[i].predicted && rev_pack.op_info[i].checkpoint_id_valid)
                        {
                            component::checkpoint_t cp;
                            rat->save(cp);
                            component::checkpoint_t t_cp;
                            t_cp = checkpoint_buffer->get_item(rev_pack.op_info[i].checkpoint_id);
                            //assert((memcmp(&cp.rat_phy_map_table_valid, &t_cp.rat_phy_map_table_valid, sizeof(t_cp.rat_phy_map_table_valid)) == 0) && (memcmp(&cp.rat_phy_map_table_visible, &t_cp.rat_phy_map_table_visible, sizeof(t_cp.rat_phy_map_table_visible)) == 0));
                            //checkpoint_buffer->set_item_sync(rev_pack.op_info[i].checkpoint_id, cp);
                        }*/
                    }
                    else
                    {
                        //pass issue stage for invalid items
                        send_pack.op_info[i].src1_loaded = true;
                        send_pack.op_info[i].src2_loaded = true;
                    }
                }

                readreg_issue_port->set(send_pack);
            }
        }
        else
        {
            readreg_issue_pack_t send_pack;
            memset(&send_pack, 0, sizeof(send_pack));
            readreg_issue_port->set(send_pack);
        }
    }
}
