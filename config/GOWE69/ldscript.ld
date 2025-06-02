ENTRY(__start) 
PROVIDE (__stack = 0);
/* For some reason these two aren't emitted automatically */
PROVIDE (_ctors = 0x804528D0);
PROVIDE (_dtors = 0x804528D8);
SECTIONS
{
	.comment :{ *(.comment)}
	.init	0x80003100	 : { *(.init)		} =0

	. =  ALIGN(16);
	.text      :
	{
		*(.text)
		*(.gnu.warning)
		*(.gnu.linkonce.t*)
	} =0
    . =  ALIGN(8);
	.over      :
	{
		*(.over)
	} =0
	. =  ALIGN(16);
	.ctors	  : 
	{ 
		__CTOR_LIST__ = .;
		LONG(-1);
		*(.ctors)
		LONG(0); 
		/* PROVIDE (__CTOR_END__ = .); */
	}
	. =  ALIGN(1);		
	.dtors	  :
	{ 
		 __DTOR_LIST__ = .;
		 LONG(-1);
		*(.dtors)
		LONG(0);
		/* PROVIDE (__DTOR_END__ = .); */
	}


	. =  ALIGN(32);
	extabindex        :
	{
		*(extabindex)
	}

	. =  ALIGN(32);
	extab     :
	{
		*(extab)
	}
	 . =  ALIGN(8);
	.rodata	  : 
	{
		 *(.rodata) *(.gnu.linkonce.r*) *(.rodata.*)
	}
	. =  ALIGN(32);
	.data    :
	{
		*(.data)
		*(.data.*)
		*(.gnu.linkonce.d*)
	}
	. =  ALIGN(32);
	.bss       :
	{
		_f_bss = .;
		__bss_start = .;
		*(.dynbss)
		*(.bss)
		*(.bss.*)
		*(COMMON)
		__bss_end = .;
		_e_bss = .;
	}

	. =  ALIGN(32);
	.sdata	  :
	{ 

		 __SDATA_START__ = .;
		 _SDA_BASE_ = . + 0x8000; 
		 _SDA2_BASE_ = . + 0x18000;

		*(.sdata) 
		*(.sdata.*)
	}

	. =  ALIGN(8);
	.sbss      :
	{
		_f_sbss = .;
		__sbss_start = .;
		*(.sbss)
		*(.sbss.*)
		*(.scommon)
		*(.dynsbss)
		__sbss_end = .;
		_e_sbss = .;
		__SBSS_END__ = .;
	}
	. =  ALIGN(32);
	.sdata2   :
	{
		__SDATA2_START__ = .;
		 *(.sdata2)
		*(.sdata2.*)
	}
	.sbss2   :
	{
		*(.sbss2)
		*(.sbss2.*)
		 __SBSS2_END__ = .;
	}


  .got2		  :  { *(.got2) }
		PROVIDE (_FIXUP_START_ = .);
  .fixup	  : { *(.fixup) }
		PROVIDE (_FIXUP_END_ = .);
		PROVIDE (_GOT2_END_ = .);
		PROVIDE (_GOT_START_ = .);
  .got		  : { *(.got) }
  .got.plt	  : { *(.got.plt) }
		PROVIDE (_GOT_END_ = .);

 
  .plt   : { *(.plt) }

	. = . + 16;		
	. = ALIGN(32);		
	_stack_end = .;
	. = . + 0x10000;	


	_stack_addr = .;
	. = . + 16;

	. = ALIGN(32) ;
	__ArenaLo = .;

	 /* . = 0x80380000;  HW1_DRIP */
	 . = 0x81780000; /*   HW1 or HW2 (Others ?)  */
	__ArenaHi = .;
	

  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  .debug_info     0 : { *(.debug_info) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
}
