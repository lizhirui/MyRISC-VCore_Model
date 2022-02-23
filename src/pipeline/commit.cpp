#include "common.h"
#include "commit.h"
#include "../component/csr_all.h"

namespace pipeline
{
	commit::commit(component::port<wb_commit_pack_t> *wb_commit_port, component::rat *rat, component::rob *rob, component::csrfile *csr_file, component::regfile<phy_regfile_item_t> *phy_regfile)
	{
		this->wb_commit_port = wb_commit_port;
		this->rat = rat;
		this->rob = rob;
		this->csr_file = csr_file;
		this->phy_regfile = phy_regfile;
		this->cur_state = state_t::normal;
	}

	void commit::reset()
	{
		this->cur_state = state_t::normal;
		this->rob_item_id = 0;
		this->restore_rob_item_id = 0;
	}

    commit_feedback_pack_t commit::run()
	{
		commit_feedback_pack_t feedback_pack;
		phy_regfile_item_t default_phy_reg_item;
		memset(&feedback_pack, 0, sizeof(feedback_pack));
		memset(&default_phy_reg_item, 0, sizeof(default_phy_reg_item));

		if(this->cur_state == state_t::normal)
		{
			//handle output
			if(!rob->is_empty())
			{
				assert(rob->get_front_id(&this->rob_item_id));
				feedback_pack.enable = true;
				feedback_pack.next_handle_rob_id = this->rob_item_id;
				feedback_pack.next_handle_rob_id_valid = true;

				for(auto i = 0;i < COMMIT_WIDTH;i++)
				{
					this->rob_item = rob->get_item(this->rob_item_id);

					if(rob_item.finish)
					{
						feedback_pack.next_handle_rob_id_valid = rob->get_next_id(this->rob_item_id, &feedback_pack.next_handle_rob_id);
						feedback_pack.committed_rob_id_valid[i] = true;
						feedback_pack.committed_rob_id[i] = this->rob_item_id;

						if(rob_item.has_exception)
						{
							assert(rob->get_tail_id(&this->restore_rob_item_id));
							feedback_pack.enable = true;
							feedback_pack.flush = true;
							cur_state = state_t::flush;
							break;
						}
						else
						{
							rob->pop_sync();

							if(rob_item.old_phy_reg_id_valid)
							{
								rat->release_map_sync(rob_item.old_phy_reg_id);
								phy_regfile->write_sync(rob_item.old_phy_reg_id, default_phy_reg_item);
								rat->commit_map_sync(rob_item.new_phy_reg_id);
							}

							feedback_pack.enable = rob->get_next_id(rob_item_id, &feedback_pack.next_handle_rob_id);
							rob->set_committed(true);
							rob->add_commit_num(1);
						}
					}

					if(!rob->get_next_id(this->rob_item_id, &this->rob_item_id))
					{
						break;
					}
				}
			}

			//handle input
			auto rev_pack = wb_commit_port->get();

			for(auto i = 0;i < EXECUTE_UNIT_NUM;i++)
			{
				if(rev_pack.op_info[i].enable)
				{
					auto rob_item = rob->get_item(rev_pack.op_info[i].rob_id);
					//rob_item.new_phy_reg_id = rev_pack.op_info[i].rd_phy;
					rob_item.finish = true;
					/*rob_item.pc = rev_pack.op_info[i].pc;
					rob_item.inst_value = rev_pack.op_info[i].value;*/
					rob_item.has_exception = rev_pack.op_info[i].has_exception;
					rob_item.exception_id = rev_pack.op_info[i].exception_id;
					rob_item.exception_value = rev_pack.op_info[i].exception_value;
					rob->set_item_sync(rev_pack.op_info[i].rob_id, rob_item);
				}
			}
		}
		else//flush
		{		
			//flush rob and restore rat
			auto t_rob_item = rob->get_item(this->restore_rob_item_id);
			
			if(t_rob_item.old_phy_reg_id_valid)
			{
				rat->restore_map_sync(t_rob_item.new_phy_reg_id, t_rob_item.old_phy_reg_id);
				phy_regfile->write_sync(t_rob_item.new_phy_reg_id, default_phy_reg_item);
			}

			if(rob->get_prev_id(this->restore_rob_item_id, &this->restore_rob_item_id) && (this->restore_rob_item_id != this->rob_item_id))
			{
				feedback_pack.enable = true;
				feedback_pack.flush = true;
			}
			else
			{
				rob->flush();
				feedback_pack.enable = true;
				feedback_pack.has_exception = true;
				csr_file->write_sys_sync(CSR_MEPC, rob_item.pc);
				csr_file->write_sys_sync(CSR_MTVAL, rob_item.exception_value);
				csr_file->write_sys_sync(CSR_MCAUSE, static_cast<uint32_t>(rob_item.exception_id));
				feedback_pack.exception_pc = csr_file->read_sys(CSR_MTVEC);
				feedback_pack.flush = true;
				cur_state = state_t::normal;
				rob->set_committed(true);
				rob->add_commit_num(1);
			}
		}

		return feedback_pack;
	}
}