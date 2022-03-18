
test.elf:     file format elf32-littleriscv


Disassembly of section .text:

80000000 <_start>:
    .section .text.entry
    .global _start
_start:
    li x1, 1
80000000:	00100093          	li	ra,1
    li x5, 0x20000000
80000004:	200002b7          	lui	t0,0x20000
    sw x1, 0(x5)
80000008:	0012a023          	sw	ra,0(t0) # 20000000 <STACK_SIZE+0x1fffc000>
    la x2, exception_entry
8000000c:	00000117          	auipc	sp,0x0
80000010:	01810113          	addi	sp,sp,24 # 80000024 <exception_entry>
    csrw mtvec, x2
80000014:	30511073          	csrw	mtvec,sp
    li x6, 0x8
80000018:	00800313          	li	t1,8
    /*csrw mie, x6
    csrsi mstatus, 0x8*/
    csrr x2, 0x123
8000001c:	12302173          	csrr	sp,0x123

80000020 <loop>:

loop:
    j loop
80000020:	0000006f          	j	80000020 <loop>

80000024 <exception_entry>:

exception_entry:
    //li x1, 100
    mv x1, x2
80000024:	00010093          	mv	ra,sp
    mret
80000028:	30200073          	mret

Disassembly of section .osdebug:

80000030 <_osdebug_start>:
	...

Disassembly of section .bss:

80002030 <sbss>:
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
   0:	006e                	c.slli	zero,0x1b
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
  48:	08090103          	lb	sp,128(s2)
  4c:	0100                	addi	s0,sp,128
  4e:	04090103          	lb	sp,64(s2)
  52:	0100                	addi	s0,sp,128
  54:	04090303          	lb	t1,64(s2)
  58:	0100                	addi	s0,sp,128
  5a:	04090303          	lb	t1,64(s2)
  5e:	0100                	addi	s0,sp,128
  60:	04090403          	lb	s0,64(s2)
  64:	0100                	addi	s0,sp,128
  66:	04090103          	lb	sp,64(s2)
  6a:	0100                	addi	s0,sp,128
  6c:	0409                	addi	s0,s0,2
  6e:	0000                	unimp
  70:	0101                	addi	sp,sp,0

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
  14:	002c                	addi	a1,sp,8
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
  14:	002c                	addi	a1,sp,8
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
