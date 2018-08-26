
.equ	SCALEBITS_OUT,	6
.equ	SCALEBITS_DISP, 9
.equ	MAX_RGB,		31
.equ	WIDTH,			256
.equ	X_STRIDE,		512
.equ	Y_STRIDE,		384
.equ	UV_STRIDE,		192

.equ	B_U_FACTOR,		129		@ = ((2.018) * (1<<SCALEBITS_OUT) + 0.5)
.equ	G_U_FACTOR,		25		@ = ((0.391) * (1<<SCALEBITS_OUT) + 0.5)
.equ	G_V_FACTOR,		52		@ = ((0.813) * (1<<SCALEBITS_OUT) + 0.5)
.equ	R_V_FACTOR,		102		@ = ((1.596) * (1<<SCALEBITS_OUT) + 0.5)
.equ	U_FACTORS,		(G_U_FACTOR << 16) | (B_U_FACTOR)
.equ	V_FACTORS,		(G_V_FACTOR << 16) | (R_V_FACTOR)
.equ	RGB_Y_FACTOR,	74		@ = ((1.164) * (1<<SCALEBITS_OUT) + 0.5)

.align
.global	yv12_to_rgb555_asm
.func	yv12_to_rgb555_asm

@ void yv12_to_rgb555_asm 
@  uint8_t * x_ptr
@  uint8_t * y_ptr
@  uint8_t * u_ptr
@  uint8_t * v_ptr
@  int height

yv12_to_rgb555_asm:

	stmfd	sp!, {r4-r11, lr}
	
@ cache colour factors into stack for quick access
	ldr		r7, =U_FACTORS
	ldr		r8, =V_FACTORS
	stmfd	sp!, {r7-r8}

@ combine Y factor and line count into one variable -- each needs only 16 bits
	mov		r10, #RGB_Y_FACTOR		@ low 16 bits are Y factor
	ldr		r12, [sp, #44]
	sub		r12, #1
	orr		r10, r10, r12, lsl #16		@ high 16 bits are line count

	
b_u0	.req r4
g_uv0	.req r5
r_v0	.req r6
rgb_y	.req r7

column_loop:
	
row_loop:
	
	ldmfd	sp, {r7-r8}		@ load table offsets
	ldrb	r11, [r2], #1		@ load U value
	ldrb	r12, [r3], #1		@ load V value
	@ Calculate colour differences
	sub		r11, #128
	sub		r12, #128
	smultb	g_uv0, r7, r11
	smulbb	r_v0, r7, r12
	smulbb	b_u0, r8, r11
	smlatb	g_uv0, r8, r12, g_uv0
	
	@ top row
	ldrh	r11, [r1], #2		@ load Y value for 2 pixels
	
	@ top left pixel luma
	and		r12, r11, #0xFF
	sub		r12, #16
	smulbb	rgb_y, r10, r12
	
	@ red	
	mov		r12, #(1<<15)
	add		r8, rgb_y, r_v0
	movs	r9, r8, asr #14
	orreq	r12, r8, lsr #9
	orrgt	r12, #MAX_RGB	

	@ green
	sub		r8, rgb_y, g_uv0
	movs	r9, r8, asr #14
	moveq	r8, r8, lsr #9
	orreq	r12, r8, lsl #5
	orrgt	r12, #(MAX_RGB<<5)	

	@ blue
	add		r8, rgb_y, b_u0
	movs	r9, r8, asr #14
	moveq	r8, r8, lsr #9
	orreq	r12, r8, lsl #10
	orrgt	r12, #(MAX_RGB<<10)	
	
	@ top right pixel luma
	lsr		r11, r11, #8
	sub		r11, #16
	smulbb	rgb_y, r10, r11

	@ red	
	add		r8, rgb_y, r_v0
	movs	r9, r8, asr #14
	moveq	r8, r8, lsr #9
	orreq	r12, r8, lsl #16
	orrgt	r12, #(MAX_RGB<<16)	

	@ green
	sub		r8, rgb_y, g_uv0
	movs	r9, r8, asr #14
	moveq	r8, r8, lsr #9
	orreq	r12, r8, lsl #21
	orrgt	r12, #(MAX_RGB<<21)	

	@ blue
	add		r8, rgb_y, b_u0
	movs	r9, r8, asr #14
	moveq	r8, r8, lsr #9
	orreq	r12, r8, lsl #26
	orrgt	r12, #(MAX_RGB<<26)	
	
	@ store 2 pixels
	str		r12, [r0], #4
	
	@ bottom row
	add		r11, r1, #Y_STRIDE
	ldrh	r11, [r11, #-2]		@ load Y value for 2 pixels
	
	@ bottom left pixel luma
	and		r12, r11, #0xFF
	sub		r12, #16
	smulbb	rgb_y, r10, r12
	
	@ red	
	mov		r12, #(1<<15)
	add		r8, rgb_y, r_v0
	movs	r9, r8, asr #14
	orreq	r12, r8, lsr #9
	orrgt	r12, #MAX_RGB	

	@ green
	sub		r8, rgb_y, g_uv0
	movs	r9, r8, asr #14
	moveq	r8, r8, lsr #9
	orreq	r12, r8, lsl #5
	orrgt	r12, #(MAX_RGB<<5)	

	@ blue
	add		r8, rgb_y, b_u0
	movs	r9, r8, asr #14
	moveq	r8, r8, lsr #9
	orreq	r12, r8, lsl #10
	orrgt	r12, #(MAX_RGB<<10)	
	
	@ bottom right pixel luma
	lsr		r11, #8
	sub		r11, #16
	smulbb	rgb_y, r10, r11
	
	@ red	
	add		r8, rgb_y, r_v0
	movs	r9, r8, asr #14
	moveq	r8, r8, lsr #9
	orreq	r12, r8, lsl #16
	orrgt	r12, #(MAX_RGB<<16)	

	@ green
	sub		r8, rgb_y, g_uv0
	movs	r9, r8, asr #14
	moveq	r8, r8, lsr #9
	orreq	r12, r8, lsl #21
	orrgt	r12, #(MAX_RGB<<21)	

	@ blue
	add		r8, rgb_y, b_u0
	movs	r9, r8, asr #14
	moveq	r8, r8, lsr #9
	orreq	r12, r8, lsl #26
	orrgt	r12, #(MAX_RGB<<26)	
	
	@ store 2 pixels
	str		r12, [r0, #(X_STRIDE-4)]

	tst		r0, #0x1FC
	bne		row_loop

	subs	r10, #(2<<16)
	add		r0, #(2*X_STRIDE - 2*WIDTH)
	add		r1, #(2*Y_STRIDE - WIDTH)
	add		r2, #(UV_STRIDE - (WIDTH / 2))
	add		r3, #(UV_STRIDE - (WIDTH / 2))
	bgt		column_loop
	
	add		sp, #8
	ldmfd	sp!, {r4-r11, pc}

.endfunc

.pool


