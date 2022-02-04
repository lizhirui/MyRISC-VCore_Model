
test.elf:     file format elf32-littleriscv


Disassembly of section .text:

80000000 <_start>:
    .section .text.entry
    .global _start
_start:
    li x1, 0
80000000:	00000093          	li	ra,0
    li x2, 0
80000004:	00000113          	li	sp,0
    li x3, 0
80000008:	00000193          	li	gp,0
    li x4, 0
8000000c:	00000213          	li	tp,0
    li x5, 0
80000010:	00000293          	li	t0,0
    li x6, 0
80000014:	00000313          	li	t1,0
    li x7, 0
80000018:	00000393          	li	t2,0
    li x8, 0
8000001c:	00000413          	li	s0,0
    li x9, 0
80000020:	00000493          	li	s1,0
    li x10, 0
80000024:	00000513          	li	a0,0
    addi x1, x1, 1
80000028:	00108093          	addi	ra,ra,1
    addi x2, x2, 1
8000002c:	00110113          	addi	sp,sp,1
    addi x3, x3, 1
80000030:	00118193          	addi	gp,gp,1
    addi x4, x4, 1
80000034:	00120213          	addi	tp,tp,1 # 1 <STACK_SIZE-0x3fff>
    addi x5, x5, 1
80000038:	00128293          	addi	t0,t0,1
    addi x6, x6, 1
8000003c:	00130313          	addi	t1,t1,1
    addi x7, x7, 1
80000040:	00138393          	addi	t2,t2,1
    addi x8, x8, 1
80000044:	00140413          	addi	s0,s0,1
    addi x1, x1, 1
80000048:	00108093          	addi	ra,ra,1
    addi x2, x2, 1
8000004c:	00110113          	addi	sp,sp,1

80000050 <loop>:

loop:
    addi x1, x1, 1
80000050:	00108093          	addi	ra,ra,1
    addi x2, x2, 1
80000054:	00110113          	addi	sp,sp,1
    addi x3, x3, 1
80000058:	00118193          	addi	gp,gp,1
    addi x4, x4, 1
8000005c:	00120213          	addi	tp,tp,1 # 1 <STACK_SIZE-0x3fff>
    addi x5, x5, 1
80000060:	00128293          	addi	t0,t0,1
    addi x6, x6, 1
80000064:	00130313          	addi	t1,t1,1
    addi x7, x7, 1
80000068:	00138393          	addi	t2,t2,1
    addi x8, x8, 1
8000006c:	00140413          	addi	s0,s0,1
    addi x9, x9, 1
80000070:	00148493          	addi	s1,s1,1
    addi x10, x10, 1
80000074:	00150513          	addi	a0,a0,1
    addi x1, x1, 1
80000078:	00108093          	addi	ra,ra,1
    addi x2, x2, 1
8000007c:	00110113          	addi	sp,sp,1
    addi x3, x3, 1
80000080:	00118193          	addi	gp,gp,1
    addi x4, x4, 1
80000084:	00120213          	addi	tp,tp,1 # 1 <STACK_SIZE-0x3fff>
    addi x5, x5, 1
80000088:	00128293          	addi	t0,t0,1
    addi x6, x6, 1
8000008c:	00130313          	addi	t1,t1,1
    addi x7, x7, 1
80000090:	00138393          	addi	t2,t2,1
    addi x8, x8, 1
80000094:	00140413          	addi	s0,s0,1
    addi x9, x9, 1
80000098:	00148493          	addi	s1,s1,1
    addi x10, x10, 1
8000009c:	00150513          	addi	a0,a0,1
800000a0:	fb1ff06f          	j	80000050 <loop>

Disassembly of section .osdebug:

800000a8 <_osdebug_start>:
	...

Disassembly of section .bss:

800020a8 <sbss>:
	...

Disassembly of section .riscv.attributes:

00000000 <.riscv.attributes>:
   0:	1e41                	addi	t3,t3,-16
   2:	0000                	unimp
   4:	7200                	flw	fs0,32(a2)
   6:	7369                	lui	t1,0xffffa
   8:	01007663          	bgeu	zero,a6,14 <STACK_SIZE-0x3fec>
   c:	0014                	0x14
   e:	0000                	unimp
  10:	7205                	lui	tp,0xfffe1
  12:	3376                	fld	ft6,376(sp)
  14:	6932                	flw	fs2,12(sp)
  16:	7032                	flw	ft0,44(sp)
  18:	5f30                	lw	a2,120(a4)
  1a:	326d                	jal	fffff9c4 <_heap_start+0x7fff49c4>
  1c:	3070                	fld	fa2,224(s0)
	...

Disassembly of section .debug_line:

00000000 <.debug_line>:
   0:	0125                	addi	sp,sp,9
   2:	0000                	unimp
   4:	00210003          	lb	zero,2(sp)
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
  24:	532e                	lw	t1,232(sp)
  26:	0100                	addi	s0,sp,128
  28:	0000                	unimp
  2a:	0000                	unimp
  2c:	0205                	addi	tp,tp,1
  2e:	0000                	unimp
  30:	8000                	0x8000
  32:	0315                	addi	t1,t1,5
  34:	0901                	addi	s2,s2,0
  36:	0004                	0x4
  38:	0301                	addi	t1,t1,0
  3a:	0901                	addi	s2,s2,0
  3c:	0004                	0x4
  3e:	0301                	addi	t1,t1,0
  40:	0901                	addi	s2,s2,0
  42:	0004                	0x4
  44:	0301                	addi	t1,t1,0
  46:	0901                	addi	s2,s2,0
  48:	0004                	0x4
  4a:	0301                	addi	t1,t1,0
  4c:	0901                	addi	s2,s2,0
  4e:	0004                	0x4
  50:	0301                	addi	t1,t1,0
  52:	0901                	addi	s2,s2,0
  54:	0004                	0x4
  56:	0301                	addi	t1,t1,0
  58:	0901                	addi	s2,s2,0
  5a:	0004                	0x4
  5c:	0301                	addi	t1,t1,0
  5e:	0901                	addi	s2,s2,0
  60:	0004                	0x4
  62:	0301                	addi	t1,t1,0
  64:	0901                	addi	s2,s2,0
  66:	0004                	0x4
  68:	0301                	addi	t1,t1,0
  6a:	0901                	addi	s2,s2,0
  6c:	0004                	0x4
  6e:	0301                	addi	t1,t1,0
  70:	0901                	addi	s2,s2,0
  72:	0004                	0x4
  74:	0301                	addi	t1,t1,0
  76:	0901                	addi	s2,s2,0
  78:	0004                	0x4
  7a:	0301                	addi	t1,t1,0
  7c:	0901                	addi	s2,s2,0
  7e:	0004                	0x4
  80:	0301                	addi	t1,t1,0
  82:	0901                	addi	s2,s2,0
  84:	0004                	0x4
  86:	0301                	addi	t1,t1,0
  88:	0901                	addi	s2,s2,0
  8a:	0004                	0x4
  8c:	0301                	addi	t1,t1,0
  8e:	0901                	addi	s2,s2,0
  90:	0004                	0x4
  92:	0301                	addi	t1,t1,0
  94:	0901                	addi	s2,s2,0
  96:	0004                	0x4
  98:	0301                	addi	t1,t1,0
  9a:	0901                	addi	s2,s2,0
  9c:	0004                	0x4
  9e:	0301                	addi	t1,t1,0
  a0:	0901                	addi	s2,s2,0
  a2:	0004                	0x4
  a4:	0301                	addi	t1,t1,0
  a6:	00040903          	lb	s2,0(s0)
  aa:	0301                	addi	t1,t1,0
  ac:	0901                	addi	s2,s2,0
  ae:	0004                	0x4
  b0:	0301                	addi	t1,t1,0
  b2:	0901                	addi	s2,s2,0
  b4:	0004                	0x4
  b6:	0301                	addi	t1,t1,0
  b8:	0901                	addi	s2,s2,0
  ba:	0004                	0x4
  bc:	0301                	addi	t1,t1,0
  be:	0901                	addi	s2,s2,0
  c0:	0004                	0x4
  c2:	0301                	addi	t1,t1,0
  c4:	0901                	addi	s2,s2,0
  c6:	0004                	0x4
  c8:	0301                	addi	t1,t1,0
  ca:	0901                	addi	s2,s2,0
  cc:	0004                	0x4
  ce:	0301                	addi	t1,t1,0
  d0:	0901                	addi	s2,s2,0
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
 122:	0901                	addi	s2,s2,0
 124:	0004                	0x4
 126:	0100                	addi	s0,sp,128
 128:	01              	Address 0x0000000000000128 is out of bounds.


Disassembly of section .debug_info:

00000000 <.debug_info>:
   0:	0022                	c.slli	zero,0x8
   2:	0000                	unimp
   4:	0002                	c.slli64	zero
   6:	0000                	unimp
   8:	0000                	unimp
   a:	0104                	addi	s1,sp,128
   c:	0000                	unimp
   e:	0000                	unimp
  10:	0000                	unimp
  12:	8000                	0x8000
  14:	00a4                	addi	s1,sp,72
  16:	8000                	0x8000
  18:	0000                	unimp
  1a:	0000                	unimp
  1c:	0000000b          	0xb
  20:	005e                	c.slli	zero,0x17
  22:	0000                	unimp
  24:	8001                	c.srli64	s0

Disassembly of section .debug_abbrev:

00000000 <.debug_abbrev>:
   0:	1101                	addi	sp,sp,-32
   2:	1000                	addi	s0,sp,32
   4:	1106                	slli	sp,sp,0x21
   6:	1201                	addi	tp,tp,-32
   8:	0301                	addi	t1,t1,0
   a:	1b0e                	slli	s6,s6,0x23
   c:	250e                	fld	fa0,192(sp)
   e:	130e                	slli	t1,t1,0x23
  10:	0005                	c.nop	1
	...

Disassembly of section .debug_aranges:

00000000 <.debug_aranges>:
   0:	001c                	0x1c
   2:	0000                	unimp
   4:	0002                	c.slli64	zero
   6:	0000                	unimp
   8:	0000                	unimp
   a:	0004                	0x4
   c:	0000                	unimp
   e:	0000                	unimp
  10:	0000                	unimp
  12:	8000                	0x8000
  14:	00a4                	addi	s1,sp,72
	...

Disassembly of section .debug_str:

00000000 <.debug_str>:
   0:	2f637273          	csrrci	tp,0x2f6,6
   4:	616d                	addi	sp,sp,240
   6:	6e69                	lui	t3,0x1a
   8:	532e                	lw	t1,232(sp)
   a:	2f00                	fld	fs0,24(a4)
   c:	6e6d                	lui	t3,0x1b
   e:	2f74                	fld	fa3,216(a4)
  10:	2f64                	fld	fs1,216(a4)
  12:	7270                	flw	fa2,100(a2)
  14:	6172676f          	jal	a4,26e2a <STACK_SIZE+0x22e2a>
  18:	2f6d                	jal	7d2 <STACK_SIZE-0x382e>
  1a:	7270                	flw	fa2,100(a2)
  1c:	63656a6f          	jal	s4,56652 <STACK_SIZE+0x52652>
  20:	2f74                	fld	fa3,216(a4)
  22:	794d                	lui	s2,0xffff3
  24:	4952                	lw	s2,20(sp)
  26:	562d4353          	0x562d4353
  2a:	65726f43          	0x65726f43
  2e:	646f6d2f          	0x646f6d2f
  32:	6c65                	lui	s8,0x19
  34:	52794d2f          	0x52794d2f
  38:	5349                	li	t1,-14
  3a:	43562d43          	fmadd.d	fs10,fa2,fs5,fs0,rdn
  3e:	5f65726f          	jal	tp,57634 <STACK_SIZE+0x53634>
  42:	6f4d                	lui	t5,0x13
  44:	6564                	flw	fs1,76(a0)
  46:	5f6c                	lw	a1,124(a4)
  48:	6b614d43          	fmadd.d	fs10,ft2,fs6,fa3,rmm
  4c:	2f65                	jal	804 <STACK_SIZE-0x37fc>
  4e:	6574                	flw	fa3,76(a0)
  50:	72707473          	csrrci	s0,0x727,0
  54:	766e6567          	0x766e6567
  58:	69616d2f          	0x69616d2f
  5c:	006e                	c.slli	zero,0x1b
  5e:	20554e47          	fmsub.s	ft8,fa0,ft5,ft4,rmm
  62:	5341                	li	t1,-16
  64:	3220                	fld	fs0,96(a2)
  66:	332e                	fld	ft6,232(sp)
  68:	0032                	c.slli	zero,0xc
