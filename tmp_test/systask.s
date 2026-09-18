	.file	"systask.cpp"
gcc2_compiled.:

	.section	.text
.L_text_b:
	.previous

	.section	.data
.L_data_b:
	.previous

	.section	.rodata
.L_rodata_b:
	.previous

	.section	.bss
.L_bss_b:
	.previous

	.section	.debug_sfnames
.L_sfnames_b:
	.string	"C:\\Users\\jferr\\Desktop\\nfsdecompiled/"
	.previous

	.section	.line
.L_line_b:
	.4byte	.L_line_e-.L_line_b
	.4byte	.L_text_b
	.previous

	.section	.debug_srcinfo
.L_srcinfo_b:
	.4byte	.L_line_b
	.4byte	.L_sfnames_b
	.4byte	.L_text_b
	.4byte	.L_text_e
	.4byte	0xffffffff
	.previous

	.section	.debug_pubnames
	.4byte	.L_debug_b
	.previous

	.section	.debug_aranges
	.4byte	.L_debug_b
	.previous

	.section	.debug
.L_debug_b:
.L_D1:
	.4byte	.L_D1_e-.L_D1
	.2byte	0x11
	.2byte	0x12
	.4byte	.L_D2
	.2byte	0x38
	.string	"src/Speed/Indep/Libs/realcore/6.24.00/source/system/cmn/systask.cpp"
	.2byte	0x258
	.string	"GNU C++ 2.95.3 SN BUILD v1.76 for Nintendo Gamecube"
	.2byte	0x136
	.4byte	0x4
	.2byte	0x111
	.4byte	.L_text_b
	.2byte	0x121
	.4byte	.L_text_e
	.2byte	0x106
	.4byte	.L_line_b
	.2byte	0x1b8
	.string	"C:\\Users\\jferr\\Desktop\\nfsdecompiled"
	.2byte	0x8006
	.4byte	.L_sfnames_b
	.2byte	0x8016
	.4byte	.L_srcinfo_b
.L_D1_e:
	.previous

	.section	".sdata","aw"
	.align 2
	.type	 reentry.3,@object
	.size	 reentry.3,4
reentry.3:
	.long 0
	.section	".text"
	.align 2
	.globl SYNCTASK_add__FPFPvi_viiPv
	.type	 SYNCTASK_add__FPFPvi_viiPv,@function
SYNCTASK_add__FPFPvi_viiPv:
.L_fSYNCTASK_add__FPFPvi_viiPv_s:
.L_LC1:


	.section	.debug_sfnames
.L_F0:
	.string	"src/Speed/Indep/Libs/realcore/6.24.00/source/system/cmn/systask.cpp"
	.previous
	.section	.line
.L_LE1:
	.4byte	15	 # /systask.cpp:15
	.2byte	0xffff
	.4byte	.L_LC1-.L_text_b
	.previous

	.section	.debug_srcinfo
	.4byte	.L_LE1-.L_line_b
	.4byte	.L_F0-.L_sfnames_b
	.previous
	stwu 1,-16(1)
	stw 31,12(1)
.L_b1:
.L_B2:
	mr 31,6
.L_LC2:

	.section	.line
	.4byte	18	 # /systask.cpp:18
	.2byte	0xffff
	.4byte	.L_LC2-.L_text_b
	.previous
	cmpwi 0,4,-1
.L_LC3:

	.section	.line
	.4byte	19	 # /systask.cpp:19
	.2byte	0xffff
	.4byte	.L_LC3-.L_text_b
	.previous
	li 12,0
.L_LC4:

	.section	.line
	.4byte	18	 # /systask.cpp:18
	.2byte	0xffff
	.4byte	.L_LC4-.L_text_b
	.previous
	bc 12,2,.L4        # beq  cr0
.L_LC5:

	.section	.line
	.4byte	19	 # /systask.cpp:19
	.2byte	0xffff
	.4byte	.L_LC5-.L_text_b
	.previous
.L_LC6:

	.section	.line
	.4byte	23	 # /systask.cpp:23
	.2byte	0xffff
	.4byte	.L_LC6-.L_text_b
	.previous
	mr 12,4
.L_LC7:

	.section	.line
	.4byte	20	 # /systask.cpp:20
	.2byte	0xffff
	.4byte	.L_LC7-.L_text_b
	.previous
	cmpwi 0,4,0
	bc 4,2,.L4        # bne  cr0
.L_LC8:

	.section	.line
	.4byte	21	 # /systask.cpp:21
	.2byte	0xffff
	.4byte	.L_LC8-.L_text_b
	.previous
	li 12,1
.L4:
.L_LC9:

	.section	.line
	.4byte	27	 # /systask.cpp:27
	.2byte	0xffff
	.4byte	.L_LC9-.L_text_b
	.previous
.L_B3:
	lwz 11,reentry.3@sda21(0)
.L_LC10:

	.section	.line
	.4byte	31	 # /systask.cpp:31
	.2byte	0xffff
	.4byte	.L_LC10-.L_text_b
	.previous
	lis 9,systemtasksubs@ha
	la 9,systemtasksubs@l(9)
.L_LC11:

	.section	.line
	.4byte	30	 # /systask.cpp:30
	.2byte	0xffff
	.4byte	.L_LC11-.L_text_b
	.previous
	li 8,-1
.L_LC12:

	.section	.line
	.4byte	28	 # /systask.cpp:28
	.2byte	0xffff
	.4byte	.L_LC12-.L_text_b
	.previous
	addi 0,11,1
.L_LC13:

	.section	.line
	.4byte	31	 # /systask.cpp:31
	.2byte	0xffff
	.4byte	.L_LC13-.L_text_b
	.previous
	li 10,0
.L_LC14:

	.section	.line
	.4byte	28	 # /systask.cpp:28
	.2byte	0xffff
	.4byte	.L_LC14-.L_text_b
	.previous
	stw 0,reentry.3@sda21(0)
.L_LC15:

	.section	.line
	.4byte	31	 # /systask.cpp:31
	.2byte	0xffff
	.4byte	.L_LC15-.L_text_b
	.previous
	li 0,16
	mtctr 0
.L18:
.L_LC16:

	.section	.line
	.4byte	32	 # /systask.cpp:32
	.2byte	0xffff
	.4byte	.L_LC16-.L_text_b
	.previous
	lwz 0,0(9)
	addi 9,9,20
	cmpw 0,0,3
	bc 12,2,.L19        # beq  cr0
.L_LC17:

	.section	.line
	.4byte	34	 # /systask.cpp:34
	.2byte	0xffff
	.4byte	.L_LC17-.L_text_b
	.previous
	cmpwi 0,0,0
	bc 4,2,.L9        # bne  cr0
	cmpwi 0,8,-1
	bc 4,2,.L9        # bne  cr0
.L_LC18:

	.section	.line
	.4byte	35	 # /systask.cpp:35
	.2byte	0xffff
	.4byte	.L_LC18-.L_text_b
	.previous
	cmpwi 0,11,0
	bc 4,2,.L14        # bne  cr0
.L_LC19:

	.section	.line
	.4byte	36	 # /systask.cpp:36
	.2byte	0xffff
	.4byte	.L_LC19-.L_text_b
	.previous
.L19:
	mr 8,10
	b .L9
.L14:
.L_LC20:

	.section	.line
	.4byte	38	 # /systask.cpp:38
	.2byte	0xffff
	.4byte	.L_LC20-.L_text_b
	.previous
	addi 11,11,-1
.L_LC21:

	.section	.line
	.4byte	41	 # /systask.cpp:41
	.2byte	0xffff
	.4byte	.L_LC21-.L_text_b
	.previous
.L9:
	addi 10,10,1
	bdnz .L18
.L_B3_e:
.L_LC22:

	.section	.line
	.4byte	43	 # /systask.cpp:43
	.2byte	0xffff
	.4byte	.L_LC22-.L_text_b
	.previous
	mulli 0,8,20
	lis 9,systemtasksubs@ha
.L_LC23:

	.section	.line
	.4byte	48	 # /systask.cpp:48
	.2byte	0xffff
	.4byte	.L_LC23-.L_text_b
	.previous
	lwz 10,reentry.3@sda21(0)
.L_LC24:

	.section	.line
	.4byte	43	 # /systask.cpp:43
	.2byte	0xffff
	.4byte	.L_LC24-.L_text_b
	.previous
	la 9,systemtasksubs@l(9)
.L_LC25:

	.section	.line
	.4byte	46	 # /systask.cpp:46
	.2byte	0xffff
	.4byte	.L_LC25-.L_text_b
	.previous
	lwz 11,libticks@sda21(0)
.L_LC26:

	.section	.line
	.4byte	43	 # /systask.cpp:43
	.2byte	0xffff
	.4byte	.L_LC26-.L_text_b
	.previous
	addi 4,9,16
	li 8,0
.L_LC27:

	.section	.line
	.4byte	44	 # /systask.cpp:44
	.2byte	0xffff
	.4byte	.L_LC27-.L_text_b
	.previous
	addi 7,9,4
.L_LC28:

	.section	.line
	.4byte	47	 # /systask.cpp:47
	.2byte	0xffff
	.4byte	.L_LC28-.L_text_b
	.previous
	stwx 3,9,0
.L_LC29:

	.section	.line
	.4byte	46	 # /systask.cpp:46
	.2byte	0xffff
	.4byte	.L_LC29-.L_text_b
	.previous
	add 11,11,5
.L_LC30:

	.section	.line
	.4byte	43	 # /systask.cpp:43
	.2byte	0xffff
	.4byte	.L_LC30-.L_text_b
	.previous
	stwx 8,4,0
.L_LC31:

	.section	.line
	.4byte	45	 # /systask.cpp:45
	.2byte	0xffff
	.4byte	.L_LC31-.L_text_b
	.previous
	addi 6,9,8
.L_LC32:

	.section	.line
	.4byte	44	 # /systask.cpp:44
	.2byte	0xffff
	.4byte	.L_LC32-.L_text_b
	.previous
	stwx 31,7,0
.L_LC33:

	.section	.line
	.4byte	46	 # /systask.cpp:46
	.2byte	0xffff
	.4byte	.L_LC33-.L_text_b
	.previous
	addi 5,9,12
.L_LC34:

	.section	.line
	.4byte	45	 # /systask.cpp:45
	.2byte	0xffff
	.4byte	.L_LC34-.L_text_b
	.previous
	stwx 12,6,0
.L_LC35:

	.section	.line
	.4byte	48	 # /systask.cpp:48
	.2byte	0xffff
	.4byte	.L_LC35-.L_text_b
	.previous
	addi 10,10,-1
.L_LC36:

	.section	.line
	.4byte	46	 # /systask.cpp:46
	.2byte	0xffff
	.4byte	.L_LC36-.L_text_b
	.previous
	stwx 11,5,0
.L_LC37:

	.section	.line
	.4byte	48	 # /systask.cpp:48
	.2byte	0xffff
	.4byte	.L_LC37-.L_text_b
	.previous
	stw 10,reentry.3@sda21(0)
.L_B2_e:
.L_b1_e:
	lwz 31,12(1)
	la 1,16(1)
	blr
.L_f1_e:
.L_fSYNCTASK_add__FPFPvi_viiPv_e:
.Lfe1:
	.size	 SYNCTASK_add__FPFPvi_viiPv,.Lfe1-SYNCTASK_add__FPFPvi_viiPv

	.section	.debug_pubnames
	.4byte	.L_P0
	.string	"SYNCTASK_add"
	.previous

	.section	.debug
.L_P0:
.L_D3:
	.4byte	.L_D3_e-.L_D3
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D4
	.2byte	0x38
	.string	"SYNCTASK_add"
	.2byte	0x111
	.4byte	.L_fSYNCTASK_add__FPFPvi_viiPv_s
	.2byte	0x121
	.4byte	.L_fSYNCTASK_add__FPFPvi_viiPv_e
	.2byte	0x8041
	.4byte	.L_b1
	.2byte	0x8051
	.4byte	.L_b1_e
.L_D3_e:
.L_D5:
	.4byte	.L_D5_e-.L_D5
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D6
	.2byte	0x38
	.string	"func"
	.2byte	0x83
	.2byte	.L_t5_e-.L_t5
.L_t5:
	.byte	0x1
	.4byte	.L_T107
.L_t5_e:
	.2byte	0x23
	.2byte	.L_l5_e-.L_l5
.L_l5:
	.byte	0x1
	.4byte	0x3
.L_l5_e:
.L_D5_e:
.L_D6:
	.4byte	.L_D6_e-.L_D6
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D7
	.2byte	0x38
	.string	"reentflag"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l6_e-.L_l6
.L_l6:
	.byte	0x1
	.4byte	0x4
.L_l6_e:
.L_D6_e:
.L_D7:
	.4byte	.L_D7_e-.L_D7
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D8
	.2byte	0x38
	.string	"time"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l7_e-.L_l7
.L_l7:
	.byte	0x1
	.4byte	0x5
.L_l7_e:
.L_D7_e:
.L_D8:
	.4byte	.L_D8_e-.L_D8
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D9
	.2byte	0x38
	.string	"userdata"
	.2byte	0x63
	.2byte	.L_t8_e-.L_t8
.L_t8:
	.byte	0x1
	.2byte	0x14
.L_t8_e:
	.2byte	0x23
	.2byte	.L_l8_e-.L_l8
.L_l8:
	.byte	0x1
	.4byte	0x1f
.L_l8_e:
.L_D8_e:
.L_D9:
	.4byte	.L_D9_e-.L_D9
	.2byte	0x15
	.2byte	0x12
	.4byte	.L_D10
	.set	.L_T107,.L_D9
.L_D9_e:
.L_D11:
	.4byte	.L_D11_e-.L_D11
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D12
	.2byte	0x63
	.2byte	.L_t11_e-.L_t11
.L_t11:
	.byte	0x1
	.2byte	0x14
.L_t11_e:
.L_D11_e:
.L_D12:
	.4byte	.L_D12_e-.L_D12
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D13
	.2byte	0x55
	.2byte	0x7
.L_D12_e:
.L_D13:
	.4byte	0x4
.L_D10:
	.4byte	.L_D10_e-.L_D10
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D14
	.2byte	0x38
	.string	"flag"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l10_e-.L_l10
.L_l10:
	.byte	0x1
	.4byte	0xc
.L_l10_e:
.L_D10_e:
.L_D14:
	.4byte	.L_D14_e-.L_D14
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D15
	.2byte	0x38
	.string	"reentry"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l14_e-.L_l14
.L_l14:
	.byte	0x3
	.4byte	reentry.3
.L_l14_e:
.L_D14_e:
.L_D15:
	.4byte	.L_D15_e-.L_D15
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D16
	.2byte	0x38
	.string	"oldreentry"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l15_e-.L_l15
.L_l15:
	.byte	0x1
	.4byte	0xb
.L_l15_e:
.L_D15_e:
.L_D16:
	.4byte	.L_D16_e-.L_D16
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D17
	.2byte	0x38
	.string	"slot"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l16_e-.L_l16
.L_l16:
	.byte	0x1
	.4byte	0x8
.L_l16_e:
.L_D16_e:
.L_D17:
	.4byte	.L_D17_e-.L_D17
	.2byte	0xb
	.2byte	0x12
	.4byte	.L_D18
	.2byte	0x111
	.4byte	.L_B3
	.2byte	0x121
	.4byte	.L_B3_e
.L_D17_e:
.L_D19:
	.4byte	.L_D19_e-.L_D19
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D20
	.2byte	0x38
	.string	"i"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l19_e-.L_l19
.L_l19:
	.byte	0x1
	.4byte	0xa
.L_l19_e:
.L_D19_e:
.L_D20:
	.4byte	0x4
.L_D18:
	.4byte	0x4
	.previous
	.align 2
	.globl SYNCTASK_del__FPFPvi_v
	.type	 SYNCTASK_del__FPFPvi_v,@function
SYNCTASK_del__FPFPvi_v:
.L_fSYNCTASK_del__FPFPvi_v_s:
.L_LC38:

	.section	.line
	.4byte	51	 # /systask.cpp:51
	.2byte	0xffff
	.4byte	.L_LC38-.L_text_b
	.previous
.L_b2:
.L_B4:
.L_LC39:

	.section	.line
	.4byte	54	 # /systask.cpp:54
	.2byte	0xffff
	.4byte	.L_LC39-.L_text_b
	.previous
	lis 9,systemtasksubs@ha
.L_LC40:

	.section	.line
	.4byte	52	 # /systask.cpp:52
	.2byte	0xffff
	.4byte	.L_LC40-.L_text_b
	.previous
	li 11,0
.L_LC41:

	.section	.line
	.4byte	54	 # /systask.cpp:54
	.2byte	0xffff
	.4byte	.L_LC41-.L_text_b
	.previous
	la 9,systemtasksubs@l(9)
	lwzx 0,9,11
	cmpw 0,0,3
	bc 12,2,.L22        # beq  cr0
.L23:
.L_LC42:

	.section	.line
	.4byte	55	 # /systask.cpp:55
	.2byte	0xffff
	.4byte	.L_LC42-.L_text_b
	.previous
	addi 11,11,1
.L_LC43:

	.section	.line
	.4byte	56	 # /systask.cpp:56
	.2byte	0xffff
	.4byte	.L_LC43-.L_text_b
	.previous
	cmpwi 0,11,15
	bclr 12,1        # gt  cr0
.L_LC44:

	.section	.line
	.4byte	59	 # /systask.cpp:59
	.2byte	0xffff
	.4byte	.L_LC44-.L_text_b
	.previous
	lwzu 0,20(9)
	cmpw 0,0,3
	bc 4,2,.L23        # bne  cr0
.L22:
.L_LC45:

	.section	.line
	.4byte	61	 # /systask.cpp:61
	.2byte	0xffff
	.4byte	.L_LC45-.L_text_b
	.previous
	cmpwi 0,11,15
	bclr 12,1        # gt  cr0
.L_LC46:

	.section	.line
	.4byte	64	 # /systask.cpp:64
	.2byte	0xffff
	.4byte	.L_LC46-.L_text_b
	.previous
	mulli 11,11,20
	lis 9,systemtasksubs@ha
	la 9,systemtasksubs@l(9)
	lwzx 0,9,11
	cmpw 0,0,3
	bclr 4,2        # ne  cr0
.L_LC47:

	.section	.line
	.4byte	65	 # /systask.cpp:65
	.2byte	0xffff
	.4byte	.L_LC47-.L_text_b
	.previous
	li 0,0
	stwx 0,9,11
.L_B4_e:
.L_LC48:

	.section	.line
	.4byte	67	 # /systask.cpp:67
	.2byte	0xffff
	.4byte	.L_LC48-.L_text_b
	.previous
	blr
.L_b2_e:
.L_f2_e:
.L_fSYNCTASK_del__FPFPvi_v_e:
.Lfe2:
	.size	 SYNCTASK_del__FPFPvi_v,.Lfe2-SYNCTASK_del__FPFPvi_v

	.section	.debug_pubnames
	.4byte	.L_P1
	.string	"SYNCTASK_del"
	.previous

	.section	.debug
.L_P1:
.L_D4:
	.4byte	.L_D4_e-.L_D4
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D21
	.2byte	0x38
	.string	"SYNCTASK_del"
	.2byte	0x111
	.4byte	.L_fSYNCTASK_del__FPFPvi_v_s
	.2byte	0x121
	.4byte	.L_fSYNCTASK_del__FPFPvi_v_e
	.2byte	0x8041
	.4byte	.L_b2
	.2byte	0x8051
	.4byte	.L_b2_e
.L_D4_e:
.L_D22:
	.4byte	.L_D22_e-.L_D22
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D23
	.2byte	0x38
	.string	"func"
	.2byte	0x83
	.2byte	.L_t22_e-.L_t22
.L_t22:
	.byte	0x1
	.4byte	.L_T107
.L_t22_e:
	.2byte	0x23
	.2byte	.L_l22_e-.L_l22
.L_l22:
	.byte	0x1
	.4byte	0x3
.L_l22_e:
.L_D22_e:
.L_D23:
	.4byte	.L_D23_e-.L_D23
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D24
	.2byte	0x38
	.string	"i"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l23_e-.L_l23
.L_l23:
	.byte	0x1
	.4byte	0xb
.L_l23_e:
.L_D23_e:
.L_D24:
	.4byte	0x4
	.previous
	.section	".sdata","aw"
	.align 2
	.type	 lastsystemtask.10,@object
	.size	 lastsystemtask.10,4
lastsystemtask.10:
	.long 0
	.section	".text"
	.align 2
	.globl SYNCTASK_run__Fv
	.type	 SYNCTASK_run__Fv,@function
SYNCTASK_run__Fv:
.L_fSYNCTASK_run__Fv_s:
.L_LC49:

	.section	.line
	.4byte	69	 # /systask.cpp:69
	.2byte	0xffff
	.4byte	.L_LC49-.L_text_b
	.previous
	stwu 1,-24(1)
	mflr 0
	stmw 28,8(1)
	stw 0,28(1)
.L_b3:
.L_B5:
.L_LC50:

	.section	.line
	.4byte	70	 # /systask.cpp:70
	.2byte	0xffff
	.4byte	.L_LC50-.L_text_b
	.previous
	li 3,0
	bl THREAD_iscurrent__FP6THREAD
	cmpwi 0,3,0
	bc 12,2,.L29        # beq  cr0
.L_LC51:

	.section	.line
	.4byte	75	 # /systask.cpp:75
	.2byte	0xffff
	.4byte	.L_LC51-.L_text_b
	.previous
.L_B6:
	lwz 0,libticks@sda21(0)
.L_LC52:

	.section	.line
	.4byte	77	 # /systask.cpp:77
	.2byte	0xffff
	.4byte	.L_LC52-.L_text_b
	.previous
	lis 9,systemtasksubs@ha
	la 31,systemtasksubs@l(9)
	li 28,1
.L_LC53:

	.section	.line
	.4byte	75	 # /systask.cpp:75
	.2byte	0xffff
	.4byte	.L_LC53-.L_text_b
	.previous
	stw 0,lastsystemtask.10@sda21(0)
.L_LC54:

	.section	.line
	.4byte	77	 # /systask.cpp:77
	.2byte	0xffff
	.4byte	.L_LC54-.L_text_b
	.previous
	addi 29,31,300
.L34:
.L_B7:
.L_LC55:

	.section	.line
	.4byte	80	 # /systask.cpp:80
	.2byte	0xffff
	.4byte	.L_LC55-.L_text_b
	.previous
.L_B8:
	lwz 9,0(31)
	cmpwi 0,9,0
	bc 12,2,.L33        # beq  cr0
	lwz 4,libticks@sda21(0)
	lwz 0,12(31)
	cmplw 0,4,0
	bc 12,0,.L33        # bllt  cr0
	lwz 30,16(31)
	cmpwi 0,30,0
	bc 4,2,.L33        # bne  cr0
.L_B9:
.L_LC56:

	.section	.line
	.4byte	84	 # /systask.cpp:84
	.2byte	0xffff
	.4byte	.L_LC56-.L_text_b
	.previous
	subf 4,0,4
	mtlr 9
.L_LC57:

	.section	.line
	.4byte	83	 # /systask.cpp:83
	.2byte	0xffff
	.4byte	.L_LC57-.L_text_b
	.previous
	stw 28,16(31)
.L_LC58:

	.section	.line
	.4byte	84	 # /systask.cpp:84
	.2byte	0xffff
	.4byte	.L_LC58-.L_text_b
	.previous
	lwz 3,4(31)
	blrl
.L_LC59:

	.section	.line
	.4byte	86	 # /systask.cpp:86
	.2byte	0xffff
	.4byte	.L_LC59-.L_text_b
	.previous
	lwz 0,libticks@sda21(0)
	lwz 9,8(31)
.L_LC60:

	.section	.line
	.4byte	85	 # /systask.cpp:85
	.2byte	0xffff
	.4byte	.L_LC60-.L_text_b
	.previous
	stw 30,16(31)
.L_LC61:

	.section	.line
	.4byte	86	 # /systask.cpp:86
	.2byte	0xffff
	.4byte	.L_LC61-.L_text_b
	.previous
	add 0,0,9
	stw 0,12(31)
.L_B9_e:
.L_B8_e:
.L_B7_e:
.L_LC62:

	.section	.line
	.4byte	88	 # /systask.cpp:88
	.2byte	0xffff
	.4byte	.L_LC62-.L_text_b
	.previous
.L33:
	addi 31,31,20
	cmpw 0,31,29
	bc 4,1,.L34        # ble  cr0
.L_B6_e:
.L_B5_e:
.L_LC63:

	.section	.line
	.4byte	89	 # /systask.cpp:89
	.2byte	0xffff
	.4byte	.L_LC63-.L_text_b
	.previous
.L29:
.L_b3_e:
	lwz 0,28(1)
	mtlr 0
	lmw 28,8(1)
	la 1,24(1)
	blr
.L_f3_e:
.L_fSYNCTASK_run__Fv_e:
.Lfe3:
	.size	 SYNCTASK_run__Fv,.Lfe3-SYNCTASK_run__Fv

	.section	.debug_pubnames
	.4byte	.L_P2
	.string	"SYNCTASK_run"
	.previous

	.section	.debug
.L_P2:
.L_D21:
	.4byte	.L_D21_e-.L_D21
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D25
	.2byte	0x38
	.string	"SYNCTASK_run"
	.2byte	0x111
	.4byte	.L_fSYNCTASK_run__Fv_s
	.2byte	0x121
	.4byte	.L_fSYNCTASK_run__Fv_e
	.2byte	0x8041
	.4byte	.L_b3
	.2byte	0x8051
	.4byte	.L_b3_e
.L_D21_e:
.L_D26:
	.4byte	.L_D26_e-.L_D26
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D27
	.2byte	0x38
	.string	"lastsystemtask"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l26_e-.L_l26
.L_l26:
	.byte	0x3
	.4byte	lastsystemtask.10
.L_l26_e:
.L_D26_e:
.L_D27:
	.4byte	.L_D27_e-.L_D27
	.2byte	0xb
	.2byte	0x12
	.4byte	.L_D28
	.2byte	0x111
	.4byte	.L_B6
	.2byte	0x121
	.4byte	.L_B6_e
.L_D27_e:
.L_D29:
	.4byte	.L_D29_e-.L_D29
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D30
	.2byte	0x38
	.string	"i"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l29_e-.L_l29
.L_l29:
.L_l29_e:
.L_D29_e:
.L_D30:
	.4byte	.L_D30_e-.L_D30
	.2byte	0xb
	.2byte	0x12
	.4byte	.L_D31
	.2byte	0x111
	.4byte	.L_B7
	.2byte	0x121
	.4byte	.L_B7_e
.L_D30_e:
.L_D32:
	.4byte	.L_D32_e-.L_D32
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D33
	.2byte	0x38
	.string	"pst"
	.2byte	0x83
	.2byte	.L_t32_e-.L_t32
.L_t32:
	.byte	0x1
	.4byte	.L_T105
.L_t32_e:
	.2byte	0x23
	.2byte	.L_l32_e-.L_l32
.L_l32:
.L_l32_e:
.L_D32_e:
.L_D33:
	.4byte	.L_D33_e-.L_D33
	.2byte	0xb
	.2byte	0x12
	.4byte	.L_D34
	.2byte	0x111
	.4byte	.L_B9
	.2byte	0x121
	.4byte	.L_B9_e
.L_D33_e:
.L_D35:
	.4byte	.L_D35_e-.L_D35
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D36
	.2byte	0x38
	.string	"when"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l35_e-.L_l35
.L_l35:
.L_l35_e:
.L_D35_e:
.L_D36:
	.4byte	0x4
.L_D34:
	.4byte	0x4
.L_D31:
	.4byte	0x4
.L_D28:
	.4byte	0x4
.L_D25:
	.4byte	.L_D25_e-.L_D25
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D37
	.set	.L_T105,.L_D25
	.2byte	0x38
	.string	"SYSTEMTASKSUB"
	.2byte	0xb6
	.4byte	0x14
.L_D25_e:
.L_D38:
	.4byte	.L_D38_e-.L_D38
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D39
	.2byte	0x38
	.string	"func"
	.2byte	0x142
	.4byte	.L_T105
	.2byte	0x83
	.2byte	.L_t38_e-.L_t38
.L_t38:
	.byte	0x1
	.4byte	.L_T107
.L_t38_e:
	.2byte	0x23
	.2byte	.L_l38_e-.L_l38
.L_l38:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l38_e:
.L_D38_e:
.L_D39:
	.4byte	.L_D39_e-.L_D39
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D40
	.2byte	0x38
	.string	"userdata"
	.2byte	0x142
	.4byte	.L_T105
	.2byte	0x63
	.2byte	.L_t39_e-.L_t39
.L_t39:
	.byte	0x1
	.2byte	0x14
.L_t39_e:
	.2byte	0x23
	.2byte	.L_l39_e-.L_l39
.L_l39:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l39_e:
.L_D39_e:
.L_D40:
	.4byte	.L_D40_e-.L_D40
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D41
	.2byte	0x38
	.string	"reent"
	.2byte	0x142
	.4byte	.L_T105
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l40_e-.L_l40
.L_l40:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l40_e:
.L_D40_e:
.L_D41:
	.4byte	.L_D41_e-.L_D41
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D42
	.2byte	0x38
	.string	"when"
	.2byte	0x142
	.4byte	.L_T105
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l41_e-.L_l41
.L_l41:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l41_e:
.L_D41_e:
.L_D42:
	.4byte	.L_D42_e-.L_D42
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D43
	.2byte	0x38
	.string	"active"
	.2byte	0x142
	.4byte	.L_T105
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l42_e-.L_l42
.L_l42:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l42_e:
.L_D42_e:
.L_D43:
	.4byte	0x4
	.previous
	.lcomm	systemtasksubs,320,4

	.section	.debug
.L_D37:
	.4byte	.L_D37_e-.L_D37
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D44
	.set	.L_T86,.L_D37
	.2byte	0x38
	.string	"__vtbl_ptr_type"
	.2byte	0xb6
	.4byte	0x8
.L_D37_e:
.L_D45:
	.4byte	.L_D45_e-.L_D45
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D46
	.2byte	0x38
	.string	"__delta"
	.2byte	0x142
	.4byte	.L_T86
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l45_e-.L_l45
.L_l45:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l45_e:
.L_D45_e:
.L_D46:
	.4byte	.L_D46_e-.L_D46
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D47
	.2byte	0x38
	.string	"__index"
	.2byte	0x142
	.4byte	.L_T86
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l46_e-.L_l46
.L_l46:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l46_e:
.L_D46_e:
.L_D47:
	.4byte	.L_D47_e-.L_D47
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D48
	.2byte	0x38
	.string	"__pfn"
	.2byte	0x142
	.4byte	.L_T86
	.2byte	0x63
	.2byte	.L_t47_e-.L_t47
.L_t47:
	.byte	0x1
	.2byte	0x14
.L_t47_e:
	.2byte	0x23
	.2byte	.L_l47_e-.L_l47
.L_l47:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l47_e:
.L_D47_e:
.L_D48:
	.4byte	.L_D48_e-.L_D48
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D49
	.2byte	0x38
	.string	"__delta2"
	.2byte	0x142
	.4byte	.L_T86
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l48_e-.L_l48
.L_l48:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l48_e:
.L_D48_e:
.L_D49:
	.4byte	0x4
.L_D44:
	.4byte	.L_D44_e-.L_D44
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D50
	.2byte	0x38
	.string	"__vtbl_ptr_type"
	.2byte	0x83
	.2byte	.L_t44_e-.L_t44
.L_t44:
	.byte	0x3
	.4byte	.L_T86
.L_t44_e:
.L_D44_e:
	.previous

	.section	.debug
.L_D50:
	.4byte	.L_D50_e-.L_D50
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D51
	.set	.L_T94,.L_D50
	.2byte	0x38
	.string	"bad_alloc"
.L_D50_e:
	.previous

	.section	.debug
.L_D51:
	.4byte	.L_D51_e-.L_D51
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D52
	.set	.L_T101,.L_D51
	.2byte	0x38
	.string	"THREAD"
.L_D51_e:
	.previous

	.section	.debug
	.previous

	.section	.debug
.L_P3:
.L_D52:
	.4byte	.L_D52_e-.L_D52
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D53
	.2byte	0x38
	.string	"libticks"
	.2byte	0x55
	.2byte	0x9
.L_D52_e:
	.previous

	.section	.debug
.L_D53:
	.4byte	.L_D53_e-.L_D53
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D54
	.set	.L_T119,.L_D53
	.2byte	0xa3
	.2byte	.L_s53_e-.L_s53
.L_s53:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0xf
	.byte	0x8
	.2byte	0x72
	.4byte	.L_T105
.L_s53_e:
.L_D53_e:
.L_D54:
	.4byte	.L_D54_e-.L_D54
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D55
	.2byte	0x38
	.string	"systemtasksubs"
	.2byte	0x72
	.4byte	.L_T119
	.2byte	0x23
	.2byte	.L_l54_e-.L_l54
.L_l54:
	.byte	0x3
	.4byte	systemtasksubs
.L_l54_e:
.L_D54_e:
	.previous

	.section	.debug
.L_D55:
	.4byte	.L_D55_e-.L_D55
	.align 2
.L_D55_e:
.L_D2:
	.previous

	.section	.text
.L_text_e:
	.previous

	.section	.data
.L_data_e:
	.previous

	.section	.rodata
.L_rodata_e:
	.previous

	.section	.bss
.L_bss_e:
	.previous

	.section	.line
.L_line_last:
	.4byte	0x0
	.2byte	0xffff
	.4byte	.L_text_e-.L_text_b
.L_line_e:
	.previous

	.section	.debug_srcinfo
	.4byte	.L_line_last-.L_line_b
	.4byte	0xffffffff
	.previous

	.section	.debug_pubnames
	.4byte	0x0
	.string	""
	.previous

	.section	.debug_aranges
	.4byte	.L_text_b
	.4byte	.L_text_e-.L_text_b
	.4byte	.L_data_b
	.4byte	.L_data_e-.L_data_b
	.4byte	.L_rodata_b
	.4byte	.L_rodata_e-.L_rodata_b
	.4byte	.L_bss_b
	.4byte	.L_bss_e-.L_bss_b
	.4byte	0x0
	.4byte	0x0
	.previous
	.ident	"GCC: (GNU) 2.95.3 SN BUILD v1.76 for Nintendo Gamecube"
