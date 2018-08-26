
	.align
	.global transfer8x8_copy_arm
    .func transfer8x8_copy_arm
transfer8x8_copy_arm:
@ r0 - dst	(u8)
@ r1 - src	(u8)
@ r2 - stride
	
	stmfd	sp!, {r4-r5}
	
	tst		r0, #0x03
	tsteq	r1, #0x03
	bne		_transfer8x8_copy_arm_unaligned
	
	mov		r3, #8
_transfer8x8_copy_arm_loop:
	ldmia	r1, {r4-r5}
	subs	r3, #1
	stmia	r0, {r4-r5}
	add		r1, r2
	add		r0, r2
	bne		_transfer8x8_copy_arm_loop
	
	ldmfd	sp!, {r4-r5}
	bx		lr


_transfer8x8_copy_arm_unaligned:
@ resort to byte-wise transfers
	sub		r2, r2, #8
	mov		r3, #8
_transfer8x8_copy_arm_unaligned_outer_loop:

	mov		r4, #8
_transfer8x8_copy_arm_unaligned_inner_loop:
	ldrb	r5, [r1], #1
	subs	r4, #1
	strb	r5, [r0], #1
	bne		_transfer8x8_copy_arm_unaligned_inner_loop
	
	add		r0, r2
	subs	r3, #1
	add		r1, r2
	bne		_transfer8x8_copy_arm_unaligned_outer_loop
	
	ldmfd	sp!, {r4-r5}
	bx		lr

	.endfunc


	.align
	.global transfer_16to8copy_arm
    .func transfer_16to8copy_arm
transfer_16to8copy_arm:
@ r0 - dst (uint8_t)
@ r1 - src (int16_t)
@ r2 - stride
	
	stmfd	sp!, {r4-r5}
	sub		r2, r2, #8
	mov		r3, #8
_transfer_16to8copy_arm_outer_loop:

	mov		r4, #8
_transfer_16to8copy_arm_inner_loop:
	ldrsh	r5, [r1], #2
	cmp		r5, #0
	movlt	r5, #0
	cmp		r5, #255
	movgt	r5, #255
	strb	r5, [r0], #1
	subs	r4, #1
	bne		_transfer_16to8copy_arm_inner_loop
	
	add		r0, r2
	subs	r3, #1
	bne		_transfer_16to8copy_arm_outer_loop
	
	ldmfd	sp!, {r4-r5}
	bx		lr

	.endfunc

	.align
	.global mem_clear_arm
    .func mem_clear_arm
mem_clear_arm:
@ r0 - dest
@ r1 - size
	stmfd	sp!, {r4-r5}
	add		r1, r0
	mov		r2, #0
	mov		r3, #0
	mov		r4, #0
	mov		r5, #0
mem_clear_loop:	
	stmia	r0!, {r2-r5}
	cmp		r0, r1
	blt		mem_clear_loop
	ldmfd	sp!, {r4-r5}
	bx		lr
	
	.endfunc
