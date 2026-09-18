	.file	"sf_sin.c"
gcc2_compiled.:
	.section	".data"
	.align 2
	.type	 zero,@object
	.size	 zero,4
zero:
	.long 0x0
	.section	".rodata"
	.align 2
	.type	$LC0,@object
$LC0:
	.long 0x0
	.size	$LC0,4
	.section	".text"
	.align 2
	.globl sinf__Ff
	.type	 sinf__Ff,@function
sinf__Ff:
.L_fsinf__Ff_s:
	stwu 1,-32(1)
	mflr 0
	stmw 30,24(1)
	stw 0,36(1)
	lis 11,zero@ha
	lfs 2,zero@l(11)
	fmr 0,1
	stfs 0,16(1)
	lwz 9,16(1)
	rlwinm 9,9,0,1,31
	lis 0,0x3f49
	ori 0,0,4056
	cmpw 0,9,0
	bc 12,1,.L7        # bgt  cr0
	li 3,0
	bl __kernel_sinf__Fffi
	b .L19
.L7:
	lis 0,0x7f7f
	ori 0,0,65535
	cmpw 0,9,0
	bc 4,1,.L9        # ble  cr0
	lis 9,$LC0@ha
	lfs 1,$LC0@l(9)
	b .L19
.L9:
	addi 30,1,8
	mr 3,30
	bl __ieee754_rem_pio2f__FfPf
	rlwinm 3,3,0,30,31
	cmpwi 0,3,1
	bc 12,2,.L13        # beq  cr0
	bc 12,1,.L17        # bgt  cr0
	cmpwi 0,3,0
	bc 12,2,.L12        # beq  cr0
	b .L15
.L17:
	cmpwi 0,3,2
	bc 12,2,.L14        # beq  cr0
	b .L15
.L12:
	lfs 2,4(30)
	li 3,1
	lfs 1,8(1)
	bl __kernel_sinf__Fffi
	b .L19
.L13:
	lfs 2,4(30)
	lfs 1,8(1)
	bl __kernel_cosf__Fff
	b .L19
.L14:
	lfs 2,4(30)
	li 3,1
	lfs 1,8(1)
	bl __kernel_sinf__Fffi
	b .L20
.L15:
	lfs 2,4(30)
	lfs 1,8(1)
	bl __kernel_cosf__Fff
.L20:
	fneg 1,1
.L19:
	lwz 0,36(1)
	mtlr 0
	lmw 30,24(1)
	la 1,32(1)
	blr
.Lfe1:
	.size	 sinf__Ff,.Lfe1-sinf__Ff
	.ident	"GCC: (GNU) 2.95.3 SN BUILD v1.76 for Nintendo Gamecube"
