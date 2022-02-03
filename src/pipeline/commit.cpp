#include "common.h"
#include "commit.h"
#include "../component/csr_all.h"

namespace pipeline
{
	commit::commit(component::port<wb_commit_pack_t> *wb_commit_port, component::rat *rat, component::rob *rob, component::csrfile *csr_file)
	{
		this->wb_commit_port = wb_commit_port;
		this->rat = rat;
		this->rob = rob;
		this->csr_file = csr_file;
		this->cur_state = state_t::normal;
	}

    commit_feedback_pack_t commit::run()
	{
		commit_feedback_pack_t feedback_pack;
		memset(&feedback_pack, 0, sizeof(feedback_pack));

		if(this->cur_state == state_t::normal)
		{
			//handle output
			if(!rob->is_empty())
			{
				this->rob_item = rob->get_front();
				assert(rob->get_front_id(&this->rob_item_id));

				if(rob_item.finish)
				{
					rob->pop_sync();

					if(rob_item.old_phy_reg_id_valid)
					{
						rat->release_map_sync(rob_item.old_phy_reg_id);
					}

					if(rob_item.has_exception)
					{
						feedback_pack.enable = true;
						feedback_pack.flush = true;
						cur_state = state_t::flush;
					}
					else
					{
						feedback_pack.enable = rob->get_next_id(rob_item_id, &feedback_pack.next_handle_rob_id);
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
					rob_item.new_phy_reg_id = rev_pack.op_info[i].rd_phy;
					rob_item.finish = true;
					rob_item.pc = rev_pack.op_info[i].pc;
					rob_item.inst_value = rev_pack.op_info[i].value;
					rob_item.has_exception = rev_pack.op_info[i].has_exception;
					rob_item.exception_id = rev_pack.op_info[i].exception_id;
					rob_item.exception_value = rev_pack.op_info[i].exception_value;
					rob->set_item_sync(rev_pack.op_info[i].rob_id, rob_item);
				}
			}
		}
		else//flush
		{
			if(rob->is_empty() || (rob->get_size() == 1))
			{
				feedback_pack.enable = true;
				feedback_pack.has_exception = true;
				csr_file->write_sys_sync(CSR_MEPC, rob_item.pc);
				csr_file->write_sys_sync(CSR_MTVAL, rob_item.exception_value);
				csr_file->write_sys_sync(CSR_MCAUSE, static_cast<uint32_t>(rob_item.exception_id));
				feedback_pack.exception_pc = csr_file->read_sys(CSR_MTVEC);
				feedback_pack.flush = true;
				cur_state = state_t::normal;
			}
			else
			{
				feedback_pack.enable = true;
				feedback_pack.flush = true;
			}
			
			//flush rob and restore rat
			auto t_rob_item = rob->get_front();
			rob->pop_sync();

			if(t_rob_item.old_phy_reg_id_valid)
			{
				rat->restore_map_sync(t_rob_item.new_phy_reg_id, t_rob_item.old_phy_reg_id);
			}
		}

		return feedback_pack;
	}
}