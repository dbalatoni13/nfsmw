	.file	"timer.cpp"
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
	.string	"src/Speed/Indep/Libs/realcore/6.24.00/source/system/cmn/timer.cpp"
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

	.section	".text"
	.align 2
	.globl TIMER_gettick__Fv
	.type	 TIMER_gettick__Fv,@function
TIMER_gettick__Fv:
.L_fTIMER_gettick__Fv_s:
.L_LC1:


	.section	.debug_sfnames
.L_F0:
	.string	"src/Speed/Indep/Libs/realcore/6.24.00/source/system/cmn/timer.cpp"
	.previous
	.section	.line
.L_LE1:
	.4byte	3	 # /timer.cpp:3
	.2byte	0xffff
	.4byte	.L_LC1-.L_text_b
	.previous

	.section	.debug_srcinfo
	.4byte	.L_LE1-.L_line_b
	.4byte	.L_F0-.L_sfnames_b
	.previous
.L_b1:
.L_B2:
.L_LC2:

	.section	.line
	.4byte	4	 # /timer.cpp:4
	.2byte	0xffff
	.4byte	.L_LC2-.L_text_b
	.previous
	lwz 3,ticks@sda21(0)
.L_B2_e:
	blr
.L_b1_e:
.L_f1_e:
.L_fTIMER_gettick__Fv_e:
.Lfe1:
	.size	 TIMER_gettick__Fv,.Lfe1-TIMER_gettick__Fv

	.section	.debug_pubnames
	.4byte	.L_P0
	.string	"TIMER_gettick"
	.previous

	.section	.debug
.L_P0:
.L_D3:
	.4byte	.L_D3_e-.L_D3
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D4
	.2byte	0x38
	.string	"TIMER_gettick"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x111
	.4byte	.L_fTIMER_gettick__Fv_s
	.2byte	0x121
	.4byte	.L_fTIMER_gettick__Fv_e
	.2byte	0x8041
	.4byte	.L_b1
	.2byte	0x8051
	.4byte	.L_b1_e
.L_D3_e:
.L_D5:
	.4byte	0x4
	.previous

	.section	.debug
.L_D4:
	.4byte	.L_D4_e-.L_D4
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D6
	.set	.L_T86,.L_D4
	.2byte	0x38
	.string	"__vtbl_ptr_type"
	.2byte	0xb6
	.4byte	0x8
.L_D4_e:
.L_D7:
	.4byte	.L_D7_e-.L_D7
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D8
	.2byte	0x38
	.string	"__delta"
	.2byte	0x142
	.4byte	.L_T86
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l7_e-.L_l7
.L_l7:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l7_e:
.L_D7_e:
.L_D8:
	.4byte	.L_D8_e-.L_D8
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D9
	.2byte	0x38
	.string	"__index"
	.2byte	0x142
	.4byte	.L_T86
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l8_e-.L_l8
.L_l8:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l8_e:
.L_D8_e:
.L_D9:
	.4byte	.L_D9_e-.L_D9
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D10
	.2byte	0x38
	.string	"__pfn"
	.2byte	0x142
	.4byte	.L_T86
	.2byte	0x63
	.2byte	.L_t9_e-.L_t9
.L_t9:
	.byte	0x1
	.2byte	0x14
.L_t9_e:
	.2byte	0x23
	.2byte	.L_l9_e-.L_l9
.L_l9:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l9_e:
.L_D9_e:
.L_D10:
	.4byte	.L_D10_e-.L_D10
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D11
	.2byte	0x38
	.string	"__delta2"
	.2byte	0x142
	.4byte	.L_T86
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l10_e-.L_l10
.L_l10:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l10_e:
.L_D10_e:
.L_D11:
	.4byte	0x4
.L_D6:
	.4byte	.L_D6_e-.L_D6
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D12
	.2byte	0x38
	.string	"__vtbl_ptr_type"
	.2byte	0x83
	.2byte	.L_t6_e-.L_t6
.L_t6:
	.byte	0x3
	.4byte	.L_T86
.L_t6_e:
.L_D6_e:
	.previous

	.section	.debug
.L_D12:
	.4byte	.L_D12_e-.L_D12
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D13
	.set	.L_T94,.L_D12
	.2byte	0x38
	.string	"bad_alloc"
.L_D12_e:
	.previous

	.section	.debug
.L_P1:
.L_D13:
	.4byte	.L_D13_e-.L_D13
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D14
	.2byte	0x38
	.string	"ticks"
	.2byte	0x55
	.2byte	0x9
.L_D13_e:
	.previous

	.section	.debug
.L_D14:
	.4byte	.L_D14_e-.L_D14
	.align 2
.L_D14_e:
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
