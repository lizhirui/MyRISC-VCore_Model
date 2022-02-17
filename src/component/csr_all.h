#pragma once
#include "./csr/marchid.h"
#include "./csr/mcause.h"
#include "./csr/mconfigptr.h"
#include "./csr/mcounteren.h"
#include "./csr/mcycle.h"
#include "./csr/mepc.h"
#include "./csr/mhartid.h"
#include "./csr/mie.h"
#include "./csr/mimpid.h"
#include "./csr/minstret.h"
#include "./csr/mip.h"
#include "./csr/misa.h"
#include "./csr/mscratch.h"
#include "./csr/mstatus.h"
#include "./csr/mstatush.h"
#include "./csr/mtval.h"
#include "./csr/mtvec.h"
#include "./csr/mvendorid.h"
#include "./csr/pmpaddr.h"
#include "./csr/pmpcfg.h"
#include "./csr/charfifo.h"
#include "./csr/finish.h"

#define CSR_MVENDORID 0xf11
#define CSR_MARCHID 0xf12
#define CSR_MIMPID 0xf13
#define CSR_MHARTID 0xf14
#define CSR_MCONFIGPTR 0xf15
#define CSR_MSTATUS 0x300
#define CSR_MISA 0x301
#define CSR_MIE 0x304
#define CSR_MTVEC 0x305
#define CSR_MCOUNTEREN 0x306
#define CSR_MSTATUSH 0x310
#define CSR_MSCRATCH 0x340
#define CSR_MEPC 0x341
#define CSR_MCAUSE 0x342
#define CSR_MTVAL 0x343
#define CSR_MIP 0x344
#define CSR_CHARFIFO 0x800
#define CSR_FINISH 0x804