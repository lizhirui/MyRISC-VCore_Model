
test.elf:     file format elf32-littleriscv


Disassembly of section .text:

80000000 <_start>:
    addi x8, x8, 1
    addi x9, x9, 1
    addi x10, x10, 1
    j loop*/

    la x1, _exception
80000000:	00000097          	auipc	ra,0x0
80000004:	02008093          	addi	ra,ra,32 # 80000020 <_exception>
    csrw mtvec, x1
80000008:	30509073          	csrw	mtvec,ra
    ecall
8000000c:	00000073          	ecall
    li x3, 1
80000010:	00100193          	li	gp,1
80000014:	00000013          	nop
80000018:	00000013          	nop
8000001c:	00000013          	nop

80000020 <_exception>:

    .align 4
_exception:
    li x2, 1
80000020:	00100113          	li	sp,1
80000024:	ffdff06f          	j	80000020 <_exception>
	...

Disassembly of section .osdebug:

80000030 <_osdebug_start>:
	...

Disassembly of section .bss:

80002030 <sbss>:
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
   0:	0055                	c.nop	21
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
  32:	03012f03          	lw	t5,48(sp)
  36:	0901                	addi	s2,s2,0
  38:	0008                	0x8
  3a:	0301                	addi	t1,t1,0
  3c:	0901                	addi	s2,s2,0
  3e:	0004                	0x4
  40:	0301                	addi	t1,t1,0
  42:	0901                	addi	s2,s2,0
  44:	0004                	0x4
  46:	0301                	addi	t1,t1,0
  48:	0904                	addi	s1,sp,144
  4a:	0010                	0x10
  4c:	0301                	addi	t1,t1,0
  4e:	0901                	addi	s2,s2,0
  50:	0004                	0x4
  52:	0901                	addi	s2,s2,0
  54:	0004                	0x4
  56:	0100                	addi	s0,sp,128
  58:	01              	Address 0x0000000000000058 is out of bounds.


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
  14:	0028                	addi	a0,sp,8
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
  14:	0028                	addi	a0,sp,8
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
