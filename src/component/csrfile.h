#pragma once
#include "common.h"
#include "csr_base.h"

namespace component
{
	class csrfile : if_print_t
	{
		private:
			typedef struct csr_item_t
			{
				bool readonly;
				std::shared_ptr<csr_base> csr;
			}csr_item_t;

			enum class sync_request_type_t
            {
                write_sys
            };

            typedef struct sync_request_t
            {
                sync_request_type_t req;
                uint32_t arg1;
                uint32_t arg2;
            }sync_request_t;

            std::queue<sync_request_t> sync_request_q;

			std::unordered_map<uint32_t, csr_item_t> csr_map_table;

			static bool csr_out_list_cmp(const std::pair<std::string, std::string> &a, const std::pair<std::string, std::string> &b)
			{
				if(std::isdigit(a.first[a.first.length() - 1]) && std::isdigit(b.first[b.first.length() - 1]))
				{
					size_t a_index = 0;
					size_t b_index = 0;

					for(auto i = a.first.length() - 1;i >= 0;i--)
					{
						if(!std::isdigit(a.first[i]))
						{
							break;
						}
						else
						{
							a_index = i;
						}
					}

					for(auto i = b.first.length() - 1;i >= 0;i--)
					{
						if(!std::isdigit(b.first[i]))
						{
							break;
						}
						else
						{
							b_index = i;
						}
					}

					auto a_left = a.first.substr(0, a_index);
					auto a_right = a.first.substr(a_index);
					auto b_left = b.first.substr(0, b_index);
					auto b_right = b.first.substr(b_index);
					auto a_int = 0;
					auto b_int = 0;
					std::istringstream a_stream(a_right);
					std::istringstream b_stream(b_right);
					a_stream >> a_int;
					b_stream >> b_int;
					return (a_left < b_left) || ((a_left == b_left) && (a_int < b_int));
				}
				else
				{
					return a.first < b.first;
				}
			}

		public:
			void map(uint32_t addr, bool readonly, std::shared_ptr<csr_base> csr)
			{
				assert(csr_map_table.find(addr) == csr_map_table.end());
				csr_item_t t_item;
				t_item.readonly = readonly;
				t_item.csr = csr;
				csr_map_table[addr] = t_item;
			}

			void write_sys(uint32_t addr, uint32_t value)
			{
				assert(!(csr_map_table.find(addr) == csr_map_table.end()));
				csr_map_table[addr].csr->write(value);
			}

			void write_sys_sync(uint32_t addr, uint32_t value)
			{
				sync_request_t t_req;

				t_req.req = sync_request_type_t::write_sys;
				t_req.arg1 = addr;
				t_req.arg2 = value;
			}

			uint32_t read_sys(uint32_t addr)
			{
				assert(!(csr_map_table.find(addr) == csr_map_table.end()));
				return csr_map_table[addr].csr->read();
			}

			bool write(uint32_t addr, uint32_t value)
			{
				if(csr_map_table.find(addr) == csr_map_table.end())
				{
					return false;
				}

				if(csr_map_table[addr].readonly)
				{
					return false;
				}

				csr_map_table[addr].csr->write(value);
				return true;
			}

			bool read(uint32_t addr, uint32_t *value)
			{
				if(csr_map_table.find(addr) == csr_map_table.end())
				{
					return false;
				}

				*value = csr_map_table[addr].csr->read();
				return true;
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
                        case sync_request_type_t::write_sys:
                            write_sys(t_req.arg1, t_req.arg2);
                            break;
                    }
                }
            }

			virtual void print(std::string indent)
			{
				std::cout << indent << "CSR List:" << std::endl;
				std::vector<std::pair<std::string, std::string>> out_list;

				for(auto iter = csr_map_table.begin();iter != csr_map_table.end();iter++)
				{
					std::ostringstream stream;
					stream << indent << std::setw(15) << iter->second.csr->get_name() << "\t[0x" << fillzero(3) << outhex(iter->first) << ", " << (iter->second.readonly ? "RO" : "RW") << "] = 0x" << fillzero(8) << outhex(iter->second.csr->read()) << std::endl;
					out_list.push_back(std::pair<std::string, std::string>(iter->second.csr->get_name(), stream.str()));
				}

				std::sort(out_list.begin(), out_list.end(), csr_out_list_cmp);

				for(auto iter = out_list.begin();iter != out_list.end();iter++)
				{
					std::cout << iter->second;
				}
			}
	};
}