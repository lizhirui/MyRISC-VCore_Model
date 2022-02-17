
test.elf:     file format elf32-littleriscv


Disassembly of section .text:

80000000 <_start>:
    addi x8, x8, 1
    addi x9, x9, 1
    addi x10, x10, 1
    j loop*/

    la sp, __stack_interrupt_cpu0
80000000:	0000b117          	auipc	sp,0xb
80000004:	00010113          	mv	sp,sp
    j main
80000008:	0f00006f          	j	800000f8 <main>

    li x1, 'h'
8000000c:	06800093          	li	ra,104
    csrw 0x800, x1
80000010:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
80000014:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
80000018:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
8000001c:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
80000020:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
80000024:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
80000028:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
8000002c:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
80000030:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
80000034:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
80000038:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
8000003c:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
80000040:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
80000044:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
80000048:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
8000004c:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
80000050:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
80000054:	80009073          	csrw	0x800,ra
    csrw 0x800, x1
80000058:	80009073          	csrw	0x800,ra

    li x2, 0
8000005c:	00000113          	li	sp,0
    li x3, 0
80000060:	00000193          	li	gp,0
    la x1, _exception
80000064:	00000097          	auipc	ra,0x0
80000068:	01c08093          	addi	ra,ra,28 # 80000080 <_exception>
    csrw mtvec, x1
8000006c:	30509073          	csrw	mtvec,ra

80000070 <loop>:

loop:
    ecall
80000070:	00000073          	ecall
    addi x3, x3, 1
80000074:	00118193          	addi	gp,gp,1
    j loop
80000078:	ff9ff06f          	j	80000070 <loop>
8000007c:	00000013          	nop

80000080 <_exception>:

    .align 4
_exception:
    addi x2, x2, 1
80000080:	00110113          	addi	sp,sp,1 # 8000b001 <__stack_interrupt_cpu0+0x1>
    csrr x4, mepc
80000084:	34102273          	csrr	tp,mepc
    addi x4, x4, 4
80000088:	00420213          	addi	tp,tp,4 # 4 <STACK_SIZE-0x3ffc>
    csrw mepc, x4
8000008c:	34121073          	csrw	mepc,tp
    mret
80000090:	30200073          	mret

80000094 <send_char>:

    .global send_char
send_char:
    csrw 0x800, a0
80000094:	80051073          	csrw	0x800,a0
80000098:	00008067          	ret
	...

800000a8 <send_str>:
void send_char(char ch);

void send_str(char *str)
{
800000a8:	fe010113          	addi	sp,sp,-32
800000ac:	00112e23          	sw	ra,28(sp)
800000b0:	00812c23          	sw	s0,24(sp)
800000b4:	02010413          	addi	s0,sp,32
800000b8:	fea42623          	sw	a0,-20(s0)
    while(*str)
800000bc:	01c0006f          	j	800000d8 <send_str+0x30>
    {
        send_char(*(str++));
800000c0:	fec42783          	lw	a5,-20(s0)
800000c4:	00178713          	addi	a4,a5,1
800000c8:	fee42623          	sw	a4,-20(s0)
800000cc:	0007c783          	lbu	a5,0(a5)
800000d0:	00078513          	mv	a0,a5
800000d4:	fc1ff0ef          	jal	ra,80000094 <send_char>
    while(*str)
800000d8:	fec42783          	lw	a5,-20(s0)
800000dc:	0007c783          	lbu	a5,0(a5)
800000e0:	fe0790e3          	bnez	a5,800000c0 <send_str+0x18>
    }
}
800000e4:	00000013          	nop
800000e8:	01c12083          	lw	ra,28(sp)
800000ec:	01812403          	lw	s0,24(sp)
800000f0:	02010113          	addi	sp,sp,32
800000f4:	00008067          	ret

800000f8 <main>:

void main()
{
800000f8:	ff010113          	addi	sp,sp,-16
800000fc:	00112623          	sw	ra,12(sp)
80000100:	00812423          	sw	s0,8(sp)
80000104:	01010413          	addi	s0,sp,16
    send_str("Hello World!\n");
80000108:	00000517          	auipc	a0,0x0
8000010c:	01050513          	addi	a0,a0,16 # 80000118 <etext>
80000110:	f99ff0ef          	jal	ra,800000a8 <send_str>
    while(1);
80000114:	0000006f          	j	80000114 <main+0x1c>

Disassembly of section .rodata:

80000118 <srodata>:
80000118:	6548                	flw	fa0,12(a0)
8000011a:	6c6c                	flw	fa1,92(s0)
8000011c:	6f57206f          	j	80073010 <__stack_interrupt_cpu0+0x68010>
80000120:	6c72                	flw	fs8,28(sp)
80000122:	2164                	fld	fs1,192(a0)
80000124:	000a                	c.slli	zero,0x2

Disassembly of section .osdebug:

80000128 <_osdebug_start>:
	...

Disassembly of section .bss:

80002128 <sbss>:
	...

Disassembly of section .debug_info:

00000000 <.debug_info>:
   0:	0095                	addi	ra,ra,5
   2:	0000                	unimp
   4:	0004                	0x4
   6:	0000                	unimp
   8:	0000                	unimp
   a:	0104                	addi	s1,sp,128
   c:	00000013          	nop
  10:	bf0c                	fsd	fa1,56(a4)
  12:	0000                	unimp
  14:	ca00                	sw	s0,16(a2)
	...
  22:	0000                	unimp
  24:	0200                	addi	s0,sp,256
  26:	0118                	addi	a4,sp,128
  28:	0000                	unimp
  2a:	0b01                	addi	s6,s6,0
  2c:	f806                	fsw	ft1,48(sp)
  2e:	0000                	unimp
  30:	2080                	fld	fs0,0(s1)
  32:	0000                	unimp
  34:	0100                	addi	s0,sp,128
  36:	4f9c                	lw	a5,24(a5)
  38:	0000                	unimp
  3a:	0300                	addi	s0,sp,384
  3c:	0114                	addi	a3,sp,128
  3e:	8000                	0x8000
  40:	0000004f          	fnmadd.s	ft0,ft0,ft0,ft0,rne
  44:	0104                	addi	s1,sp,128
  46:	055a                	slli	a0,a0,0x16
  48:	00011803          	lh	a6,0(sp)
  4c:	0080                	addi	s0,sp,64
  4e:	0500                	addi	s0,sp,640
  50:	000a                	c.slli	zero,0x2
  52:	0000                	unimp
  54:	0301                	addi	t1,t1,0
  56:	a806                	fsd	ft1,16(sp)
  58:	0000                	unimp
  5a:	5080                	lw	s0,32(s1)
  5c:	0000                	unimp
  5e:	0100                	addi	s0,sp,128
  60:	7f9c                	flw	fa5,56(a5)
  62:	0000                	unimp
  64:	0600                	addi	s0,sp,768
  66:	00727473          	csrrci	s0,0x7,4
  6a:	0301                	addi	t1,t1,0
  6c:	7f15                	lui	t5,0xfffe5
  6e:	0000                	unimp
  70:	0000                	unimp
  72:	0000                	unimp
  74:	0700                	addi	s0,sp,896
  76:	00d8                	addi	a4,sp,68
  78:	8000                	0x8000
  7a:	008c                	addi	a1,sp,64
  7c:	0000                	unimp
  7e:	0800                	addi	s0,sp,16
  80:	8504                	0x8504
  82:	0000                	unimp
  84:	0900                	addi	s0,sp,144
  86:	0801                	addi	a6,a6,0
  88:	0005                	c.nop	1
  8a:	0000                	unimp
  8c:	000a                	c.slli	zero,0x2
  8e:	0000                	unimp
  90:	0000                	unimp
  92:	0000                	unimp
  94:	0100                	addi	s0,sp,128
  96:	0601                	addi	a2,a2,0
  98:	2200                	fld	fs0,0(a2)
  9a:	0000                	unimp
  9c:	0200                	addi	s0,sp,256
  9e:	a400                	fsd	fs0,8(s0)
  a0:	0000                	unimp
  a2:	0400                	addi	s0,sp,512
  a4:	9101                	srli	a0,a0,0x20
  a6:	0000                	unimp
  a8:	0000                	unimp
  aa:	0000                	unimp
  ac:	9c80                	0x9c80
  ae:	0000                	unimp
  b0:	1d80                	addi	s0,sp,752
  b2:	0001                	nop
  b4:	ca00                	sw	s0,16(a2)
  b6:	0000                	unimp
  b8:	2b00                	fld	fs0,16(a4)
  ba:	0001                	nop
  bc:	0100                	addi	s0,sp,128
  be:	80              	Address 0x00000000000000be is out of bounds.


Disassembly of section .debug_abbrev:

00000000 <.debug_abbrev>:
   0:	1101                	addi	sp,sp,-32
   2:	2501                	jal	602 <STACK_SIZE-0x39fe>
   4:	130e                	slli	t1,t1,0x23
   6:	1b0e030b          	0x1b0e030b
   a:	550e                	lw	a0,224(sp)
   c:	10011117          	auipc	sp,0x10011
  10:	02000017          	auipc	zero,0x2000
  14:	012e                	slli	sp,sp,0xb
  16:	0e03193f 0b3b0b3a 	0xb3b0b3a0e03193f
  1e:	0b39                	addi	s6,s6,14
  20:	0111                	addi	sp,sp,4
  22:	0612                	slli	a2,a2,0x4
  24:	1840                	addi	s0,sp,52
  26:	01194297          	auipc	t0,0x1194
  2a:	03000013          	li	zero,48
  2e:	8289                	srli	a3,a3,0x2
  30:	0101                	addi	sp,sp,0
  32:	0111                	addi	sp,sp,4
  34:	1331                	addi	t1,t1,-20
  36:	0000                	unimp
  38:	8a04                	0x8a04
  3a:	0182                	c.slli64	gp
  3c:	0200                	addi	s0,sp,256
  3e:	9118                	0x9118
  40:	1842                	slli	a6,a6,0x30
  42:	0000                	unimp
  44:	2e05                	jal	374 <STACK_SIZE-0x3c8c>
  46:	3f01                	jal	ffffff56 <__stack_interrupt_cpu0+0x7fff4f56>
  48:	0319                	addi	t1,t1,6
  4a:	3a0e                	fld	fs4,224(sp)
  4c:	390b3b0b          	0x390b3b0b
  50:	1119270b          	0x1119270b
  54:	1201                	addi	tp,tp,-32
  56:	4006                	0x4006
  58:	9718                	0x9718
  5a:	1942                	slli	s2,s2,0x30
  5c:	1301                	addi	t1,t1,-32
  5e:	0000                	unimp
  60:	0506                	slli	a0,a0,0x1
  62:	0300                	addi	s0,sp,384
  64:	3a08                	fld	fa0,48(a2)
  66:	390b3b0b          	0x390b3b0b
  6a:	0213490b          	0x213490b
  6e:	07000017          	auipc	zero,0x7000
  72:	8289                	srli	a3,a3,0x2
  74:	0001                	nop
  76:	0111                	addi	sp,sp,4
  78:	1331                	addi	t1,t1,-20
  7a:	0000                	unimp
  7c:	0f08                	addi	a0,sp,912
  7e:	0b00                	addi	s0,sp,400
  80:	0013490b          	0x13490b
  84:	0900                	addi	s0,sp,144
  86:	0024                	addi	s1,sp,8
  88:	0b3e0b0b          	0xb3e0b0b
  8c:	00000e03          	lb	t3,0(zero) # 0 <STACK_SIZE-0x4000>
  90:	2e0a                	fld	ft8,128(sp)
  92:	3f00                	fld	fs0,56(a4)
  94:	3c19                	jal	fffffaaa <__stack_interrupt_cpu0+0x7fff4aaa>
  96:	6e19                	lui	t3,0x6
  98:	030e                	slli	t1,t1,0x3
  9a:	3a0e                	fld	fs4,224(sp)
  9c:	390b3b0b          	0x390b3b0b
  a0:	0000000b          	0xb
  a4:	1101                	addi	sp,sp,-32
  a6:	1000                	addi	s0,sp,32
  a8:	1106                	slli	sp,sp,0x21
  aa:	1201                	addi	tp,tp,-32
  ac:	0301                	addi	t1,t1,0
  ae:	1b0e                	slli	s6,s6,0x23
  b0:	250e                	fld	fa0,192(sp)
  b2:	130e                	slli	t1,t1,0x23
  b4:	0005                	c.nop	1
	...

Disassembly of section .debug_loc:

00000000 <.debug_loc>:
   0:	00a8                	addi	a0,sp,72
   2:	8000                	0x8000
   4:	00c0                	addi	s0,sp,68
   6:	8000                	0x8000
   8:	0001                	nop
   a:	c05a                	sw	s6,0(sp)
   c:	0000                	unimp
   e:	cc80                	sw	s0,24(s1)
  10:	0000                	unimp
  12:	0280                	addi	s0,sp,320
  14:	7800                	flw	fs0,48(s0)
  16:	cc6c                	sw	a1,92(s0)
  18:	0000                	unimp
  1a:	d780                	sw	s0,40(a5)
  1c:	0000                	unimp
  1e:	0180                	addi	s0,sp,192
  20:	5e00                	lw	s0,56(a2)
  22:	800000d7          	0x800000d7
  26:	00f0                	addi	a2,sp,76
  28:	8000                	0x8000
  2a:	0002                	c.slli64	zero
  2c:	6c78                	flw	fa4,92(s0)
  2e:	00f0                	addi	a2,sp,76
  30:	8000                	0x8000
  32:	00f4                	addi	a3,sp,76
  34:	8000                	0x8000
  36:	0002                	c.slli64	zero
  38:	0c72                	slli	s8,s8,0x1c
  3a:	00f4                	addi	a3,sp,76
  3c:	8000                	0x8000
  3e:	00f8                	addi	a4,sp,76
  40:	8000                	0x8000
  42:	0002                	c.slli64	zero
  44:	6c72                	flw	fs8,28(sp)
	...

Disassembly of section .debug_aranges:

00000000 <.debug_aranges>:
   0:	0024                	addi	s1,sp,8
   2:	0000                	unimp
   4:	0002                	c.slli64	zero
   6:	0000                	unimp
   8:	0000                	unimp
   a:	0004                	0x4
   c:	0000                	unimp
   e:	0000                	unimp
  10:	00a8                	addi	a0,sp,72
  12:	8000                	0x8000
  14:	0050                	addi	a2,sp,4
  16:	0000                	unimp
  18:	00f8                	addi	a4,sp,76
  1a:	8000                	0x8000
  1c:	0020                	addi	s0,sp,8
	...
  26:	0000                	unimp
  28:	001c                	0x1c
  2a:	0000                	unimp
  2c:	0002                	c.slli64	zero
  2e:	0099                	addi	ra,ra,6
  30:	0000                	unimp
  32:	0004                	0x4
  34:	0000                	unimp
  36:	0000                	unimp
  38:	0000                	unimp
  3a:	8000                	0x8000
  3c:	009c                	addi	a5,sp,64
	...

Disassembly of section .debug_ranges:

00000000 <.debug_ranges>:
   0:	00a8                	addi	a0,sp,72
   2:	8000                	0x8000
   4:	00f8                	addi	a4,sp,76
   6:	8000                	0x8000
   8:	00f8                	addi	a4,sp,76
   a:	8000                	0x8000
   c:	0118                	addi	a4,sp,128
   e:	8000                	0x8000
	...

Disassembly of section .debug_line:

00000000 <.debug_line>:
   0:	008d                	addi	ra,ra,3
   2:	0000                	unimp
   4:	00210003          	lb	zero,2(sp) # 1001100e <STACK_SIZE+0x1000d00e>
   8:	0000                	unimp
   a:	0101                	addi	sp,sp,0
   c:	000d0efb          	0xd0efb
  10:	0101                	addi	sp,sp,0
  12:	0101                	addi	sp,sp,0
  14:	0000                	unimp
  16:	0100                	addi	s0,sp,128
  18:	0000                	unimp
  1a:	7301                	lui	t1,0xfffe0
  1c:	6372                	flw	ft6,28(sp)
  1e:	0000                	unimp
  20:	616d                	addi	sp,sp,240
  22:	6e69                	lui	t3,0x1a
  24:	632e                	flw	ft6,200(sp)
  26:	0100                	addi	s0,sp,128
  28:	0000                	unimp
  2a:	0500                	addi	s0,sp,640
  2c:	0001                	nop
  2e:	0205                	addi	tp,tp,1
  30:	00a8                	addi	a0,sp,72
  32:	8000                	0x8000
  34:	0515                	addi	a0,a0,5
  36:	030a                	slli	t1,t1,0x2
  38:	0901                	addi	s2,s2,0
  3a:	0014                	0x14
  3c:	0501                	addi	a0,a0,0
  3e:	0318                	addi	a4,sp,384
  40:	0902                	c.slli64	s2
  42:	0004                	0x4
  44:	0501                	addi	a0,a0,0
  46:	0309                	addi	t1,t1,2
  48:	0900                	addi	s0,sp,144
  4a:	000c                	0xc
  4c:	0501                	addi	a0,a0,0
  4e:	097e030b          	0x97e030b
  52:	000c                	0xc
  54:	0501                	addi	a0,a0,0
  56:	030a                	slli	t1,t1,0x2
  58:	0900                	addi	s0,sp,144
  5a:	0008                	0x8
  5c:	0501                	addi	a0,a0,0
  5e:	0301                	addi	t1,t1,0
  60:	0904                	addi	s1,sp,144
  62:	0004                	0x4
  64:	0901                	addi	s2,s2,0
  66:	0014                	0x14
  68:	0100                	addi	s0,sp,128
  6a:	0501                	addi	a0,a0,0
  6c:	0001                	nop
  6e:	0205                	addi	tp,tp,1
  70:	00f8                	addi	a4,sp,76
  72:	8000                	0x8000
  74:	05010b03          	lb	s6,80(sp)
  78:	0305                	addi	t1,t1,1
  7a:	0901                	addi	s2,s2,0
  7c:	0010                	0x10
  7e:	0501                	addi	a0,a0,0
  80:	000a                	c.slli	zero,0x2
  82:	0402                	c.slli64	s0
  84:	0301                	addi	t1,t1,0
  86:	0901                	addi	s2,s2,0
  88:	000c                	0xc
  8a:	0901                	addi	s2,s2,0
  8c:	0004                	0x4
  8e:	0100                	addi	s0,sp,128
  90:	0c01                	addi	s8,s8,0
  92:	0001                	nop
  94:	0300                	addi	s0,sp,384
  96:	2400                	fld	fs0,8(s0)
  98:	0000                	unimp
  9a:	0100                	addi	s0,sp,128
  9c:	fb01                	bnez	a4,ffffffac <__stack_interrupt_cpu0+0x7fff4fac>
  9e:	0d0e                	slli	s10,s10,0x3
  a0:	0100                	addi	s0,sp,128
  a2:	0101                	addi	sp,sp,0
  a4:	0001                	nop
  a6:	0000                	unimp
  a8:	0001                	nop
  aa:	0100                	addi	s0,sp,128
  ac:	00637273          	csrrci	tp,0x6,6
  b0:	7300                	flw	fs0,32(a4)
  b2:	6174                	flw	fa3,68(a0)
  b4:	7472                	flw	fs0,60(sp)
  b6:	7075                	c.lui	zero,0xffffd
  b8:	532e                	lw	t1,232(sp)
  ba:	0100                	addi	s0,sp,128
  bc:	0000                	unimp
  be:	0000                	unimp
  c0:	0205                	addi	tp,tp,1
  c2:	0000                	unimp
  c4:	8000                	0x8000
  c6:	03012f03          	lw	t5,48(sp)
  ca:	0901                	addi	s2,s2,0
  cc:	0008                	0x8
  ce:	0301                	addi	t1,t1,0
  d0:	0902                	c.slli64	s2
  d2:	0004                	0x4
  d4:	0301                	addi	t1,t1,0
  d6:	0901                	addi	s2,s2,0
  d8:	0004                	0x4
  da:	0301                	addi	t1,t1,0
  dc:	0901                	addi	s2,s2,0
  de:	0004                	0x4
  e0:	0301                	addi	t1,t1,0
  e2:	0901                	addi	s2,s2,0
  e4:	0004                	0x4
  e6:	0301                	addi	t1,t1,0
  e8:	0901                	addi	s2,s2,0
  ea:	0004                	0x4
  ec:	0301                	addi	t1,t1,0
  ee:	0901                	addi	s2,s2,0
  f0:	0004                	0x4
  f2:	0301                	addi	t1,t1,0
  f4:	0901                	addi	s2,s2,0
  f6:	0004                	0x4
  f8:	0301                	addi	t1,t1,0
  fa:	0901                	addi	s2,s2,0
  fc:	0004                	0x4
  fe:	0301                	addi	t1,t1,0
 100:	0901                	addi	s2,s2,0
 102:	0004                	0x4
 104:	0301                	addi	t1,t1,0
 106:	0901                	addi	s2,s2,0
 108:	0004                	0x4
 10a:	0301                	addi	t1,t1,0
 10c:	0901                	addi	s2,s2,0
 10e:	0004                	0x4
 110:	0301                	addi	t1,t1,0
 112:	0901                	addi	s2,s2,0
 114:	0004                	0x4
 116:	0301                	addi	t1,t1,0
 118:	0901                	addi	s2,s2,0
 11a:	0004                	0x4
 11c:	0301                	addi	t1,t1,0
 11e:	0901                	addi	s2,s2,0
 120:	0004                	0x4
 122:	0301                	addi	t1,t1,0
 124:	0901                	addi	s2,s2,0
 126:	0004                	0x4
 128:	0301                	addi	t1,t1,0
 12a:	0901                	addi	s2,s2,0
 12c:	0004                	0x4
 12e:	0301                	addi	t1,t1,0
 130:	0901                	addi	s2,s2,0
 132:	0004                	0x4
 134:	0301                	addi	t1,t1,0
 136:	0901                	addi	s2,s2,0
 138:	0004                	0x4
 13a:	0301                	addi	t1,t1,0
 13c:	0901                	addi	s2,s2,0
 13e:	0004                	0x4
 140:	0301                	addi	t1,t1,0
 142:	0901                	addi	s2,s2,0
 144:	0004                	0x4
 146:	0301                	addi	t1,t1,0
 148:	0902                	c.slli64	s2
 14a:	0004                	0x4
 14c:	0301                	addi	t1,t1,0
 14e:	0901                	addi	s2,s2,0
 150:	0004                	0x4
 152:	0301                	addi	t1,t1,0
 154:	0901                	addi	s2,s2,0
 156:	0004                	0x4
 158:	0301                	addi	t1,t1,0
 15a:	0901                	addi	s2,s2,0
 15c:	0008                	0x8
 15e:	0301                	addi	t1,t1,0
 160:	00040903          	lb	s2,0(s0)
 164:	0301                	addi	t1,t1,0
 166:	0901                	addi	s2,s2,0
 168:	0004                	0x4
 16a:	0301                	addi	t1,t1,0
 16c:	0901                	addi	s2,s2,0
 16e:	0004                	0x4
 170:	0301                	addi	t1,t1,0
 172:	0904                	addi	s1,sp,144
 174:	0008                	0x8
 176:	0301                	addi	t1,t1,0
 178:	0901                	addi	s2,s2,0
 17a:	0004                	0x4
 17c:	0301                	addi	t1,t1,0
 17e:	0901                	addi	s2,s2,0
 180:	0004                	0x4
 182:	0301                	addi	t1,t1,0
 184:	0901                	addi	s2,s2,0
 186:	0004                	0x4
 188:	0301                	addi	t1,t1,0
 18a:	0901                	addi	s2,s2,0
 18c:	0004                	0x4
 18e:	0301                	addi	t1,t1,0
 190:	0904                	addi	s1,sp,144
 192:	0004                	0x4
 194:	0301                	addi	t1,t1,0
 196:	0901                	addi	s2,s2,0
 198:	0004                	0x4
 19a:	0901                	addi	s2,s2,0
 19c:	0004                	0x4
 19e:	0100                	addi	s0,sp,128
 1a0:	01              	Address 0x00000000000001a0 is out of bounds.


Disassembly of section .debug_str:

00000000 <.debug_str>:
   0:	646e6573          	csrrsi	a0,0x646,28
   4:	635f 6168 0072      	0x726168635f
   a:	646e6573          	csrrsi	a0,0x646,28
   e:	735f 7274 4700      	0x47007274735f
  14:	554e                	lw	a0,240(sp)
  16:	4320                	lw	s0,64(a4)
  18:	3731                	jal	ffffff24 <__stack_interrupt_cpu0+0x7fff4f24>
  1a:	3820                	fld	fs0,112(s0)
  1c:	332e                	fld	ft6,232(sp)
  1e:	302e                	fld	ft0,232(sp)
  20:	2d20                	fld	fs0,88(a0)
  22:	636d                	lui	t1,0x1b
  24:	6f6d                	lui	t5,0x1b
  26:	6564                	flw	fs1,76(a0)
  28:	3d6c                	fld	fa1,248(a0)
  2a:	656d                	lui	a0,0x1b
  2c:	6164                	flw	fs1,68(a0)
  2e:	796e                	flw	fs2,248(sp)
  30:	2d20                	fld	fs0,88(a0)
  32:	616d                	addi	sp,sp,240
  34:	6372                	flw	ft6,28(sp)
  36:	3d68                	fld	fa0,248(a0)
  38:	7672                	flw	fa2,60(sp)
  3a:	6d693233          	0x6d693233
  3e:	2d20                	fld	fs0,88(a0)
  40:	616d                	addi	sp,sp,240
  42:	6962                	flw	fs2,24(sp)
  44:	693d                	lui	s2,0xf
  46:	706c                	flw	fa1,100(s0)
  48:	2d203233          	0x2d203233
  4c:	62646767          	0x62646767
  50:	2d20                	fld	fs0,88(a0)
  52:	2d20304f          	0x2d20304f
  56:	7666                	flw	fa2,120(sp)
  58:	7261                	lui	tp,0xffff8
  5a:	742d                	lui	s0,0xfffeb
  5c:	6172                	flw	ft2,28(sp)
  5e:	6e696b63          	bltu	s2,t1,754 <STACK_SIZE-0x38ac>
  62:	662d2067          	0x662d2067
  66:	7266                	flw	ft4,120(sp)
  68:	6565                	lui	a0,0x19
  6a:	6e617473          	csrrci	s0,0x6e6,2
  6e:	6964                	flw	fs1,84(a0)
  70:	676e                	flw	fa4,216(sp)
  72:	2d20                	fld	fs0,88(a0)
  74:	6e66                	flw	ft8,88(sp)
  76:	6f632d6f          	jal	s10,3276c <STACK_SIZE+0x2e76c>
  7a:	6d6d                	lui	s10,0x1b
  7c:	2d206e6f          	jal	t3,634e <STACK_SIZE+0x234e>
  80:	6666                	flw	fa2,88(sp)
  82:	6e75                	lui	t3,0x1d
  84:	6f697463          	bgeu	s2,s6,76c <STACK_SIZE-0x3894>
  88:	2d6e                	fld	fs10,216(sp)
  8a:	74636573          	csrrsi	a0,0x746,6
  8e:	6f69                	lui	t5,0x1a
  90:	736e                	flw	ft6,248(sp)
  92:	2d20                	fld	fs0,88(a0)
  94:	6466                	flw	fs0,88(sp)
  96:	7461                	lui	s0,0xffff8
  98:	2d61                	jal	730 <STACK_SIZE-0x38d0>
  9a:	74636573          	csrrsi	a0,0x746,6
  9e:	6f69                	lui	t5,0x1a
  a0:	736e                	flw	ft6,248(sp)
  a2:	2d20                	fld	fs0,88(a0)
  a4:	7366                	flw	ft6,120(sp)
  a6:	7274                	flw	fa3,100(a2)
  a8:	6369                	lui	t1,0x1a
  aa:	2d74                	fld	fa3,216(a0)
  ac:	6f76                	flw	ft10,92(sp)
  ae:	616c                	flw	fa1,68(a0)
  b0:	6974                	flw	fa3,84(a0)
  b2:	656c                	flw	fa1,76(a0)
  b4:	622d                	lui	tp,0xb
  b6:	7469                	lui	s0,0xffffa
  b8:	6966                	flw	fs2,88(sp)
  ba:	6c65                	lui	s8,0x19
  bc:	7364                	flw	fs1,100(a4)
  be:	7300                	flw	fs0,32(a4)
  c0:	6372                	flw	ft6,28(sp)
  c2:	69616d2f          	0x69616d2f
  c6:	2e6e                	fld	ft8,216(sp)
  c8:	6d2f0063          	beq	t5,s2,788 <STACK_SIZE-0x3878>
  cc:	746e                	flw	fs0,248(sp)
  ce:	702f642f          	0x702f642f
  d2:	6f72                	flw	ft10,28(sp)
  d4:	6d617267          	0x6d617267
  d8:	6f72702f          	0x6f72702f
  dc:	656a                	flw	fa0,152(sp)
  de:	4d2f7463          	bgeu	t5,s2,5a6 <STACK_SIZE-0x3a5a>
  e2:	5279                	li	tp,-2
  e4:	5349                	li	t1,-14
  e6:	43562d43          	fmadd.d	fs10,fa2,fs5,fs0,rdn
  ea:	2f65726f          	jal	tp,573e0 <STACK_SIZE+0x533e0>
  ee:	6f6d                	lui	t5,0x1b
  f0:	6564                	flw	fs1,76(a0)
  f2:	2f6c                	fld	fa1,216(a4)
  f4:	794d                	lui	s2,0xffff3
  f6:	4952                	lw	s2,20(sp)
  f8:	562d4353          	0x562d4353
  fc:	65726f43          	0x65726f43
 100:	4d5f 646f 6c65      	0x6c65646f4d5f
 106:	435f 614d 656b      	0x656b614d435f
 10c:	7365742f          	0x7365742f
 110:	7074                	flw	fa3,100(s0)
 112:	6772                	flw	fa4,28(sp)
 114:	6e65                	lui	t3,0x19
 116:	2f76                	fld	ft10,344(sp)
 118:	616d                	addi	sp,sp,240
 11a:	6e69                	lui	t3,0x1a
 11c:	7300                	flw	fs0,32(a4)
 11e:	6372                	flw	ft6,28(sp)
 120:	6174732f          	0x6174732f
 124:	7472                	flw	fs0,60(sp)
 126:	7075                	c.lui	zero,0xffffd
 128:	532e                	lw	t1,232(sp)
 12a:	4700                	lw	s0,8(a4)
 12c:	554e                	lw	a0,240(sp)
 12e:	4120                	lw	s0,64(a0)
 130:	2e322053          	0x2e322053
 134:	          	snez	tp,zero

Disassembly of section .comment:

00000000 <.comment>:
   0:	3a434347          	fmsub.d	ft6,ft6,ft4,ft7,rmm
   4:	2820                	fld	fs0,80(s0)
   6:	69466953          	0x69466953
   a:	6576                	flw	fa0,92(sp)
   c:	4720                	lw	s0,72(a4)
   e:	38204343          	fmadd.s	ft6,ft0,ft2,ft7,rmm
  12:	332e                	fld	ft6,232(sp)
  14:	302e                	fld	ft0,232(sp)
  16:	322d                	jal	fffff940 <__stack_interrupt_cpu0+0x7fff4940>
  18:	3230                	fld	fa2,96(a2)
  1a:	2e30                	fld	fa2,88(a2)
  1c:	3430                	fld	fa2,104(s0)
  1e:	312e                	fld	ft2,232(sp)
  20:	2029                	jal	2a <STACK_SIZE-0x3fd6>
  22:	2e38                	fld	fa4,88(a2)
  24:	00302e33          	sgtz	t3,gp

Disassembly of section .riscv.attributes:

00000000 <.riscv.attributes>:
   0:	2041                	jal	80 <STACK_SIZE-0x3f80>
   2:	0000                	unimp
   4:	7200                	flw	fs0,32(a2)
   6:	7369                	lui	t1,0xffffa
   8:	01007663          	bgeu	zero,a6,14 <STACK_SIZE-0x3fec>
   c:	0016                	c.slli	zero,0x5
   e:	0000                	unimp
  10:	1004                	addi	s1,sp,32
  12:	7205                	lui	tp,0xfffe1
  14:	3376                	fld	ft6,376(sp)
  16:	6932                	flw	fs2,12(sp)
  18:	7032                	flw	ft0,44(sp)
  1a:	5f30                	lw	a2,120(a4)
  1c:	326d                	jal	fffff9c6 <__stack_interrupt_cpu0+0x7fff49c6>
  1e:	3070                	fld	fa2,224(s0)
	...

Disassembly of section .debug_frame:

00000000 <.debug_frame>:
   0:	000c                	0xc
   2:	0000                	unimp
   4:	ffff                	0xffff
   6:	ffff                	0xffff
   8:	0001                	nop
   a:	7c01                	lui	s8,0xfffe0
   c:	0d01                	addi	s10,s10,0
   e:	0002                	c.slli64	zero
  10:	0024                	addi	s1,sp,8
  12:	0000                	unimp
  14:	0000                	unimp
  16:	0000                	unimp
  18:	00a8                	addi	a0,sp,72
  1a:	8000                	0x8000
  1c:	0050                	addi	a2,sp,4
  1e:	0000                	unimp
  20:	0e44                	addi	s1,sp,788
  22:	4820                	lw	s0,80(s0)
  24:	0181                	addi	gp,gp,0
  26:	0288                	addi	a0,sp,320
  28:	0c44                	addi	s1,sp,532
  2a:	0008                	0x8
  2c:	c174                	sw	a3,68(a0)
  2e:	c844                	sw	s1,20(s0)
  30:	020c                	addi	a1,sp,256
  32:	4420                	lw	s0,72(s0)
  34:	000e                	c.slli	zero,0x3
  36:	0000                	unimp
  38:	0018                	0x18
  3a:	0000                	unimp
  3c:	0000                	unimp
  3e:	0000                	unimp
  40:	00f8                	addi	a4,sp,76
  42:	8000                	0x8000
  44:	0020                	addi	s0,sp,8
  46:	0000                	unimp
  48:	0e44                	addi	s1,sp,788
  4a:	4810                	lw	a2,16(s0)
  4c:	0181                	addi	gp,gp,0
  4e:	0288                	addi	a0,sp,320
  50:	0c44                	addi	s1,sp,532
  52:	0008                	0x8
