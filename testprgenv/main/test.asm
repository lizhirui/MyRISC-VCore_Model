
test.elf:     file format elf32-littleriscv


Disassembly of section .text:

80000000 <_start>:
80000000:	00208093          	addi	ra,ra,2
80000004:	ffdff06f          	j	80000000 <_start>

Disassembly of section .osdebug:

80000008 <_osdebug_start>:
	...

Disassembly of section .bss:

80002008 <sbss>:
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
   0:	0000003b          	0x3b
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
  38:	0901                	addi	s2,s2,0
  3a:	0004                	0x4
  3c:	0100                	addi	s0,sp,128
  3e:	01              	Address 0x000000000000003e is out of bounds.


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
  14:	0008                	0x8
  16:	8000                	0x8000
  18:	0000                	unimp
  1a:	0000                	unimp
  1c:	0000000b          	0xb
  20:	004c                	addi	a1,sp,4
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
  14:	0008                	0x8
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
  34:	7365542f          	0x7365542f
  38:	5074                	lw	a3,100(s0)
  3a:	6772                	flw	fa4,28(sp)
  3c:	6e45                	lui	t3,0x11
  3e:	6976                	flw	fs2,92(sp)
  40:	6f72                	flw	ft10,28(sp)
  42:	6d6e                	flw	fs10,216(sp)
  44:	6e65                	lui	t3,0x19
  46:	2f74                	fld	fa3,216(a4)
  48:	00637273          	csrrci	tp,0x6,6
  4c:	20554e47          	fmsub.s	ft8,fa0,ft5,ft4,rmm
  50:	5341                	li	t1,-16
  52:	3220                	fld	fs0,96(a2)
  54:	332e                	fld	ft6,232(sp)
  56:	0032                	c.slli	zero,0xc
