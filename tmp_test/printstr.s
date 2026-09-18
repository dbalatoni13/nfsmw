	.file	"printstr.cpp"
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
	.string	"src/Speed/Indep/Libs/realcore/6.24.00/source/system/debug/cmn/printstr.cpp"
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
	.type	 PRINTchannellist,@object
	.size	 PRINTchannellist,768
PRINTchannellist:
	.long lbl_80414A2C
	.long 1
	.long 1
	.long lbl_80414A3C
	.long 0
	.long 1
	.long lbl_80414A4C
	.long 1
	.long 1
	.long lbl_80414A5C
	.long 0
	.long 1
	.long lbl_80414A6C
	.long 0
	.long 1
	.long lbl_80414A7C
	.long 0
	.long 1
	.long lbl_80414A8C
	.long 0
	.long 1
	.long lbl_80414A9C
	.long 0
	.long 1
	.long lbl_80414AAC
	.long 0
	.long 1
	.long lbl_80414ABC
	.long 0
	.long 1
	.long lbl_80414ACC
	.long 0
	.long 1
	.long lbl_80414ADC
	.long 0
	.long 1
	.long lbl_80414AEC
	.long 0
	.long 1
	.long lbl_80414B00
	.long 0
	.long 1
	.long lbl_80414B14
	.long 0
	.long 1
	.space	588
	.section	".text"
	.align 2
	.globl PRINT_vstring__F12PRINTCHANNELPCcP13__va_list_tag
	.type	 PRINT_vstring__F12PRINTCHANNELPCcP13__va_list_tag,@function
PRINT_vstring__F12PRINTCHANNELPCcP13__va_list_tag:
.L_fPRINT_vstring__F12PRINTCHANNELPCcP13__va_list_tag_s:
.L_LC1:


	.section	.debug_sfnames
.L_F0:
	.string	"src/Speed/Indep/Libs/realcore/6.24.00/source/system/debug/cmn/printstr.cpp"
	.previous
	.section	.line
.L_LE1:
	.4byte	72	 # /printstr.cpp:72
	.2byte	0xffff
	.4byte	.L_LC1-.L_text_b
	.previous

	.section	.debug_srcinfo
	.4byte	.L_LE1-.L_line_b
	.4byte	.L_F0-.L_sfnames_b
	.previous
	stwu 1,-2072(1)
	mflr 0
	stmw 29,2060(1)
	stw 0,2076(1)
.L_b1:
.L_B2:
.L_B3:
	mr 29,3
.L_LC2:

	.section	.line
	.4byte	73	 # /printstr.cpp:73
	.2byte	0xffff
	.4byte	.L_LC2-.L_text_b
	.previous
	lis 9,PRINTchannellist@ha
	mulli 11,29,12
	la 9,PRINTchannellist@l(9)
	addi 9,9,4
.L_LC3:

	.section	.line
	.4byte	72	 # /printstr.cpp:72
	.2byte	0xffff
	.4byte	.L_LC3-.L_text_b
	.previous
	mr 6,5
.L_LC4:

	.section	.line
	.4byte	73	 # /printstr.cpp:73
	.2byte	0xffff
	.4byte	.L_LC4-.L_text_b
	.previous
	lwzx 0,9,11
	cmpwi 0,0,0
	bc 12,2,.L10        # beq  cr0
.L_B4:
.L_LC5:

	.section	.line
	.4byte	76	 # /printstr.cpp:76
	.2byte	0xffff
	.4byte	.L_LC5-.L_text_b
	.previous
.L_B5:
	mr 5,4
	addi 3,1,8
	li 4,2048
.L_LC6:

	.section	.line
	.4byte	79	 # /printstr.cpp:79
	.2byte	0xffff
	.4byte	.L_LC6-.L_text_b
	.previous
	li 30,8
.L_LC7:

	.section	.line
	.4byte	76	 # /printstr.cpp:76
	.2byte	0xffff
	.4byte	.L_LC7-.L_text_b
	.previous
	bl vsnprintf
.L_LC8:

	.section	.line
	.4byte	78	 # /printstr.cpp:78
	.2byte	0xffff
	.4byte	.L_LC8-.L_text_b
	.previous
	lis 9,PRINTdevicelist@ha
	la 31,PRINTdevicelist@l(9)
.L7:
.L_LC9:

	.section	.line
	.4byte	80	 # /printstr.cpp:80
	.2byte	0xffff
	.4byte	.L_LC9-.L_text_b
	.previous
	lwz 0,8(31)
	cmpwi 0,0,0
	bc 12,2,.L8        # beq  cr0
	lwz 0,4(31)
	cmpwi 0,0,0
	bc 12,2,.L8        # beq  cr0
.L_LC10:

	.section	.line
	.4byte	81	 # /printstr.cpp:81
	.2byte	0xffff
	.4byte	.L_LC10-.L_text_b
	.previous
	mr 3,29
	addi 4,1,8
	mtlr 0
	blrl
.L8:
.L_LC11:

	.section	.line
	.4byte	83	 # /printstr.cpp:83
	.2byte	0xffff
	.4byte	.L_LC11-.L_text_b
	.previous
	addi 31,31,12
.L_LC12:

	.section	.line
	.4byte	84	 # /printstr.cpp:84
	.2byte	0xffff
	.4byte	.L_LC12-.L_text_b
	.previous
	addic. 30,30,-1
	bc 4,2,.L7        # bne  cr0
.L_B5_e:
.L_B4_e:
.L_B3_e:
.L_B2_e:
.L10:
.L_b1_e:
	lwz 0,2076(1)
	mtlr 0
	lmw 29,2060(1)
	la 1,2072(1)
	blr
.L_f1_e:
.L_fPRINT_vstring__F12PRINTCHANNELPCcP13__va_list_tag_e:
.Lfe1:
	.size	 PRINT_vstring__F12PRINTCHANNELPCcP13__va_list_tag,.Lfe1-PRINT_vstring__F12PRINTCHANNELPCcP13__va_list_tag

	.section	.debug_pubnames
	.4byte	.L_P0
	.string	"PRINT_vstring"
	.previous

	.section	.debug
.L_P0:
.L_D3:
	.4byte	.L_D3_e-.L_D3
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D4
	.2byte	0x38
	.string	"PRINT_vstring"
	.2byte	0x111
	.4byte	.L_fPRINT_vstring__F12PRINTCHANNELPCcP13__va_list_tag_s
	.2byte	0x121
	.4byte	.L_fPRINT_vstring__F12PRINTCHANNELPCcP13__va_list_tag_e
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
	.string	"channel"
	.2byte	0x72
	.4byte	.L_T406
	.2byte	0x23
	.2byte	.L_l5_e-.L_l5
.L_l5:
	.byte	0x1
	.4byte	0x1d
.L_l5_e:
.L_D5_e:
.L_D6:
	.4byte	.L_D6_e-.L_D6
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D7
	.2byte	0x38
	.string	"format"
	.2byte	0x63
	.2byte	.L_t6_e-.L_t6
.L_t6:
	.byte	0x1
	.byte	0x3
	.2byte	0x1
.L_t6_e:
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
	.string	"args"
	.2byte	0x83
	.2byte	.L_t7_e-.L_t7
.L_t7:
	.byte	0x1
	.4byte	.L_T101
.L_t7_e:
	.2byte	0x23
	.2byte	.L_l7_e-.L_l7
.L_l7:
	.byte	0x1
	.4byte	0x6
.L_l7_e:
.L_D7_e:
.L_D8:
	.4byte	.L_D8_e-.L_D8
	.2byte	0xb
	.2byte	0x12
	.4byte	.L_D9
	.2byte	0x111
	.4byte	.L_B4
	.2byte	0x121
	.4byte	.L_B4_e
.L_D8_e:
.L_D10:
	.4byte	.L_D10_e-.L_D10
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D11
	.set	.L_T468,.L_D10
	.2byte	0xa3
	.2byte	.L_s10_e-.L_s10
.L_s10:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x7ff
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s10_e:
.L_D10_e:
.L_D11:
	.4byte	.L_D11_e-.L_D11
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D12
	.2byte	0x38
	.string	"buffer"
	.2byte	0x72
	.4byte	.L_T468
	.2byte	0x23
	.2byte	.L_l11_e-.L_l11
.L_l11:
	.byte	0x2
	.4byte	0x1
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l11_e:
.L_D11_e:
.L_D12:
	.4byte	.L_D12_e-.L_D12
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D13
	.2byte	0x38
	.string	"pdev"
	.2byte	0x83
	.2byte	.L_t12_e-.L_t12
.L_t12:
	.byte	0x1
	.4byte	.L_T420
.L_t12_e:
	.2byte	0x23
	.2byte	.L_l12_e-.L_l12
.L_l12:
	.byte	0x1
	.4byte	0x1f
.L_l12_e:
.L_D12_e:
.L_D13:
	.4byte	.L_D13_e-.L_D13
	.2byte	0xb
	.2byte	0x12
	.4byte	.L_D14
	.2byte	0x111
	.4byte	.L_B5
	.2byte	0x121
	.4byte	.L_B5_e
.L_D13_e:
.L_D15:
	.4byte	.L_D15_e-.L_D15
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D16
	.2byte	0x38
	.string	"i"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l15_e-.L_l15
.L_l15:
	.byte	0x1
	.4byte	0x1e
.L_l15_e:
.L_D15_e:
.L_D16:
	.4byte	0x4
.L_D14:
	.4byte	0x4
.L_D9:
	.4byte	0x4
.L_D4:
	.4byte	.L_D4_e-.L_D4
	.2byte	0x4
	.2byte	0x12
	.4byte	.L_D17
	.set	.L_T406,.L_D4
	.2byte	0x38
	.string	"PRINTCHANNEL"
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xf4
	.4byte	.L_e4_e-.L_e4
.L_e4:
	.4byte	0xe
	.string	"PRINT_CH_EAGLREND"
	.4byte	0xd
	.string	"PRINT_CH_EAGLANIM"
	.4byte	0xc
	.string	"PRINT_CH_EAGLCORE"
	.4byte	0xb
	.string	"PRINT_CH_INPUT"
	.4byte	0xa
	.string	"PRINT_CH_STREAM"
	.4byte	0x9
	.string	"PRINT_CH_FILE"
	.4byte	0x8
	.string	"PRINT_CH_MEMORY"
	.4byte	0x7
	.string	"PRINT_CH_MATH"
	.4byte	0x6
	.string	"PRINT_CH_CODEC"
	.4byte	0x5
	.string	"PRINT_CH_SYSTEM"
	.4byte	0x4
	.string	"PRINT_CH_STD"
	.4byte	0x3
	.string	"PRINT_CH_DEBUG"
	.4byte	0x2
	.string	"PRINT_CH_ALERT"
	.4byte	0x1
	.string	"PRINT_CH_REAL"
	.4byte	0x0
	.string	"PRINT_CH_ALL"
.L_e4_e:
.L_D4_e:
.L_D17:
	.4byte	.L_D17_e-.L_D17
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D18
	.set	.L_T101,.L_D17
	.2byte	0x38
	.string	"__va_list_tag"
	.2byte	0xb6
	.4byte	0xc
.L_D17_e:
.L_D19:
	.4byte	.L_D19_e-.L_D19
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D20
	.2byte	0x38
	.string	"gpr"
	.2byte	0x142
	.4byte	.L_T101
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l19_e-.L_l19
.L_l19:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l19_e:
.L_D19_e:
.L_D20:
	.4byte	.L_D20_e-.L_D20
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D21
	.2byte	0x38
	.string	"fpr"
	.2byte	0x142
	.4byte	.L_T101
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l20_e-.L_l20
.L_l20:
	.byte	0x4
	.4byte	0x1
	.byte	0x7
.L_l20_e:
.L_D20_e:
.L_D21:
	.4byte	.L_D21_e-.L_D21
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D22
	.2byte	0x38
	.string	"overflow_arg_area"
	.2byte	0x142
	.4byte	.L_T101
	.2byte	0x63
	.2byte	.L_t21_e-.L_t21
.L_t21:
	.byte	0x1
	.2byte	0x1
.L_t21_e:
	.2byte	0x23
	.2byte	.L_l21_e-.L_l21
.L_l21:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l21_e:
.L_D21_e:
.L_D22:
	.4byte	.L_D22_e-.L_D22
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D23
	.2byte	0x38
	.string	"reg_save_area"
	.2byte	0x142
	.4byte	.L_T101
	.2byte	0x63
	.2byte	.L_t22_e-.L_t22
.L_t22:
	.byte	0x1
	.2byte	0x1
.L_t22_e:
	.2byte	0x23
	.2byte	.L_l22_e-.L_l22
.L_l22:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l22_e:
.L_D22_e:
.L_D23:
	.4byte	0x4
.L_D18:
	.4byte	.L_D18_e-.L_D18
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D24
	.set	.L_T420,.L_D18
	.2byte	0x38
	.string	"PRINTDEVICE"
	.2byte	0xb6
	.4byte	0xc
.L_D18_e:
.L_D25:
	.4byte	.L_D25_e-.L_D25
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D26
	.2byte	0x38
	.string	"flags"
	.2byte	0x142
	.4byte	.L_T420
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l25_e-.L_l25
.L_l25:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l25_e:
.L_D25_e:
.L_D26:
	.4byte	.L_D26_e-.L_D26
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D27
	.2byte	0x38
	.string	"func"
	.2byte	0x142
	.4byte	.L_T420
	.2byte	0x83
	.2byte	.L_t26_e-.L_t26
.L_t26:
	.byte	0x1
	.4byte	.L_T407
.L_t26_e:
	.2byte	0x23
	.2byte	.L_l26_e-.L_l26
.L_l26:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l26_e:
.L_D26_e:
.L_D27:
	.4byte	.L_D27_e-.L_D27
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D28
	.2byte	0x38
	.string	"enabled"
	.2byte	0x142
	.4byte	.L_T420
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l27_e-.L_l27
.L_l27:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l27_e:
.L_D27_e:
.L_D28:
	.4byte	0x4
.L_D24:
	.4byte	.L_D24_e-.L_D24
	.2byte	0x15
	.2byte	0x12
	.4byte	.L_D29
	.set	.L_T407,.L_D24
	.2byte	0x55
	.2byte	0x7
.L_D24_e:
.L_D30:
	.4byte	.L_D30_e-.L_D30
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D31
	.2byte	0x72
	.4byte	.L_T406
.L_D30_e:
.L_D31:
	.4byte	.L_D31_e-.L_D31
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D32
	.2byte	0x63
	.2byte	.L_t31_e-.L_t31
.L_t31:
	.byte	0x1
	.byte	0x3
	.2byte	0x1
.L_t31_e:
.L_D31_e:
.L_D32:
	.4byte	0x4
	.previous
	.globl PRINTdevicelist
	.lcomm	PRINTdevicelist,96,4

	.section	.debug
.L_D29:
	.4byte	.L_D29_e-.L_D29
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D33
	.set	.L_T86,.L_D29
	.2byte	0x38
	.string	"__vtbl_ptr_type"
	.2byte	0xb6
	.4byte	0x8
.L_D29_e:
.L_D34:
	.4byte	.L_D34_e-.L_D34
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D35
	.2byte	0x38
	.string	"__delta"
	.2byte	0x142
	.4byte	.L_T86
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l34_e-.L_l34
.L_l34:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l34_e:
.L_D34_e:
.L_D35:
	.4byte	.L_D35_e-.L_D35
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D36
	.2byte	0x38
	.string	"__index"
	.2byte	0x142
	.4byte	.L_T86
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l35_e-.L_l35
.L_l35:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l35_e:
.L_D35_e:
.L_D36:
	.4byte	.L_D36_e-.L_D36
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D37
	.2byte	0x38
	.string	"__pfn"
	.2byte	0x142
	.4byte	.L_T86
	.2byte	0x63
	.2byte	.L_t36_e-.L_t36
.L_t36:
	.byte	0x1
	.2byte	0x14
.L_t36_e:
	.2byte	0x23
	.2byte	.L_l36_e-.L_l36
.L_l36:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l36_e:
.L_D36_e:
.L_D37:
	.4byte	.L_D37_e-.L_D37
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D38
	.2byte	0x38
	.string	"__delta2"
	.2byte	0x142
	.4byte	.L_T86
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l37_e-.L_l37
.L_l37:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l37_e:
.L_D37_e:
.L_D38:
	.4byte	0x4
.L_D33:
	.4byte	.L_D33_e-.L_D33
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D39
	.2byte	0x38
	.string	"__vtbl_ptr_type"
	.2byte	0x83
	.2byte	.L_t33_e-.L_t33
.L_t33:
	.byte	0x3
	.4byte	.L_T86
.L_t33_e:
.L_D33_e:
	.previous

	.section	.debug
.L_D39:
	.4byte	.L_D39_e-.L_D39
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D40
	.set	.L_T94,.L_D39
	.2byte	0x38
	.string	"bad_alloc"
.L_D39_e:
	.previous

	.section	.debug
	.previous

	.section	.debug
.L_D40:
	.4byte	.L_D40_e-.L_D40
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D41
	.set	.L_T113,.L_D40
	.2byte	0xa3
	.2byte	.L_s40_e-.L_s40
.L_s40:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x72
	.4byte	.L_T101
.L_s40_e:
.L_D40_e:
.L_D41:
	.4byte	.L_D41_e-.L_D41
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D42
	.2byte	0x38
	.string	"__va_list"
	.2byte	0x72
	.4byte	.L_T113
.L_D41_e:
	.previous

	.section	.debug
.L_D42:
	.4byte	.L_D42_e-.L_D42
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D43
	.2byte	0x38
	.string	"__gnuc_va_list"
	.2byte	0x72
	.4byte	.L_T113
.L_D42_e:
	.previous

	.section	.debug
.L_D43:
	.4byte	.L_D43_e-.L_D43
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D44
	.set	.L_T118,.L_D43
	.2byte	0x38
	.string	"__va_regsave_t"
	.2byte	0xb6
	.4byte	0x60
.L_D43_e:
.L_D45:
	.4byte	.L_D45_e-.L_D45
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D46
	.2byte	0x38
	.string	"__gp_save"
	.2byte	0x142
	.4byte	.L_T118
	.2byte	0x72
	.4byte	.L_T122
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
	.string	"__fp_save"
	.2byte	0x142
	.4byte	.L_T118
	.2byte	0x72
	.4byte	.L_T125
	.2byte	0x23
	.2byte	.L_l46_e-.L_l46
.L_l46:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l46_e:
.L_D46_e:
.L_D47:
	.4byte	.L_D47_e-.L_D47
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D48
	.2byte	0x38
	.string	"._0"
	.2byte	0x142
	.4byte	.L_T118
	.2byte	0x72
	.4byte	.L_T118
.L_D47_e:
.L_D48:
	.4byte	0x4
.L_D44:
	.4byte	.L_D44_e-.L_D44
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D49
	.2byte	0x38
	.string	"__va_regsave_t"
	.2byte	0x72
	.4byte	.L_T118
.L_D44_e:
.L_D49:
	.4byte	.L_D49_e-.L_D49
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D50
	.set	.L_T122,.L_D49
	.2byte	0xa3
	.2byte	.L_s49_e-.L_s49
.L_s49:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x7
	.byte	0x8
	.2byte	0x55
	.2byte	0xa
.L_s49_e:
.L_D49_e:
.L_D50:
	.4byte	.L_D50_e-.L_D50
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D51
	.set	.L_T125,.L_D50
	.2byte	0xa3
	.2byte	.L_s50_e-.L_s50
.L_s50:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x7
	.byte	0x8
	.2byte	0x55
	.2byte	0xf
.L_s50_e:
.L_D50_e:
	.previous

	.section	.debug
.L_D51:
	.4byte	.L_D51_e-.L_D51
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D52
	.2byte	0x38
	.string	"va_list"
	.2byte	0x72
	.4byte	.L_T113
.L_D51_e:
	.previous

	.section	.debug
.L_D52:
	.4byte	.L_D52_e-.L_D52
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D53
	.2byte	0x38
	.string	"__int32_t"
	.2byte	0x55
	.2byte	0x7
.L_D52_e:
	.previous

	.section	.debug
.L_D53:
	.4byte	.L_D53_e-.L_D53
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D54
	.2byte	0x38
	.string	"__uint32_t"
	.2byte	0x55
	.2byte	0x9
.L_D53_e:
	.previous

	.section	.debug
.L_D54:
	.4byte	.L_D54_e-.L_D54
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D55
	.2byte	0x38
	.string	"size_t"
	.2byte	0x55
	.2byte	0x9
.L_D54_e:
	.previous

	.section	.debug
.L_D55:
	.4byte	.L_D55_e-.L_D55
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D56
	.2byte	0x38
	.string	"clock_t"
	.2byte	0x55
	.2byte	0x9
.L_D55_e:
	.previous

	.section	.debug
.L_D56:
	.4byte	.L_D56_e-.L_D56
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D57
	.2byte	0x38
	.string	"time_t"
	.2byte	0x55
	.2byte	0x7
.L_D56_e:
	.previous

	.section	.debug
.L_D57:
	.4byte	.L_D57_e-.L_D57
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D58
	.set	.L_T141,.L_D57
	.2byte	0x38
	.string	"tm"
	.2byte	0xb6
	.4byte	0x24
.L_D57_e:
.L_D59:
	.4byte	.L_D59_e-.L_D59
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D60
	.2byte	0x38
	.string	"tm_sec"
	.2byte	0x142
	.4byte	.L_T141
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l59_e-.L_l59
.L_l59:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l59_e:
.L_D59_e:
.L_D60:
	.4byte	.L_D60_e-.L_D60
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D61
	.2byte	0x38
	.string	"tm_min"
	.2byte	0x142
	.4byte	.L_T141
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l60_e-.L_l60
.L_l60:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l60_e:
.L_D60_e:
.L_D61:
	.4byte	.L_D61_e-.L_D61
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D62
	.2byte	0x38
	.string	"tm_hour"
	.2byte	0x142
	.4byte	.L_T141
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l61_e-.L_l61
.L_l61:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l61_e:
.L_D61_e:
.L_D62:
	.4byte	.L_D62_e-.L_D62
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D63
	.2byte	0x38
	.string	"tm_mday"
	.2byte	0x142
	.4byte	.L_T141
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l62_e-.L_l62
.L_l62:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l62_e:
.L_D62_e:
.L_D63:
	.4byte	.L_D63_e-.L_D63
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D64
	.2byte	0x38
	.string	"tm_mon"
	.2byte	0x142
	.4byte	.L_T141
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l63_e-.L_l63
.L_l63:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l63_e:
.L_D63_e:
.L_D64:
	.4byte	.L_D64_e-.L_D64
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D65
	.2byte	0x38
	.string	"tm_year"
	.2byte	0x142
	.4byte	.L_T141
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l64_e-.L_l64
.L_l64:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l64_e:
.L_D64_e:
.L_D65:
	.4byte	.L_D65_e-.L_D65
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D66
	.2byte	0x38
	.string	"tm_wday"
	.2byte	0x142
	.4byte	.L_T141
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l65_e-.L_l65
.L_l65:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l65_e:
.L_D65_e:
.L_D66:
	.4byte	.L_D66_e-.L_D66
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D67
	.2byte	0x38
	.string	"tm_yday"
	.2byte	0x142
	.4byte	.L_T141
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l66_e-.L_l66
.L_l66:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l66_e:
.L_D66_e:
.L_D67:
	.4byte	.L_D67_e-.L_D67
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D68
	.2byte	0x38
	.string	"tm_isdst"
	.2byte	0x142
	.4byte	.L_T141
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l67_e-.L_l67
.L_l67:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l67_e:
.L_D67_e:
.L_D68:
	.4byte	0x4
	.previous

	.section	.debug
.L_D58:
	.4byte	.L_D58_e-.L_D58
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D69
	.2byte	0x38
	.string	"ULong"
	.2byte	0x55
	.2byte	0x9
.L_D58_e:
	.previous

	.section	.debug
.L_D69:
	.4byte	.L_D69_e-.L_D69
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D70
	.set	.L_T170,.L_D69
	.2byte	0x38
	.string	"_glue"
	.2byte	0xb6
	.4byte	0xc
.L_D69_e:
.L_D71:
	.4byte	.L_D71_e-.L_D71
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D72
	.2byte	0x38
	.string	"_next"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x83
	.2byte	.L_t71_e-.L_t71
.L_t71:
	.byte	0x1
	.4byte	.L_T170
.L_t71_e:
	.2byte	0x23
	.2byte	.L_l71_e-.L_l71
.L_l71:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l71_e:
.L_D71_e:
.L_D72:
	.4byte	.L_D72_e-.L_D72
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D73
	.2byte	0x38
	.string	"_niobs"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l72_e-.L_l72
.L_l72:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l72_e:
.L_D72_e:
.L_D73:
	.4byte	.L_D73_e-.L_D73
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D74
	.2byte	0x38
	.string	"_iobs"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x83
	.2byte	.L_t73_e-.L_t73
.L_t73:
	.byte	0x1
	.4byte	.L_T172
.L_t73_e:
	.2byte	0x23
	.2byte	.L_l73_e-.L_l73
.L_l73:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l73_e:
.L_D73_e:
.L_D74:
	.4byte	0x4
.L_D70:
	.4byte	.L_D70_e-.L_D70
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D75
	.set	.L_T172,.L_D70
	.2byte	0x38
	.string	"__sFILE"
	.2byte	0xb6
	.4byte	0x60
.L_D70_e:
.L_D76:
	.4byte	.L_D76_e-.L_D76
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D77
	.2byte	0x38
	.string	"_p"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x63
	.2byte	.L_t76_e-.L_t76
.L_t76:
	.byte	0x1
	.2byte	0x3
.L_t76_e:
	.2byte	0x23
	.2byte	.L_l76_e-.L_l76
.L_l76:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l76_e:
.L_D76_e:
.L_D77:
	.4byte	.L_D77_e-.L_D77
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D78
	.2byte	0x38
	.string	"_r"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l77_e-.L_l77
.L_l77:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l77_e:
.L_D77_e:
.L_D78:
	.4byte	.L_D78_e-.L_D78
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D79
	.2byte	0x38
	.string	"_w"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l78_e-.L_l78
.L_l78:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l78_e:
.L_D78_e:
.L_D79:
	.4byte	.L_D79_e-.L_D79
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D80
	.2byte	0x38
	.string	"_flags"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l79_e-.L_l79
.L_l79:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l79_e:
.L_D79_e:
.L_D80:
	.4byte	.L_D80_e-.L_D80
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D81
	.2byte	0x38
	.string	"_file"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l80_e-.L_l80
.L_l80:
	.byte	0x4
	.4byte	0xe
	.byte	0x7
.L_l80_e:
.L_D80_e:
.L_D81:
	.4byte	.L_D81_e-.L_D81
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D82
	.2byte	0x38
	.string	"_bf"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x72
	.4byte	.L_T213
	.2byte	0x23
	.2byte	.L_l81_e-.L_l81
.L_l81:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l81_e:
.L_D81_e:
.L_D82:
	.4byte	.L_D82_e-.L_D82
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D83
	.2byte	0x38
	.string	"_lbfsize"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l82_e-.L_l82
.L_l82:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l82_e:
.L_D82_e:
.L_D83:
	.4byte	.L_D83_e-.L_D83
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D84
	.2byte	0x38
	.string	"_cookie"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x63
	.2byte	.L_t83_e-.L_t83
.L_t83:
	.byte	0x1
	.2byte	0x14
.L_t83_e:
	.2byte	0x23
	.2byte	.L_l83_e-.L_l83
.L_l83:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l83_e:
.L_D83_e:
.L_D84:
	.4byte	.L_D84_e-.L_D84
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D85
	.2byte	0x38
	.string	"_read"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x83
	.2byte	.L_t84_e-.L_t84
.L_t84:
	.byte	0x1
	.4byte	.L_T226
.L_t84_e:
	.2byte	0x23
	.2byte	.L_l84_e-.L_l84
.L_l84:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l84_e:
.L_D84_e:
.L_D85:
	.4byte	.L_D85_e-.L_D85
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D86
	.2byte	0x38
	.string	"_write"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x83
	.2byte	.L_t85_e-.L_t85
.L_t85:
	.byte	0x1
	.4byte	.L_T228
.L_t85_e:
	.2byte	0x23
	.2byte	.L_l85_e-.L_l85
.L_l85:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l85_e:
.L_D85_e:
.L_D86:
	.4byte	.L_D86_e-.L_D86
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D87
	.2byte	0x38
	.string	"_seek"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x83
	.2byte	.L_t86_e-.L_t86
.L_t86:
	.byte	0x1
	.4byte	.L_T230
.L_t86_e:
	.2byte	0x23
	.2byte	.L_l86_e-.L_l86
.L_l86:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l86_e:
.L_D86_e:
.L_D87:
	.4byte	.L_D87_e-.L_D87
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D88
	.2byte	0x38
	.string	"_close"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x83
	.2byte	.L_t87_e-.L_t87
.L_t87:
	.byte	0x1
	.4byte	.L_T75
.L_t87_e:
	.2byte	0x23
	.2byte	.L_l87_e-.L_l87
.L_l87:
	.byte	0x4
	.4byte	0x2c
	.byte	0x7
.L_l87_e:
.L_D87_e:
.L_D88:
	.4byte	.L_D88_e-.L_D88
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D89
	.2byte	0x38
	.string	"_ub"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x72
	.4byte	.L_T213
	.2byte	0x23
	.2byte	.L_l88_e-.L_l88
.L_l88:
	.byte	0x4
	.4byte	0x30
	.byte	0x7
.L_l88_e:
.L_D88_e:
.L_D89:
	.4byte	.L_D89_e-.L_D89
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D90
	.2byte	0x38
	.string	"_up"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x63
	.2byte	.L_t89_e-.L_t89
.L_t89:
	.byte	0x1
	.2byte	0x3
.L_t89_e:
	.2byte	0x23
	.2byte	.L_l89_e-.L_l89
.L_l89:
	.byte	0x4
	.4byte	0x38
	.byte	0x7
.L_l89_e:
.L_D89_e:
.L_D90:
	.4byte	.L_D90_e-.L_D90
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D91
	.2byte	0x38
	.string	"_ur"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l90_e-.L_l90
.L_l90:
	.byte	0x4
	.4byte	0x3c
	.byte	0x7
.L_l90_e:
.L_D90_e:
.L_D91:
	.4byte	.L_D91_e-.L_D91
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D92
	.2byte	0x38
	.string	"_ubuf"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x72
	.4byte	.L_T235
	.2byte	0x23
	.2byte	.L_l91_e-.L_l91
.L_l91:
	.byte	0x4
	.4byte	0x40
	.byte	0x7
.L_l91_e:
.L_D91_e:
.L_D92:
	.4byte	.L_D92_e-.L_D92
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D93
	.2byte	0x38
	.string	"_nbuf"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x72
	.4byte	.L_T237
	.2byte	0x23
	.2byte	.L_l92_e-.L_l92
.L_l92:
	.byte	0x4
	.4byte	0x4a
	.byte	0x7
.L_l92_e:
.L_D92_e:
.L_D93:
	.4byte	.L_D93_e-.L_D93
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D94
	.2byte	0x38
	.string	"_lb"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x72
	.4byte	.L_T213
	.2byte	0x23
	.2byte	.L_l93_e-.L_l93
.L_l93:
	.byte	0x4
	.4byte	0x4c
	.byte	0x7
.L_l93_e:
.L_D93_e:
.L_D94:
	.4byte	.L_D94_e-.L_D94
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D95
	.2byte	0x38
	.string	"_blksize"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l94_e-.L_l94
.L_l94:
	.byte	0x4
	.4byte	0x54
	.byte	0x7
.L_l94_e:
.L_D94_e:
.L_D95:
	.4byte	.L_D95_e-.L_D95
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D96
	.2byte	0x38
	.string	"_offset"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l95_e-.L_l95
.L_l95:
	.byte	0x4
	.4byte	0x58
	.byte	0x7
.L_l95_e:
.L_D95_e:
.L_D96:
	.4byte	.L_D96_e-.L_D96
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D97
	.2byte	0x38
	.string	"_data"
	.2byte	0x142
	.4byte	.L_T172
	.2byte	0x83
	.2byte	.L_t96_e-.L_t96
.L_t96:
	.byte	0x1
	.4byte	.L_T238
.L_t96_e:
	.2byte	0x23
	.2byte	.L_l96_e-.L_l96
.L_l96:
	.byte	0x4
	.4byte	0x5c
	.byte	0x7
.L_l96_e:
.L_D96_e:
.L_D97:
	.4byte	0x4
.L_D75:
	.4byte	.L_D75_e-.L_D75
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D98
	.set	.L_T213,.L_D75
	.2byte	0x38
	.string	"__sbuf"
	.2byte	0xb6
	.4byte	0x8
.L_D75_e:
.L_D99:
	.4byte	.L_D99_e-.L_D99
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D100
	.2byte	0x38
	.string	"_base"
	.2byte	0x142
	.4byte	.L_T213
	.2byte	0x63
	.2byte	.L_t99_e-.L_t99
.L_t99:
	.byte	0x1
	.2byte	0x3
.L_t99_e:
	.2byte	0x23
	.2byte	.L_l99_e-.L_l99
.L_l99:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l99_e:
.L_D99_e:
.L_D100:
	.4byte	.L_D100_e-.L_D100
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D101
	.2byte	0x38
	.string	"_size"
	.2byte	0x142
	.4byte	.L_T213
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l100_e-.L_l100
.L_l100:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l100_e:
.L_D100_e:
.L_D101:
	.4byte	0x4
.L_D98:
	.4byte	.L_D98_e-.L_D98
	.2byte	0x15
	.2byte	0x12
	.4byte	.L_D102
	.set	.L_T226,.L_D98
	.2byte	0x55
	.2byte	0x7
.L_D98_e:
.L_D103:
	.4byte	.L_D103_e-.L_D103
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D104
	.2byte	0x63
	.2byte	.L_t103_e-.L_t103
.L_t103:
	.byte	0x1
	.2byte	0x14
.L_t103_e:
.L_D103_e:
.L_D104:
	.4byte	.L_D104_e-.L_D104
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D105
	.2byte	0x63
	.2byte	.L_t104_e-.L_t104
.L_t104:
	.byte	0x1
	.2byte	0x1
.L_t104_e:
.L_D104_e:
.L_D105:
	.4byte	.L_D105_e-.L_D105
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D106
	.2byte	0x55
	.2byte	0x7
.L_D105_e:
.L_D106:
	.4byte	0x4
.L_D102:
	.4byte	.L_D102_e-.L_D102
	.2byte	0x15
	.2byte	0x12
	.4byte	.L_D107
	.set	.L_T228,.L_D102
	.2byte	0x55
	.2byte	0x7
.L_D102_e:
.L_D108:
	.4byte	.L_D108_e-.L_D108
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D109
	.2byte	0x63
	.2byte	.L_t108_e-.L_t108
.L_t108:
	.byte	0x1
	.2byte	0x14
.L_t108_e:
.L_D108_e:
.L_D109:
	.4byte	.L_D109_e-.L_D109
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D110
	.2byte	0x63
	.2byte	.L_t109_e-.L_t109
.L_t109:
	.byte	0x1
	.byte	0x3
	.2byte	0x1
.L_t109_e:
.L_D109_e:
.L_D110:
	.4byte	.L_D110_e-.L_D110
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D111
	.2byte	0x55
	.2byte	0x7
.L_D110_e:
.L_D111:
	.4byte	0x4
.L_D107:
	.4byte	.L_D107_e-.L_D107
	.2byte	0x15
	.2byte	0x12
	.4byte	.L_D112
	.set	.L_T230,.L_D107
	.2byte	0x55
	.2byte	0x7
.L_D107_e:
.L_D113:
	.4byte	.L_D113_e-.L_D113
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D114
	.2byte	0x63
	.2byte	.L_t113_e-.L_t113
.L_t113:
	.byte	0x1
	.2byte	0x14
.L_t113_e:
.L_D113_e:
.L_D114:
	.4byte	.L_D114_e-.L_D114
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D115
	.2byte	0x55
	.2byte	0xa
.L_D114_e:
.L_D115:
	.4byte	.L_D115_e-.L_D115
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D116
	.2byte	0x55
	.2byte	0x7
.L_D115_e:
.L_D116:
	.4byte	0x4
.L_D112:
	.4byte	.L_D112_e-.L_D112
	.2byte	0x15
	.2byte	0x12
	.4byte	.L_D117
	.set	.L_T75,.L_D112
	.2byte	0x55
	.2byte	0x7
.L_D112_e:
.L_D118:
	.4byte	.L_D118_e-.L_D118
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D119
	.2byte	0x63
	.2byte	.L_t118_e-.L_t118
.L_t118:
	.byte	0x1
	.2byte	0x14
.L_t118_e:
.L_D118_e:
.L_D119:
	.4byte	0x4
.L_D117:
	.4byte	.L_D117_e-.L_D117
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D120
	.set	.L_T235,.L_D117
	.2byte	0xa3
	.2byte	.L_s117_e-.L_s117
.L_s117:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x9
	.byte	0x8
	.2byte	0x55
	.2byte	0x3
.L_s117_e:
.L_D117_e:
.L_D120:
	.4byte	.L_D120_e-.L_D120
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D121
	.set	.L_T237,.L_D120
	.2byte	0xa3
	.2byte	.L_s120_e-.L_s120
.L_s120:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x3
.L_s120_e:
.L_D120_e:
.L_D121:
	.4byte	.L_D121_e-.L_D121
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D122
	.set	.L_T238,.L_D121
	.2byte	0x38
	.string	"_reent"
	.2byte	0xb6
	.4byte	0x304
.L_D121_e:
.L_D123:
	.4byte	.L_D123_e-.L_D123
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D124
	.2byte	0x38
	.string	"_errno"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l123_e-.L_l123
.L_l123:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l123_e:
.L_D123_e:
.L_D124:
	.4byte	.L_D124_e-.L_D124
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D125
	.2byte	0x38
	.string	"_stdin"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x83
	.2byte	.L_t124_e-.L_t124
.L_t124:
	.byte	0x1
	.4byte	.L_T172
.L_t124_e:
	.2byte	0x23
	.2byte	.L_l124_e-.L_l124
.L_l124:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l124_e:
.L_D124_e:
.L_D125:
	.4byte	.L_D125_e-.L_D125
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D126
	.2byte	0x38
	.string	"_stdout"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x83
	.2byte	.L_t125_e-.L_t125
.L_t125:
	.byte	0x1
	.4byte	.L_T172
.L_t125_e:
	.2byte	0x23
	.2byte	.L_l125_e-.L_l125
.L_l125:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l125_e:
.L_D125_e:
.L_D126:
	.4byte	.L_D126_e-.L_D126
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D127
	.2byte	0x38
	.string	"_stderr"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x83
	.2byte	.L_t126_e-.L_t126
.L_t126:
	.byte	0x1
	.4byte	.L_T172
.L_t126_e:
	.2byte	0x23
	.2byte	.L_l126_e-.L_l126
.L_l126:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l126_e:
.L_D126_e:
.L_D127:
	.4byte	.L_D127_e-.L_D127
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D128
	.2byte	0x38
	.string	"_inc"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l127_e-.L_l127
.L_l127:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l127_e:
.L_D127_e:
.L_D128:
	.4byte	.L_D128_e-.L_D128
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D129
	.2byte	0x38
	.string	"_emergency"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x72
	.4byte	.L_T250
	.2byte	0x23
	.2byte	.L_l128_e-.L_l128
.L_l128:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l128_e:
.L_D128_e:
.L_D129:
	.4byte	.L_D129_e-.L_D129
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D130
	.2byte	0x38
	.string	"_current_category"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l129_e-.L_l129
.L_l129:
	.byte	0x4
	.4byte	0x30
	.byte	0x7
.L_l129_e:
.L_D129_e:
.L_D130:
	.4byte	.L_D130_e-.L_D130
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D131
	.2byte	0x38
	.string	"_current_locale"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x63
	.2byte	.L_t130_e-.L_t130
.L_t130:
	.byte	0x1
	.byte	0x3
	.2byte	0x1
.L_t130_e:
	.2byte	0x23
	.2byte	.L_l130_e-.L_l130
.L_l130:
	.byte	0x4
	.4byte	0x34
	.byte	0x7
.L_l130_e:
.L_D130_e:
.L_D131:
	.4byte	.L_D131_e-.L_D131
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D132
	.2byte	0x38
	.string	"__sdidinit"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l131_e-.L_l131
.L_l131:
	.byte	0x4
	.4byte	0x38
	.byte	0x7
.L_l131_e:
.L_D131_e:
.L_D132:
	.4byte	.L_D132_e-.L_D132
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D133
	.2byte	0x38
	.string	"__cleanup"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x83
	.2byte	.L_t132_e-.L_t132
.L_t132:
	.byte	0x1
	.4byte	.L_T251
.L_t132_e:
	.2byte	0x23
	.2byte	.L_l132_e-.L_l132
.L_l132:
	.byte	0x4
	.4byte	0x3c
	.byte	0x7
.L_l132_e:
.L_D132_e:
.L_D133:
	.4byte	.L_D133_e-.L_D133
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D134
	.2byte	0x38
	.string	"_result"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x83
	.2byte	.L_t133_e-.L_t133
.L_t133:
	.byte	0x1
	.4byte	.L_T183
.L_t133_e:
	.2byte	0x23
	.2byte	.L_l133_e-.L_l133
.L_l133:
	.byte	0x4
	.4byte	0x40
	.byte	0x7
.L_l133_e:
.L_D133_e:
.L_D134:
	.4byte	.L_D134_e-.L_D134
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D135
	.2byte	0x38
	.string	"_result_k"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l134_e-.L_l134
.L_l134:
	.byte	0x4
	.4byte	0x44
	.byte	0x7
.L_l134_e:
.L_D134_e:
.L_D135:
	.4byte	.L_D135_e-.L_D135
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D136
	.2byte	0x38
	.string	"_p5s"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x83
	.2byte	.L_t135_e-.L_t135
.L_t135:
	.byte	0x1
	.4byte	.L_T183
.L_t135_e:
	.2byte	0x23
	.2byte	.L_l135_e-.L_l135
.L_l135:
	.byte	0x4
	.4byte	0x48
	.byte	0x7
.L_l135_e:
.L_D135_e:
.L_D136:
	.4byte	.L_D136_e-.L_D136
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D137
	.2byte	0x38
	.string	"_freelist"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x83
	.2byte	.L_t136_e-.L_t136
.L_t136:
	.byte	0x1
	.byte	0x1
	.4byte	.L_T183
.L_t136_e:
	.2byte	0x23
	.2byte	.L_l136_e-.L_l136
.L_l136:
	.byte	0x4
	.4byte	0x4c
	.byte	0x7
.L_l136_e:
.L_D136_e:
.L_D137:
	.4byte	.L_D137_e-.L_D137
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D138
	.2byte	0x38
	.string	"_cvtlen"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l137_e-.L_l137
.L_l137:
	.byte	0x4
	.4byte	0x50
	.byte	0x7
.L_l137_e:
.L_D137_e:
.L_D138:
	.4byte	.L_D138_e-.L_D138
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D139
	.2byte	0x38
	.string	"_cvtbuf"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x63
	.2byte	.L_t138_e-.L_t138
.L_t138:
	.byte	0x1
	.2byte	0x1
.L_t138_e:
	.2byte	0x23
	.2byte	.L_l138_e-.L_l138
.L_l138:
	.byte	0x4
	.4byte	0x54
	.byte	0x7
.L_l138_e:
.L_D138_e:
.L_D139:
	.4byte	.L_D139_e-.L_D139
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D140
	.set	.L_T254,.L_D139
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0xb6
	.4byte	0xf0
.L_D139_e:
.L_D141:
	.4byte	.L_D141_e-.L_D141
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D142
	.set	.L_T256,.L_D141
	.2byte	0x142
	.4byte	.L_T254
	.2byte	0xb6
	.4byte	0x4c
.L_D141_e:
.L_D143:
	.4byte	.L_D143_e-.L_D143
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D144
	.2byte	0x38
	.string	"_rand_next"
	.2byte	0x142
	.4byte	.L_T256
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l143_e-.L_l143
.L_l143:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l143_e:
.L_D143_e:
.L_D144:
	.4byte	.L_D144_e-.L_D144
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D145
	.2byte	0x38
	.string	"_strtok_last"
	.2byte	0x142
	.4byte	.L_T256
	.2byte	0x63
	.2byte	.L_t144_e-.L_t144
.L_t144:
	.byte	0x1
	.2byte	0x1
.L_t144_e:
	.2byte	0x23
	.2byte	.L_l144_e-.L_l144
.L_l144:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l144_e:
.L_D144_e:
.L_D145:
	.4byte	.L_D145_e-.L_D145
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D146
	.2byte	0x38
	.string	"_asctime_buf"
	.2byte	0x142
	.4byte	.L_T256
	.2byte	0x72
	.4byte	.L_T259
	.2byte	0x23
	.2byte	.L_l145_e-.L_l145
.L_l145:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l145_e:
.L_D145_e:
.L_D146:
	.4byte	.L_D146_e-.L_D146
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D147
	.2byte	0x38
	.string	"_localtime_buf"
	.2byte	0x142
	.4byte	.L_T256
	.2byte	0x72
	.4byte	.L_T141
	.2byte	0x23
	.2byte	.L_l146_e-.L_l146
.L_l146:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l146_e:
.L_D146_e:
.L_D147:
	.4byte	.L_D147_e-.L_D147
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D148
	.2byte	0x38
	.string	"_gamma_signgam"
	.2byte	0x142
	.4byte	.L_T256
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l147_e-.L_l147
.L_l147:
	.byte	0x4
	.4byte	0x48
	.byte	0x7
.L_l147_e:
.L_D147_e:
.L_D148:
	.4byte	0x4
.L_D142:
	.4byte	.L_D142_e-.L_D142
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D149
	.2byte	0x38
	.string	"_reent"
	.2byte	0x142
	.4byte	.L_T254
	.2byte	0x72
	.4byte	.L_T256
	.2byte	0x23
	.2byte	.L_l142_e-.L_l142
.L_l142:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l142_e:
.L_D142_e:
.L_D149:
	.4byte	.L_D149_e-.L_D149
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D150
	.set	.L_T269,.L_D149
	.2byte	0x142
	.4byte	.L_T254
	.2byte	0xb6
	.4byte	0xf0
.L_D149_e:
.L_D151:
	.4byte	.L_D151_e-.L_D151
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D152
	.2byte	0x38
	.string	"_nextf"
	.2byte	0x142
	.4byte	.L_T269
	.2byte	0x72
	.4byte	.L_T273
	.2byte	0x23
	.2byte	.L_l151_e-.L_l151
.L_l151:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l151_e:
.L_D151_e:
.L_D152:
	.4byte	.L_D152_e-.L_D152
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D153
	.2byte	0x38
	.string	"_nmalloc"
	.2byte	0x142
	.4byte	.L_T269
	.2byte	0x72
	.4byte	.L_T275
	.2byte	0x23
	.2byte	.L_l152_e-.L_l152
.L_l152:
	.byte	0x4
	.4byte	0x78
	.byte	0x7
.L_l152_e:
.L_D152_e:
.L_D153:
	.4byte	0x4
.L_D150:
	.4byte	.L_D150_e-.L_D150
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D154
	.2byte	0x38
	.string	"_unused"
	.2byte	0x142
	.4byte	.L_T254
	.2byte	0x72
	.4byte	.L_T269
	.2byte	0x23
	.2byte	.L_l150_e-.L_l150
.L_l150:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l150_e:
.L_D150_e:
.L_D154:
	.4byte	0x4
.L_D140:
	.4byte	.L_D140_e-.L_D140
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D155
	.2byte	0x38
	.string	"_new"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x72
	.4byte	.L_T254
	.2byte	0x23
	.2byte	.L_l140_e-.L_l140
.L_l140:
	.byte	0x4
	.4byte	0x58
	.byte	0x7
.L_l140_e:
.L_D140_e:
.L_D155:
	.4byte	.L_D155_e-.L_D155
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D156
	.2byte	0x38
	.string	"_atexit"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x83
	.2byte	.L_t155_e-.L_t155
.L_t155:
	.byte	0x1
	.4byte	.L_T197
.L_t155_e:
	.2byte	0x23
	.2byte	.L_l155_e-.L_l155
.L_l155:
	.byte	0x4
	.4byte	0x148
	.byte	0x7
.L_l155_e:
.L_D155_e:
.L_D156:
	.4byte	.L_D156_e-.L_D156
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D157
	.2byte	0x38
	.string	"_atexit0"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x72
	.4byte	.L_T197
	.2byte	0x23
	.2byte	.L_l156_e-.L_l156
.L_l156:
	.byte	0x4
	.4byte	0x14c
	.byte	0x7
.L_l156_e:
.L_D156_e:
.L_D157:
	.4byte	.L_D157_e-.L_D157
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D158
	.2byte	0x38
	.string	"_sig_func"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x83
	.2byte	.L_t157_e-.L_t157
.L_t157:
	.byte	0x1
	.byte	0x1
	.4byte	.L_T61
.L_t157_e:
	.2byte	0x23
	.2byte	.L_l157_e-.L_l157
.L_l157:
	.byte	0x4
	.4byte	0x1d4
	.byte	0x7
.L_l157_e:
.L_D157_e:
.L_D158:
	.4byte	.L_D158_e-.L_D158
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D159
	.2byte	0x38
	.string	"__sglue"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x72
	.4byte	.L_T170
	.2byte	0x23
	.2byte	.L_l158_e-.L_l158
.L_l158:
	.byte	0x4
	.4byte	0x1d8
	.byte	0x7
.L_l158_e:
.L_D158_e:
.L_D159:
	.4byte	.L_D159_e-.L_D159
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D160
	.2byte	0x38
	.string	"__sf"
	.2byte	0x142
	.4byte	.L_T238
	.2byte	0x72
	.4byte	.L_T298
	.2byte	0x23
	.2byte	.L_l159_e-.L_l159
.L_l159:
	.byte	0x4
	.4byte	0x1e4
	.byte	0x7
.L_l159_e:
.L_D159_e:
.L_D160:
	.4byte	0x4
.L_D122:
	.4byte	.L_D122_e-.L_D122
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D161
	.set	.L_T250,.L_D122
	.2byte	0xa3
	.2byte	.L_s122_e-.L_s122
.L_s122:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x18
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s122_e:
.L_D122_e:
.L_D161:
	.4byte	.L_D161_e-.L_D161
	.2byte	0x15
	.2byte	0x12
	.4byte	.L_D162
	.set	.L_T251,.L_D161
.L_D161_e:
.L_D163:
	.4byte	.L_D163_e-.L_D163
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D164
	.2byte	0x83
	.2byte	.L_t163_e-.L_t163
.L_t163:
	.byte	0x1
	.4byte	.L_T238
.L_t163_e:
.L_D163_e:
.L_D164:
	.4byte	0x4
.L_D162:
	.4byte	.L_D162_e-.L_D162
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D165
	.set	.L_T183,.L_D162
	.2byte	0x38
	.string	"_Bigint"
	.2byte	0xb6
	.4byte	0x18
.L_D162_e:
.L_D166:
	.4byte	.L_D166_e-.L_D166
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D167
	.2byte	0x38
	.string	"_next"
	.2byte	0x142
	.4byte	.L_T183
	.2byte	0x83
	.2byte	.L_t166_e-.L_t166
.L_t166:
	.byte	0x1
	.4byte	.L_T183
.L_t166_e:
	.2byte	0x23
	.2byte	.L_l166_e-.L_l166
.L_l166:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l166_e:
.L_D166_e:
.L_D167:
	.4byte	.L_D167_e-.L_D167
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D168
	.2byte	0x38
	.string	"_k"
	.2byte	0x142
	.4byte	.L_T183
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l167_e-.L_l167
.L_l167:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l167_e:
.L_D167_e:
.L_D168:
	.4byte	.L_D168_e-.L_D168
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D169
	.2byte	0x38
	.string	"_maxwds"
	.2byte	0x142
	.4byte	.L_T183
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l168_e-.L_l168
.L_l168:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l168_e:
.L_D168_e:
.L_D169:
	.4byte	.L_D169_e-.L_D169
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D170
	.2byte	0x38
	.string	"_sign"
	.2byte	0x142
	.4byte	.L_T183
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l169_e-.L_l169
.L_l169:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l169_e:
.L_D169_e:
.L_D170:
	.4byte	.L_D170_e-.L_D170
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D171
	.2byte	0x38
	.string	"_wds"
	.2byte	0x142
	.4byte	.L_T183
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l170_e-.L_l170
.L_l170:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l170_e:
.L_D170_e:
.L_D171:
	.4byte	.L_D171_e-.L_D171
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D172
	.2byte	0x38
	.string	"_x"
	.2byte	0x142
	.4byte	.L_T183
	.2byte	0x72
	.4byte	.L_T187
	.2byte	0x23
	.2byte	.L_l171_e-.L_l171
.L_l171:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l171_e:
.L_D171_e:
.L_D172:
	.4byte	0x4
.L_D165:
	.4byte	.L_D165_e-.L_D165
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D173
	.set	.L_T259,.L_D165
	.2byte	0xa3
	.2byte	.L_s165_e-.L_s165
.L_s165:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x19
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s165_e:
.L_D165_e:
.L_D173:
	.4byte	.L_D173_e-.L_D173
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D174
	.set	.L_T273,.L_D173
	.2byte	0xa3
	.2byte	.L_s173_e-.L_s173
.L_s173:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1d
	.byte	0x8
	.2byte	0x63
	.2byte	.L_t173_e-.L_t173
.L_t173:
	.byte	0x1
	.2byte	0x3
.L_t173_e:
.L_s173_e:
.L_D173_e:
.L_D174:
	.4byte	.L_D174_e-.L_D174
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D175
	.set	.L_T275,.L_D174
	.2byte	0xa3
	.2byte	.L_s174_e-.L_s174
.L_s174:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1d
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s174_e:
.L_D174_e:
.L_D175:
	.4byte	.L_D175_e-.L_D175
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D176
	.set	.L_T197,.L_D175
	.2byte	0x38
	.string	"_atexit"
	.2byte	0xb6
	.4byte	0x88
.L_D175_e:
.L_D177:
	.4byte	.L_D177_e-.L_D177
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D178
	.2byte	0x38
	.string	"_next"
	.2byte	0x142
	.4byte	.L_T197
	.2byte	0x83
	.2byte	.L_t177_e-.L_t177
.L_t177:
	.byte	0x1
	.4byte	.L_T197
.L_t177_e:
	.2byte	0x23
	.2byte	.L_l177_e-.L_l177
.L_l177:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l177_e:
.L_D177_e:
.L_D178:
	.4byte	.L_D178_e-.L_D178
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D179
	.2byte	0x38
	.string	"_ind"
	.2byte	0x142
	.4byte	.L_T197
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l178_e-.L_l178
.L_l178:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l178_e:
.L_D178_e:
.L_D179:
	.4byte	.L_D179_e-.L_D179
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D180
	.2byte	0x38
	.string	"_fns"
	.2byte	0x142
	.4byte	.L_T197
	.2byte	0x72
	.4byte	.L_T203
	.2byte	0x23
	.2byte	.L_l179_e-.L_l179
.L_l179:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l179_e:
.L_D179_e:
.L_D180:
	.4byte	0x4
.L_D176:
	.4byte	.L_D176_e-.L_D176
	.2byte	0x15
	.2byte	0x12
	.4byte	.L_D181
	.set	.L_T61,.L_D176
.L_D176_e:
.L_D182:
	.4byte	.L_D182_e-.L_D182
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D183
	.2byte	0x55
	.2byte	0x7
.L_D182_e:
.L_D183:
	.4byte	0x4
.L_D181:
	.4byte	.L_D181_e-.L_D181
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D184
	.set	.L_T298,.L_D181
	.2byte	0xa3
	.2byte	.L_s181_e-.L_s181
.L_s181:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x2
	.byte	0x8
	.2byte	0x72
	.4byte	.L_T172
.L_s181_e:
.L_D181_e:
.L_D184:
	.4byte	.L_D184_e-.L_D184
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D185
	.set	.L_T187,.L_D184
	.2byte	0xa3
	.2byte	.L_s184_e-.L_s184
.L_s184:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s184_e:
.L_D184_e:
.L_D185:
	.4byte	.L_D185_e-.L_D185
	.2byte	0x15
	.2byte	0x12
	.4byte	.L_D186
	.set	.L_T60,.L_D185
.L_D185_e:
.L_D187:
	.4byte	0x4
.L_D186:
	.4byte	.L_D186_e-.L_D186
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D188
	.set	.L_T203,.L_D186
	.2byte	0xa3
	.2byte	.L_s186_e-.L_s186
.L_s186:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1f
	.byte	0x8
	.2byte	0x83
	.2byte	.L_t186_e-.L_t186
.L_t186:
	.byte	0x1
	.4byte	.L_T60
.L_t186_e:
.L_s186_e:
.L_D186_e:
	.previous

	.section	.debug
	.previous

	.section	.debug
	.previous

	.section	.debug
	.previous

	.section	.debug
	.previous

	.section	.debug
.L_D188:
	.4byte	.L_D188_e-.L_D188
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D189
	.2byte	0x38
	.string	"_fpos_t"
	.2byte	0x55
	.2byte	0x7
.L_D188_e:
	.previous

	.section	.debug
	.previous

	.section	.debug
.L_D189:
	.4byte	.L_D189_e-.L_D189
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D190
	.2byte	0x38
	.string	"fpos_t"
	.2byte	0x55
	.2byte	0x7
.L_D189_e:
	.previous

	.section	.debug
.L_D190:
	.4byte	.L_D190_e-.L_D190
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D191
	.2byte	0x38
	.string	"FILE"
	.2byte	0x72
	.4byte	.L_T172
.L_D190_e:
	.previous

	.section	.debug
	.previous

	.section	.debug
.L_D191:
	.4byte	.L_D191_e-.L_D191
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D192
	.2byte	0x38
	.string	"PRINTDEVICEFUNC"
	.2byte	0x72
	.4byte	.L_T407
.L_D191_e:
	.previous

	.section	.debug
.L_D192:
	.4byte	.L_D192_e-.L_D192
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D193
	.set	.L_T409,.L_D192
	.2byte	0x38
	.string	"PRINTCHANNELINFO"
	.2byte	0xb6
	.4byte	0xc
.L_D192_e:
.L_D194:
	.4byte	.L_D194_e-.L_D194
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D195
	.2byte	0x38
	.string	"name"
	.2byte	0x142
	.4byte	.L_T409
	.2byte	0x63
	.2byte	.L_t194_e-.L_t194
.L_t194:
	.byte	0x1
	.byte	0x3
	.2byte	0x1
.L_t194_e:
	.2byte	0x23
	.2byte	.L_l194_e-.L_l194
.L_l194:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l194_e:
.L_D194_e:
.L_D195:
	.4byte	.L_D195_e-.L_D195
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D196
	.2byte	0x38
	.string	"enabled"
	.2byte	0x142
	.4byte	.L_T409
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l195_e-.L_l195
.L_l195:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l195_e:
.L_D195_e:
.L_D196:
	.4byte	.L_D196_e-.L_D196
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D197
	.2byte	0x38
	.string	"used"
	.2byte	0x142
	.4byte	.L_T409
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l196_e-.L_l196
.L_l196:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l196_e:
.L_D196_e:
.L_D197:
	.4byte	0x4
	.previous

	.section	.debug
	.previous

	.section	.debug
.L_D193:
	.4byte	.L_D193_e-.L_D193
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D198
	.set	.L_T432,.L_D193
	.2byte	0xa3
	.2byte	.L_s193_e-.L_s193
.L_s193:
	.byte	0x1
	.2byte	0x7
	.4byte	0x0
	.2byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s193_e:
.L_D193_e:
.L_P1:
.L_D198:
	.4byte	.L_D198_e-.L_D198
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D199
	.2byte	0x38
	.string	"lbl_80414A2C"
	.2byte	0x83
	.2byte	.L_t198_e-.L_t198
.L_t198:
	.byte	0x3
	.4byte	.L_T432
.L_t198_e:
.L_D198_e:
	.previous

	.section	.debug
.L_D199:
	.4byte	.L_D199_e-.L_D199
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D200
	.set	.L_T434,.L_D199
	.2byte	0xa3
	.2byte	.L_s199_e-.L_s199
.L_s199:
	.byte	0x1
	.2byte	0x7
	.4byte	0x0
	.2byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s199_e:
.L_D199_e:
.L_P2:
.L_D200:
	.4byte	.L_D200_e-.L_D200
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D201
	.2byte	0x38
	.string	"lbl_80414A3C"
	.2byte	0x83
	.2byte	.L_t200_e-.L_t200
.L_t200:
	.byte	0x3
	.4byte	.L_T434
.L_t200_e:
.L_D200_e:
	.previous

	.section	.debug
.L_D201:
	.4byte	.L_D201_e-.L_D201
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D202
	.set	.L_T436,.L_D201
	.2byte	0xa3
	.2byte	.L_s201_e-.L_s201
.L_s201:
	.byte	0x1
	.2byte	0x7
	.4byte	0x0
	.2byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s201_e:
.L_D201_e:
.L_P3:
.L_D202:
	.4byte	.L_D202_e-.L_D202
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D203
	.2byte	0x38
	.string	"lbl_80414A4C"
	.2byte	0x83
	.2byte	.L_t202_e-.L_t202
.L_t202:
	.byte	0x3
	.4byte	.L_T436
.L_t202_e:
.L_D202_e:
	.previous

	.section	.debug
.L_D203:
	.4byte	.L_D203_e-.L_D203
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D204
	.set	.L_T438,.L_D203
	.2byte	0xa3
	.2byte	.L_s203_e-.L_s203
.L_s203:
	.byte	0x1
	.2byte	0x7
	.4byte	0x0
	.2byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s203_e:
.L_D203_e:
.L_P4:
.L_D204:
	.4byte	.L_D204_e-.L_D204
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D205
	.2byte	0x38
	.string	"lbl_80414A5C"
	.2byte	0x83
	.2byte	.L_t204_e-.L_t204
.L_t204:
	.byte	0x3
	.4byte	.L_T438
.L_t204_e:
.L_D204_e:
	.previous

	.section	.debug
.L_D205:
	.4byte	.L_D205_e-.L_D205
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D206
	.set	.L_T440,.L_D205
	.2byte	0xa3
	.2byte	.L_s205_e-.L_s205
.L_s205:
	.byte	0x1
	.2byte	0x7
	.4byte	0x0
	.2byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s205_e:
.L_D205_e:
.L_P5:
.L_D206:
	.4byte	.L_D206_e-.L_D206
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D207
	.2byte	0x38
	.string	"lbl_80414A6C"
	.2byte	0x83
	.2byte	.L_t206_e-.L_t206
.L_t206:
	.byte	0x3
	.4byte	.L_T440
.L_t206_e:
.L_D206_e:
	.previous

	.section	.debug
.L_D207:
	.4byte	.L_D207_e-.L_D207
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D208
	.set	.L_T442,.L_D207
	.2byte	0xa3
	.2byte	.L_s207_e-.L_s207
.L_s207:
	.byte	0x1
	.2byte	0x7
	.4byte	0x0
	.2byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s207_e:
.L_D207_e:
.L_P6:
.L_D208:
	.4byte	.L_D208_e-.L_D208
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D209
	.2byte	0x38
	.string	"lbl_80414A7C"
	.2byte	0x83
	.2byte	.L_t208_e-.L_t208
.L_t208:
	.byte	0x3
	.4byte	.L_T442
.L_t208_e:
.L_D208_e:
	.previous

	.section	.debug
.L_D209:
	.4byte	.L_D209_e-.L_D209
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D210
	.set	.L_T444,.L_D209
	.2byte	0xa3
	.2byte	.L_s209_e-.L_s209
.L_s209:
	.byte	0x1
	.2byte	0x7
	.4byte	0x0
	.2byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s209_e:
.L_D209_e:
.L_P7:
.L_D210:
	.4byte	.L_D210_e-.L_D210
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D211
	.2byte	0x38
	.string	"lbl_80414A8C"
	.2byte	0x83
	.2byte	.L_t210_e-.L_t210
.L_t210:
	.byte	0x3
	.4byte	.L_T444
.L_t210_e:
.L_D210_e:
	.previous

	.section	.debug
.L_D211:
	.4byte	.L_D211_e-.L_D211
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D212
	.set	.L_T446,.L_D211
	.2byte	0xa3
	.2byte	.L_s211_e-.L_s211
.L_s211:
	.byte	0x1
	.2byte	0x7
	.4byte	0x0
	.2byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s211_e:
.L_D211_e:
.L_P8:
.L_D212:
	.4byte	.L_D212_e-.L_D212
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D213
	.2byte	0x38
	.string	"lbl_80414A9C"
	.2byte	0x83
	.2byte	.L_t212_e-.L_t212
.L_t212:
	.byte	0x3
	.4byte	.L_T446
.L_t212_e:
.L_D212_e:
	.previous

	.section	.debug
.L_D213:
	.4byte	.L_D213_e-.L_D213
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D214
	.set	.L_T448,.L_D213
	.2byte	0xa3
	.2byte	.L_s213_e-.L_s213
.L_s213:
	.byte	0x1
	.2byte	0x7
	.4byte	0x0
	.2byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s213_e:
.L_D213_e:
.L_P9:
.L_D214:
	.4byte	.L_D214_e-.L_D214
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D215
	.2byte	0x38
	.string	"lbl_80414AAC"
	.2byte	0x83
	.2byte	.L_t214_e-.L_t214
.L_t214:
	.byte	0x3
	.4byte	.L_T448
.L_t214_e:
.L_D214_e:
	.previous

	.section	.debug
.L_D215:
	.4byte	.L_D215_e-.L_D215
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D216
	.set	.L_T450,.L_D215
	.2byte	0xa3
	.2byte	.L_s215_e-.L_s215
.L_s215:
	.byte	0x1
	.2byte	0x7
	.4byte	0x0
	.2byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s215_e:
.L_D215_e:
.L_P10:
.L_D216:
	.4byte	.L_D216_e-.L_D216
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D217
	.2byte	0x38
	.string	"lbl_80414ABC"
	.2byte	0x83
	.2byte	.L_t216_e-.L_t216
.L_t216:
	.byte	0x3
	.4byte	.L_T450
.L_t216_e:
.L_D216_e:
	.previous

	.section	.debug
.L_D217:
	.4byte	.L_D217_e-.L_D217
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D218
	.set	.L_T452,.L_D217
	.2byte	0xa3
	.2byte	.L_s217_e-.L_s217
.L_s217:
	.byte	0x1
	.2byte	0x7
	.4byte	0x0
	.2byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s217_e:
.L_D217_e:
.L_P11:
.L_D218:
	.4byte	.L_D218_e-.L_D218
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D219
	.2byte	0x38
	.string	"lbl_80414ACC"
	.2byte	0x83
	.2byte	.L_t218_e-.L_t218
.L_t218:
	.byte	0x3
	.4byte	.L_T452
.L_t218_e:
.L_D218_e:
	.previous

	.section	.debug
.L_D219:
	.4byte	.L_D219_e-.L_D219
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D220
	.set	.L_T454,.L_D219
	.2byte	0xa3
	.2byte	.L_s219_e-.L_s219
.L_s219:
	.byte	0x1
	.2byte	0x7
	.4byte	0x0
	.2byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s219_e:
.L_D219_e:
.L_P12:
.L_D220:
	.4byte	.L_D220_e-.L_D220
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D221
	.2byte	0x38
	.string	"lbl_80414ADC"
	.2byte	0x83
	.2byte	.L_t220_e-.L_t220
.L_t220:
	.byte	0x3
	.4byte	.L_T454
.L_t220_e:
.L_D220_e:
	.previous

	.section	.debug
.L_D221:
	.4byte	.L_D221_e-.L_D221
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D222
	.set	.L_T456,.L_D221
	.2byte	0xa3
	.2byte	.L_s221_e-.L_s221
.L_s221:
	.byte	0x1
	.2byte	0x7
	.4byte	0x0
	.2byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s221_e:
.L_D221_e:
.L_P13:
.L_D222:
	.4byte	.L_D222_e-.L_D222
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D223
	.2byte	0x38
	.string	"lbl_80414AEC"
	.2byte	0x83
	.2byte	.L_t222_e-.L_t222
.L_t222:
	.byte	0x3
	.4byte	.L_T456
.L_t222_e:
.L_D222_e:
	.previous

	.section	.debug
.L_D223:
	.4byte	.L_D223_e-.L_D223
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D224
	.set	.L_T458,.L_D223
	.2byte	0xa3
	.2byte	.L_s223_e-.L_s223
.L_s223:
	.byte	0x1
	.2byte	0x7
	.4byte	0x0
	.2byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s223_e:
.L_D223_e:
.L_P14:
.L_D224:
	.4byte	.L_D224_e-.L_D224
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D225
	.2byte	0x38
	.string	"lbl_80414B00"
	.2byte	0x83
	.2byte	.L_t224_e-.L_t224
.L_t224:
	.byte	0x3
	.4byte	.L_T458
.L_t224_e:
.L_D224_e:
	.previous

	.section	.debug
.L_D225:
	.4byte	.L_D225_e-.L_D225
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D226
	.set	.L_T460,.L_D225
	.2byte	0xa3
	.2byte	.L_s225_e-.L_s225
.L_s225:
	.byte	0x1
	.2byte	0x7
	.4byte	0x0
	.2byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s225_e:
.L_D225_e:
.L_P15:
.L_D226:
	.4byte	.L_D226_e-.L_D226
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D227
	.2byte	0x38
	.string	"lbl_80414B14"
	.2byte	0x83
	.2byte	.L_t226_e-.L_t226
.L_t226:
	.byte	0x3
	.4byte	.L_T460
.L_t226_e:
.L_D226_e:
	.previous

	.section	.debug
.L_D227:
	.4byte	.L_D227_e-.L_D227
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D228
	.set	.L_T463,.L_D227
	.2byte	0xa3
	.2byte	.L_s227_e-.L_s227
.L_s227:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x3f
	.byte	0x8
	.2byte	0x72
	.4byte	.L_T409
.L_s227_e:
.L_D227_e:
.L_D228:
	.4byte	.L_D228_e-.L_D228
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D229
	.2byte	0x38
	.string	"PRINTchannellist"
	.2byte	0x72
	.4byte	.L_T463
	.2byte	0x23
	.2byte	.L_l228_e-.L_l228
.L_l228:
	.byte	0x3
	.4byte	PRINTchannellist
.L_l228_e:
.L_D228_e:
	.previous

	.section	.debug_pubnames
	.4byte	.L_P16
	.string	"PRINTdevicelist"
	.previous

	.section	.debug_aranges
	.4byte	PRINTdevicelist
	.4byte	0x60
	.previous

	.section	.debug
.L_D229:
	.4byte	.L_D229_e-.L_D229
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D230
	.set	.L_T465,.L_D229
	.2byte	0xa3
	.2byte	.L_s229_e-.L_s229
.L_s229:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x7
	.byte	0x8
	.2byte	0x72
	.4byte	.L_T420
.L_s229_e:
.L_D229_e:
.L_P16:
.L_D230:
	.4byte	.L_D230_e-.L_D230
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D231
	.2byte	0x38
	.string	"PRINTdevicelist"
	.2byte	0x72
	.4byte	.L_T465
	.2byte	0x23
	.2byte	.L_l230_e-.L_l230
.L_l230:
	.byte	0x3
	.4byte	PRINTdevicelist
.L_l230_e:
.L_D230_e:
	.previous

	.section	.debug
.L_D231:
	.4byte	.L_D231_e-.L_D231
	.align 2
.L_D231_e:
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
