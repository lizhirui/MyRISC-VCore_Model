#pragma once
#include "common.h"
#include "../csr_base.h"

namespace component
{
	namespace csr
	{
		class charfifo : public csr_base
		{
			private:
				boost::lockfree::spsc_queue<char, boost::lockfree::capacity<1024>> *send_fifo;

			public:
				charfifo(boost::lockfree::spsc_queue<char, boost::lockfree::capacity<1024>> *send_fifo) : csr_base("charfifo", 0x00000000)
				{
					this->send_fifo = send_fifo;
				}

				virtual uint32_t filter(uint32_t value)
				{
					while(!send_fifo->push((char)(value & 0xff)));
					return 0;
				}
		};
	}
}