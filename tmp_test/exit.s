	.file	"exit.cpp"
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
	.string	"src/Speed/Indep/Libs/realcore/6.24.00/source/system/cmn/exit.cpp"
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

	.section	".data"
	.align 2
	.type	 exitfunctions,@object
	.size	 exitfunctions,256
exitfunctions:
	.long 0
	.space	252
	.section	".text"
	.align 2
	.globl SYSTEM_addexit__FPFv_v
	.type	 SYSTEM_addexit__FPFv_v,@function
SYSTEM_addexit__FPFv_v:
.L_fSYSTEM_addexit__FPFv_v_s:
.L_LC1:


	.section	.debug_sfnames
.L_F0:
	.string	"src/Speed/Indep/Libs/realcore/6.24.00/source/system/cmn/exit.cpp"
	.previous
	.section	.line
.L_LE1:
	.4byte	3	 # /exit.cpp:3
	.2byte	0xffff
	.4byte	.L_LC1-.L_text_b
	.previous

	.section	.debug_srcinfo
	.4byte	.L_LE1-.L_line_b
	.4byte	.L_F0-.L_sfnames_b
	.previous
.L_b1:
.L_B2:
.L_B3:
.L_LC2:

	.section	.line
	.4byte	4	 # /exit.cpp:4
	.2byte	0xffff
	.4byte	.L_LC2-.L_text_b
	.previous
	li 0,64
	lis 11,exitfunctions@ha
	mtctr 0
	la 9,exitfunctions@l(11)
.L19:
.L_LC3:

	.section	.line
	.4byte	5	 # /exit.cpp:5
	.2byte	0xffff
	.4byte	.L_LC3-.L_text_b
	.previous
	lwz 0,0(9)
	addi 9,9,4
	cmpw 0,0,3
	bclr 12,2        # eq  cr0
.L_LC4:

	.section	.line
	.4byte	8	 # /exit.cpp:8
	.2byte	0xffff
	.4byte	.L_LC4-.L_text_b
	.previous
	bdnz .L19
.L_B3_e:
.L_LC5:

	.section	.line
	.4byte	10	 # /exit.cpp:10
	.2byte	0xffff
	.4byte	.L_LC5-.L_text_b
	.previous
	lis 9,exitfunctions@ha
	lwz 0,exitfunctions@l(9)
	cmpwi 0,0,0
	bc 4,2,.L9        # bne  cr0
.L_LC6:

	.section	.line
	.4byte	11	 # /exit.cpp:11
	.2byte	0xffff
	.4byte	.L_LC6-.L_text_b
	.previous
	stw 3,exitfunctions@l(11)
.L_LC7:

	.section	.line
	.4byte	12	 # /exit.cpp:12
	.2byte	0xffff
	.4byte	.L_LC7-.L_text_b
	.previous
	blr
.L17:
.L_LC8:

	.section	.line
	.4byte	17	 # /exit.cpp:17
	.2byte	0xffff
	.4byte	.L_LC8-.L_text_b
	.previous
	stwx 3,10,11
.L_LC9:

	.section	.line
	.4byte	18	 # /exit.cpp:18
	.2byte	0xffff
	.4byte	.L_LC9-.L_text_b
	.previous
	blr
.L9:
.L_LC10:

	.section	.line
	.4byte	15	 # /exit.cpp:15
	.2byte	0xffff
	.4byte	.L_LC10-.L_text_b
	.previous
.L_B4:
	li 0,63
	la 10,exitfunctions@l(9)
	mtctr 0
	li 11,4
	addi 9,10,4
.L18:
.L_LC11:

	.section	.line
	.4byte	16	 # /exit.cpp:16
	.2byte	0xffff
	.4byte	.L_LC11-.L_text_b
	.previous
	lwz 0,0(9)
	addi 9,9,4
	cmpwi 0,0,0
	bc 12,2,.L17        # beq  cr0
.L_LC12:

	.section	.line
	.4byte	20	 # /exit.cpp:20
	.2byte	0xffff
	.4byte	.L_LC12-.L_text_b
	.previous
	addi 11,11,4
	bdnz .L18
.L_B4_e:
.L_B2_e:
.L_LC13:

	.section	.line
	.4byte	21	 # /exit.cpp:21
	.2byte	0xffff
	.4byte	.L_LC13-.L_text_b
	.previous
	blr
.L_b1_e:
.L_f1_e:
.L_fSYSTEM_addexit__FPFv_v_e:
.Lfe1:
	.size	 SYSTEM_addexit__FPFv_v,.Lfe1-SYSTEM_addexit__FPFv_v

	.section	.debug_pubnames
	.4byte	.L_P0
	.string	"SYSTEM_addexit"
	.previous

	.section	.debug
.L_P0:
.L_D3:
	.4byte	.L_D3_e-.L_D3
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D4
	.2byte	0x38
	.string	"SYSTEM_addexit"
	.2byte	0x111
	.4byte	.L_fSYSTEM_addexit__FPFv_v_s
	.2byte	0x121
	.4byte	.L_fSYSTEM_addexit__FPFv_v_e
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
	.4byte	.L_T60
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
	.2byte	0x15
	.2byte	0x12
	.4byte	.L_D7
	.set	.L_T60,.L_D6
.L_D6_e:
.L_D8:
	.4byte	0x4
.L_D7:
	.4byte	.L_D7_e-.L_D7
	.2byte	0xb
	.2byte	0x12
	.4byte	.L_D9
	.2byte	0x111
	.4byte	.L_B3
	.2byte	0x121
	.4byte	.L_B3_e
.L_D7_e:
.L_D10:
	.4byte	.L_D10_e-.L_D10
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D11
	.2byte	0x38
	.string	"i"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l10_e-.L_l10
.L_l10:
.L_l10_e:
.L_D10_e:
.L_D11:
	.4byte	0x4
.L_D9:
	.4byte	.L_D9_e-.L_D9
	.2byte	0xb
	.2byte	0x12
	.4byte	.L_D12
	.2byte	0x111
	.4byte	.L_B4
	.2byte	0x121
	.4byte	.L_B4_e
.L_D9_e:
.L_D13:
	.4byte	.L_D13_e-.L_D13
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D14
	.2byte	0x38
	.string	"i"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l13_e-.L_l13
.L_l13:
.L_l13_e:
.L_D13_e:
.L_D14:
	.4byte	0x4
.L_D12:
	.4byte	0x4
	.previous

	.section	.debug
.L_D4:
	.4byte	.L_D4_e-.L_D4
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D15
	.set	.L_T86,.L_D4
	.2byte	0x38
	.string	"__vtbl_ptr_type"
	.2byte	0xb6
	.4byte	0x8
.L_D4_e:
.L_D16:
	.4byte	.L_D16_e-.L_D16
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D17
	.2byte	0x38
	.string	"__delta"
	.2byte	0x142
	.4byte	.L_T86
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l16_e-.L_l16
.L_l16:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l16_e:
.L_D16_e:
.L_D17:
	.4byte	.L_D17_e-.L_D17
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D18
	.2byte	0x38
	.string	"__index"
	.2byte	0x142
	.4byte	.L_T86
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l17_e-.L_l17
.L_l17:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l17_e:
.L_D17_e:
.L_D18:
	.4byte	.L_D18_e-.L_D18
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D19
	.2byte	0x38
	.string	"__pfn"
	.2byte	0x142
	.4byte	.L_T86
	.2byte	0x63
	.2byte	.L_t18_e-.L_t18
.L_t18:
	.byte	0x1
	.2byte	0x14
.L_t18_e:
	.2byte	0x23
	.2byte	.L_l18_e-.L_l18
.L_l18:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l18_e:
.L_D18_e:
.L_D19:
	.4byte	.L_D19_e-.L_D19
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D20
	.2byte	0x38
	.string	"__delta2"
	.2byte	0x142
	.4byte	.L_T86
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l19_e-.L_l19
.L_l19:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l19_e:
.L_D19_e:
.L_D20:
	.4byte	0x4
.L_D15:
	.4byte	.L_D15_e-.L_D15
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D21
	.2byte	0x38
	.string	"__vtbl_ptr_type"
	.2byte	0x83
	.2byte	.L_t15_e-.L_t15
.L_t15:
	.byte	0x3
	.4byte	.L_T86
.L_t15_e:
.L_D15_e:
	.previous

	.section	.debug
.L_D21:
	.4byte	.L_D21_e-.L_D21
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D22
	.set	.L_T94,.L_D21
	.2byte	0x38
	.string	"bad_alloc"
.L_D21_e:
	.previous

	.section	.debug
.L_D22:
	.4byte	.L_D22_e-.L_D22
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D23
	.set	.L_T105,.L_D22
	.2byte	0xa3
	.2byte	.L_s22_e-.L_s22
.L_s22:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x3f
	.byte	0x8
	.2byte	0x83
	.2byte	.L_t22_e-.L_t22
.L_t22:
	.byte	0x1
	.4byte	.L_T60
.L_t22_e:
.L_s22_e:
.L_D22_e:
.L_D23:
	.4byte	.L_D23_e-.L_D23
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D24
	.2byte	0x38
	.string	"exitfunctions"
	.2byte	0x72
	.4byte	.L_T105
	.2byte	0x23
	.2byte	.L_l23_e-.L_l23
.L_l23:
	.byte	0x3
	.4byte	exitfunctions
.L_l23_e:
.L_D23_e:
	.previous

	.section	.debug
.L_D24:
	.4byte	.L_D24_e-.L_D24
	.align 2
.L_D24_e:
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
