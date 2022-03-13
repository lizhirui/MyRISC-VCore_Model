
test.elf:     file format elf32-littleriscv


Disassembly of section .text:

80000000 <_start>:
    .section .text.entry
    .global _start
_start:
    li x1, 11
80000000:	00b00093          	li	ra,11
    li x2, 11
80000004:	00b00113          	li	sp,11
    li x3, 11
80000008:	00b00193          	li	gp,11
    li x4, 11
8000000c:	00b00213          	li	tp,11
    li x5, 11
80000010:	00b00293          	li	t0,11
    li x6, 11
80000014:	00b00313          	li	t1,11
    li x7, 11
80000018:	00b00393          	li	t2,11
    li x8, 11
8000001c:	00b00413          	li	s0,11
    li x9, 11
80000020:	00b00493          	li	s1,11
    li x11, 11
80000024:	00b00593          	li	a1,11
    li x11, 11
80000028:	00b00593          	li	a1,11
    li x12, 11
8000002c:	00b00613          	li	a2,11
    li x13, 11
80000030:	00b00693          	li	a3,11
    li x14, 11
80000034:	00b00713          	li	a4,11
    li x15, 11
80000038:	00b00793          	li	a5,11
    li x16, 11
8000003c:	00b00813          	li	a6,11
    li x17, 11
80000040:	00b00893          	li	a7,11
    li x18, 11
80000044:	00b00913          	li	s2,11
    li x19, 11
80000048:	00b00993          	li	s3,11
    la x20, 0x80001000
8000004c:	80001a37          	lui	s4,0x80001
    sw x20, 0(x20)
80000050:	014a2023          	sw	s4,0(s4) # 80001000 <_heap_start+0xffff6000>
    fence
80000054:	0ff0000f          	fence
    la x20, 0x80001010
80000058:	80001a37          	lui	s4,0x80001
8000005c:	010a0a13          	addi	s4,s4,16 # 80001010 <_heap_start+0xffff6010>
    sw x20, 0(x20)
80000060:	014a2023          	sw	s4,0(s4)
    la x20, 0x80001020
80000064:	80001a37          	lui	s4,0x80001
80000068:	020a0a13          	addi	s4,s4,32 # 80001020 <_heap_start+0xffff6020>
    fence
8000006c:	0ff0000f          	fence
    sw x20, 0(x20)
80000070:	014a2023          	sw	s4,0(s4)
    fence.i
80000074:	0000100f          	fence.i
    li x1, 11
80000078:	00b00093          	li	ra,11
    li x2, 11
8000007c:	00b00113          	li	sp,11
    li x3, 11
80000080:	00b00193          	li	gp,11
    li x4, 11
80000084:	00b00213          	li	tp,11
    li x5, 11
80000088:	00b00293          	li	t0,11
    li x6, 11
8000008c:	00b00313          	li	t1,11
    li x7, 11
80000090:	00b00393          	li	t2,11
    li x8, 11
80000094:	00b00413          	li	s0,11
    li x9, 11
80000098:	00b00493          	li	s1,11
    li x11, 11
8000009c:	00b00593          	li	a1,11
    li x11, 11
800000a0:	00b00593          	li	a1,11
    li x12, 11
800000a4:	00b00613          	li	a2,11
    li x13, 11
800000a8:	00b00693          	li	a3,11
    li x14, 11
800000ac:	00b00713          	li	a4,11
    li x15, 11
800000b0:	00b00793          	li	a5,11
    li x16, 11
800000b4:	00b00813          	li	a6,11
    li x17, 11
800000b8:	00b00893          	li	a7,11
    li x18, 11
800000bc:	00b00913          	li	s2,11
    li x19, 11
800000c0:	00b00993          	li	s3,11

800000c4 <loop>:

loop:
    addi x1, x1, -1
800000c4:	fff08093          	addi	ra,ra,-1
    bne x1, x0, loop
800000c8:	fe009ee3          	bnez	ra,800000c4 <loop>
    li x2, 1
800000cc:	00100113          	li	sp,1

800000d0 <infinite_loop>:
infinite_loop:
800000d0:	0000006f          	j	800000d0 <infinite_loop>

Disassembly of section .osdebug:

800000d8 <_osdebug_start>:
	...

Disassembly of section .bss:

800020d8 <sbss>:
	...

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
  1c:	326d                	jal	fffff9c6 <_heap_start+0x7fff49c6>
  1e:	3070                	fld	fa2,224(s0)
	...

Disassembly of section .debug_line:

00000000 <.debug_line>:
   0:	0164                	addi	s1,sp,140
   2:	0000                	unimp
   4:	00240003          	lb	zero,2(s0)
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
  20:	72617473          	csrrci	s0,0x726,2
  24:	7574                	flw	fa3,108(a0)
  26:	2e70                	fld	fa2,216(a2)
  28:	00010053          	fadd.s	ft0,ft2,ft0,rne
  2c:	0000                	unimp
  2e:	0500                	addi	s0,sp,640
  30:	0002                	c.slli64	zero
  32:	0000                	unimp
  34:	1580                	addi	s0,sp,736
  36:	04090103          	lb	sp,64(s2)
  3a:	0100                	addi	s0,sp,128
  3c:	04090103          	lb	sp,64(s2)
  40:	0100                	addi	s0,sp,128
  42:	04090103          	lb	sp,64(s2)
  46:	0100                	addi	s0,sp,128
  48:	04090103          	lb	sp,64(s2)
  4c:	0100                	addi	s0,sp,128
  4e:	04090103          	lb	sp,64(s2)
  52:	0100                	addi	s0,sp,128
  54:	04090103          	lb	sp,64(s2)
  58:	0100                	addi	s0,sp,128
  5a:	04090103          	lb	sp,64(s2)
  5e:	0100                	addi	s0,sp,128
  60:	04090103          	lb	sp,64(s2)
  64:	0100                	addi	s0,sp,128
  66:	04090103          	lb	sp,64(s2)
  6a:	0100                	addi	s0,sp,128
  6c:	04090103          	lb	sp,64(s2)
  70:	0100                	addi	s0,sp,128
  72:	04090103          	lb	sp,64(s2)
  76:	0100                	addi	s0,sp,128
  78:	04090103          	lb	sp,64(s2)
  7c:	0100                	addi	s0,sp,128
  7e:	04090103          	lb	sp,64(s2)
  82:	0100                	addi	s0,sp,128
  84:	04090103          	lb	sp,64(s2)
  88:	0100                	addi	s0,sp,128
  8a:	04090103          	lb	sp,64(s2)
  8e:	0100                	addi	s0,sp,128
  90:	04090103          	lb	sp,64(s2)
  94:	0100                	addi	s0,sp,128
  96:	04090103          	lb	sp,64(s2)
  9a:	0100                	addi	s0,sp,128
  9c:	04090103          	lb	sp,64(s2)
  a0:	0100                	addi	s0,sp,128
  a2:	04090103          	lb	sp,64(s2)
  a6:	0100                	addi	s0,sp,128
  a8:	04090103          	lb	sp,64(s2)
  ac:	0100                	addi	s0,sp,128
  ae:	04090103          	lb	sp,64(s2)
  b2:	0100                	addi	s0,sp,128
  b4:	04090103          	lb	sp,64(s2)
  b8:	0100                	addi	s0,sp,128
  ba:	08090103          	lb	sp,128(s2)
  be:	0100                	addi	s0,sp,128
  c0:	04090103          	lb	sp,64(s2)
  c4:	0100                	addi	s0,sp,128
  c6:	08090103          	lb	sp,128(s2)
  ca:	0100                	addi	s0,sp,128
  cc:	04090103          	lb	sp,64(s2)
  d0:	0100                	addi	s0,sp,128
  d2:	04090103          	lb	sp,64(s2)
  d6:	0100                	addi	s0,sp,128
  d8:	04090103          	lb	sp,64(s2)
  dc:	0100                	addi	s0,sp,128
  de:	04090103          	lb	sp,64(s2)
  e2:	0100                	addi	s0,sp,128
  e4:	04090103          	lb	sp,64(s2)
  e8:	0100                	addi	s0,sp,128
  ea:	04090103          	lb	sp,64(s2)
  ee:	0100                	addi	s0,sp,128
  f0:	04090103          	lb	sp,64(s2)
  f4:	0100                	addi	s0,sp,128
  f6:	04090103          	lb	sp,64(s2)
  fa:	0100                	addi	s0,sp,128
  fc:	04090103          	lb	sp,64(s2)
 100:	0100                	addi	s0,sp,128
 102:	04090103          	lb	sp,64(s2)
 106:	0100                	addi	s0,sp,128
 108:	04090103          	lb	sp,64(s2)
 10c:	0100                	addi	s0,sp,128
 10e:	04090103          	lb	sp,64(s2)
 112:	0100                	addi	s0,sp,128
 114:	04090103          	lb	sp,64(s2)
 118:	0100                	addi	s0,sp,128
 11a:	04090103          	lb	sp,64(s2)
 11e:	0100                	addi	s0,sp,128
 120:	04090103          	lb	sp,64(s2)
 124:	0100                	addi	s0,sp,128
 126:	04090103          	lb	sp,64(s2)
 12a:	0100                	addi	s0,sp,128
 12c:	04090103          	lb	sp,64(s2)
 130:	0100                	addi	s0,sp,128
 132:	04090103          	lb	sp,64(s2)
 136:	0100                	addi	s0,sp,128
 138:	04090103          	lb	sp,64(s2)
 13c:	0100                	addi	s0,sp,128
 13e:	04090103          	lb	sp,64(s2)
 142:	0100                	addi	s0,sp,128
 144:	04090103          	lb	sp,64(s2)
 148:	0100                	addi	s0,sp,128
 14a:	04090303          	lb	t1,64(s2)
 14e:	0100                	addi	s0,sp,128
 150:	04090103          	lb	sp,64(s2)
 154:	0100                	addi	s0,sp,128
 156:	04090103          	lb	sp,64(s2)
 15a:	0100                	addi	s0,sp,128
 15c:	04090203          	lb	tp,64(s2)
 160:	0100                	addi	s0,sp,128
 162:	0409                	addi	s0,s0,2
 164:	0000                	unimp
 166:	0101                	addi	sp,sp,0

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
  14:	00d4                	addi	a3,sp,68
  16:	8000                	0x8000
  18:	0000                	unimp
  1a:	0000                	unimp
  1c:	000e                	c.slli	zero,0x3
  1e:	0000                	unimp
  20:	0061                	c.nop	24
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
  14:	00d4                	addi	a3,sp,68
	...

Disassembly of section .debug_str:

00000000 <.debug_str>:
   0:	2f637273          	csrrci	tp,0x2f6,6
   4:	72617473          	csrrci	s0,0x726,2
   8:	7574                	flw	fa3,108(a0)
   a:	2e70                	fld	fa2,216(a2)
   c:	6d2f0053          	0x6d2f0053
  10:	746e                	flw	fs0,248(sp)
  12:	702f642f          	0x702f642f
  16:	6f72                	flw	ft10,28(sp)
  18:	6d617267          	0x6d617267
  1c:	6f72702f          	0x6f72702f
  20:	656a                	flw	fa0,152(sp)
  22:	4d2f7463          	bgeu	t5,s2,4ea <STACK_SIZE-0x3b16>
  26:	5279                	li	tp,-2
  28:	5349                	li	t1,-14
  2a:	43562d43          	fmadd.d	fs10,fa2,fs5,fs0,rdn
  2e:	2f65726f          	jal	tp,57324 <STACK_SIZE+0x53324>
  32:	6f6d                	lui	t5,0x1b
  34:	6564                	flw	fs1,76(a0)
  36:	2f6c                	fld	fa1,216(a4)
  38:	794d                	lui	s2,0xffff3
  3a:	4952                	lw	s2,20(sp)
  3c:	562d4353          	0x562d4353
  40:	65726f43          	0x65726f43
  44:	4d5f 646f 6c65      	0x6c65646f4d5f
  4a:	435f 614d 656b      	0x656b614d435f
  50:	7365742f          	0x7365742f
  54:	7074                	flw	fa3,100(s0)
  56:	6772                	flw	fa4,28(sp)
  58:	6e65                	lui	t3,0x19
  5a:	2f76                	fld	ft10,344(sp)
  5c:	616d                	addi	sp,sp,240
  5e:	6e69                	lui	t3,0x1a
  60:	4700                	lw	s0,8(a4)
  62:	554e                	lw	a0,240(sp)
  64:	4120                	lw	s0,64(a0)
  66:	2e322053          	0x2e322053
  6a:	          	snez	tp,zero
