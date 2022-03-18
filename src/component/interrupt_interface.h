#pragma once
#include "common.h"
#include "csrfile.h"
#include "csr_all.h"

namespace component
{
    class interrupt_interface : public if_reset_t
    {
        private:
            enum class sync_request_type_t
            {
                set_ack
            };
            
            typedef struct sync_request_t
            {
                sync_request_type_t req;
                riscv_interrupt_t cause;
            }sync_request_t;
            
            std::queue<sync_request_t> sync_request_q;

            bool meip = false;
            bool msip = false;
            bool mtip = false;
            bool mei_ack = false;
            bool msi_ack = false;
            bool mti_ack = false;

            csrfile *csr_file;

        public:
            interrupt_interface(csrfile *csr_file)
            {
                this->csr_file = csr_file;
            }

            virtual void reset()
            {
                mei_ack = false;
                msi_ack = false;
                mti_ack = false;
            }

            bool get_cause(riscv_interrupt_t *cause)
            {
                csr::mie mie;
                csr::mstatus mstatus;
                mie.load(csr_file->read_sys(CSR_MIE));
                mstatus.load(csr_file->read_sys(CSR_MSTATUS));

                if(!mstatus.get_mie())
                {
                    return false;
                }

                if(meip && mie.get_meie())
                {
                    *cause = riscv_interrupt_t::machine_external;
                }
                else if(msip && mie.get_msie())
                {
                    *cause = riscv_interrupt_t::machine_software;
                }
                else if(mtip && mie.get_mtie())
                {
                    *cause = riscv_interrupt_t::machine_timer;
                }
                else
                {
                    return false;
                }

                return true;
            }

            bool has_interrupt()
            {
                csr::mie mie;
                csr::mstatus mstatus;
                mie.load(csr_file->read_sys(CSR_MIE));
                mstatus.load(csr_file->read_sys(CSR_MSTATUS));
                return mstatus.get_mie() && ((meip && mie.get_meie()) || (msip && mie.get_msie()) || (mtip && mie.get_mtie()));
            }

            void set_pending(riscv_interrupt_t cause, bool pending)
            {
                switch(cause)
                {
                    case riscv_interrupt_t::machine_external:
                        meip = pending;
                        break;

                    case riscv_interrupt_t::machine_software:
                        msip = pending;
                        break;

                    case riscv_interrupt_t::machine_timer:
                        mtip = pending;
                        break;
                }
            }

            void set_ack(riscv_interrupt_t cause)
            {
                switch(cause)
                {
                    case riscv_interrupt_t::machine_external:
                        mei_ack = true;
                        break;

                    case riscv_interrupt_t::machine_software:
                        msi_ack = true;
                        break;

                    case riscv_interrupt_t::machine_timer:
                        mti_ack = true;
                        break;
                }
            }

            void set_ack_sync(riscv_interrupt_t cause)
            {
                sync_request_t t_req;
                t_req.req = sync_request_type_t::set_ack;
                t_req.cause = cause;
                sync_request_q.push(t_req);
            }

            bool get_ack(riscv_interrupt_t cause)
            {
                switch(cause)
                {
                    case riscv_interrupt_t::machine_external:
                        return mei_ack;

                    case riscv_interrupt_t::machine_software:
                        return msi_ack;

                    case riscv_interrupt_t::machine_timer:
                        return mti_ack;

                    default:
                        return false;
                }
            }

            void run()
            {
                mei_ack = false;
                msi_ack = false;
                mti_ack = false;
                csr::mip mip;
                mip.load(csr_file->read_sys(CSR_MIP));
                mip.set_meip(meip);
                mip.set_msip(msip);
                mip.set_mtip(mtip);
                csr_file->write_sys_sync(CSR_MIP, mip.get_value());
            }

            void sync()
            {
                while(!sync_request_q.empty())
                {
                    auto item = sync_request_q.front();
                    sync_request_q.pop();
                    
                    switch(item.req)
                    {
                        case sync_request_type_t::set_ack:
                            set_ack(item.cause);
                            break;
                    }
                }
            }
    };
}