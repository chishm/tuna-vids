
	.align
	.global transfer_16to8add_arm
    .func transfer_16to8add_arm
transfer_16to8add_arm:
@ r0 - uint_8 * const dst
@ r1 - int16_t * const src
@ r2 - uint32_t stride

@ Used vars:
@ r3 - current row
@ r4-r7 - input data
@ r8-r9 - output data
@ r10-r11 - temporary data

	stmfd	sp!,	{r4-r11}
	
	mov		r3, #8

_transfer_16to8add_arm_row_loop:

	ldmia	r0, {r8-r9}		@ read row dst
	ldmia	r1!, {r5-r8}	@ read row src
	
	@ pixel 1
	and		r10, r8, #0xFF
	mov		r11, r4, lsl #16
	add		r10, r10, r11, asr #16
	cmp		r10, #0
	movlt	r10, #0
	cmp		r10, #255
	movgt	r10, #255
	bic		r8, #0xFF
	orr		r8, r10
	
	@ pixel 2
	and		r10, r8, #0xFF00
	mov		r11, r4, asr #16
	add		r10, r11, r10, lsr #8
	cmp		r10, #0
	movlt	r10, #0
	cmp		r10, #255
	movgt	r10, #255
	bic		r8, #0xFF00
	orr		r8, r10, lsl #8

	@ pixel 3
	and		r10, r8, #0xFF0000
	add		r10, r10, r5, lsl #16
	cmp		r10, #0
	movlt	r10, #0
	cmp		r10, #0xFF0000
	movgt	r10, #0xFF0000
	bic		r8, #0xFF0000
	orr		r8, r10
	
	@ pixel 4
	and		r10, r8, #0xFF000000
	mov		r11, r5, asr #16
	add		r10, r11, r10, lsr #24
	cmp		r10, #0
	movlt	r10, #0
	cmp		r10, #255
	movgt	r10, #255
	bic		r8, #0xFF000000
	orr		r8, r10, lsl #24

	@ pixel 5
	and		r10, r9, #0xFF
	mov		r11, r6, lsl #16
	add		r10, r10, r11, asr #16
	cmp		r10, #0
	movlt	r10, #0
	cmp		r10, #255
	movgt	r10, #255
	bic		r9, #0xFF
	orr		r9, r10

	@ pixel 6
	and		r10, r9, #0xFF00
	mov		r11, r6, asr #16
	add		r10, r11, r10, lsr #8
	cmp		r10, #0
	movlt	r10, #0
	cmp		r10, #255
	movgt	r10, #255
	bic		r9, #0xFF00
	orr		r9, r10, lsl #8

	@ pixel 7
	and		r10, r9, #0xFF0000
	add		r10, r10, r7, lsl #16
	cmp		r10, #0
	movlt	r10, #0
	cmp		r10, #0xFF0000
	movgt	r10, #0xFF0000
	bic		r9, #0xFF0000
	orr		r9, r10
	
	@ pixel 8
	and		r10, r9, #0xFF000000
	mov		r11, r7, asr #16
	add		r10, r11, r10, lsr #24
	cmp		r10, #0
	movlt	r10, #0
	cmp		r10, #255
	movgt	r10, #255
	bic		r9, #0xFF000000
	orr		r9, r10, lsl #24

	stmia	r0, {r8-r9}		@ write row dst
	add		r0, r0, r2
	
	subs	r3, r3, #1
	bne		_transfer_16to8add_arm_row_loop
	
	ldmfd	sp!,	{r4-r11}
	bx		lr
	
	.endfunc
