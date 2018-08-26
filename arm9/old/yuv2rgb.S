
@ void yv12_to_rgb555_asm (uint8_t* x_ptr, uint8_t* y_ptr, uint8_t* u_ptr, uint8_t* v_ptr, int height);

	.align
	.global	yv12_to_rgb555_asm
	.func	yv12_to_rgb555_asm

.equ	shift_factor,	13
.equ	RGB_Y_factor,	9536
.equ	B_U_factor,		16532
.equ	G_U_factor,		3203
.equ	G_V_factor,		6660
.equ	R_V_factor,		13075
.equ	final_shift,	16
.equ	max_value,		31
.equ	high_bits,		0xffe00000

.equ	x_stride,		512
.equ	y_stride,		384
.equ	uv_stride,		192
.equ	width,			256

@ r0	: u8* x_ptr
@ r1	: u8* y_ptr
@ r2	: u8* u_ptr
@ r3	: u8* v_ptr
@ sp[0]	: int height
@ sp[4]	: int vflip

G_U_factor_addr:
	.long	G_U_factor
G_V_factor_addr:
	.long	G_V_factor
B_U_factor_addr:
	.long	B_U_factor
R_V_factor_addr:
	.long	R_V_factor
	
yv12_to_rgb555_asm:

	stmfd	sp!, {r4-r11, lr}
	
	ldr		r4, [sp]
row_loop:

column_loop:

	ldrb	r5, [r2], #1		@ read U component for 2x2 pixel block
	sub		r5, #128
	ldrb	r7, [r3], #1		@ read V component
	sub		r7, #128
	@ calculate green component first
	ldrh	r8, G_U_factor_addr
	mul		r6, r8, r5
	ldrh	r9, G_V_factor_addr
	mla		r6, r9, r7, r6
	@ calculate blue component, destroying read U value
	ldrh	r8, B_U_factor_addr
	mul		r5, r8, r5
	@ calculate red component, destroying read V value
	ldr		r9, R_V_factor_addr
	mul		r7, r9, r7
	
	@ r5 = blue, r6 = green, r7 = red
	
@ Now work on each pixel using their Y value
	@ top left
	ldrb	r8, [r1], #1
	sub		r8, #16
	ldrh	r11, =RGB_Y_factor
	mul		r8, r11, r8
	sub		r9, r8, r6
	add		r10, r8, r7
	ldr		r12, =high_bits
	add		r8, r8, r5
	@ r8 = blue, r9 = green, r10 = red
	tst		r8, r12
	movlt	r8, #0
	lsreq	r8, #final_shift
	movgt	r8, #max_value
	tst		r9, r12
	movlt	r9, #0
	lsreq	r9, #final_shift
	movgt	r9, #max_value
	tst		r10, r12
	movlt	lr, #0
	lsreq	lr, r10, #final_shift
	movgt	lr, #max_value
	orr		lr, r9, lsl #5
	orr		lr, r8, lsl #10
	@ top right
	ldrb	r8, [r1], #1
	sub		r8, #16
	mul		r8, r11, r8
	sub		r9, r8, r6
	add		r10, r8, r7
	add		r8, r8, r5
	tst		r8, r12
	movlt	r8, #0
	lsreq	r8, #final_shift
	movgt	r8, #max_value
	tst		r9, r12
	movlt	r9, #0
	lsreq	r9, #final_shift
	movgt	r9, #max_value
	tst		r10, r12
	lsreq	r10, r10, #final_shift
	orreq	lr, r10, lsl #16
	orrgt	lr, #(max_value<<16)
	orr		lr, r9, lsl #21
	orr		lr, r8, lsl #26
	@ write double pixel
	str		lr, [r0], #4
	
	@ bottom left
	ldrb	r8, [r1, #(y_stride-2)]
	sub		r8, #16
	mul		r8, r11, r8
	sub		r9, r8, r6
	add		r10, r8, r7
	add		r8, r8, r5
	@ r8 = blue, r9 = green, r10 = red
	tst		r8, r12
	movlt	r8, #0
	lsreq	r8, #final_shift
	movgt	r8, #max_value
	tst		r9, r12
	movlt	r9, #0
	lsreq	r9, #final_shift
	movgt	r9, #max_value
	tst		r10, r12
	movlt	lr, #0
	lsreq	lr, r10, #final_shift
	movgt	lr, #max_value
	orr		lr, r9, lsl #5
	orr		lr, r8, lsl #10
	@ bottom right
	ldrb	r8, [r1, #(y_stride-1)]
	sub		r8, #16
	mul		r8, r11, r8
	sub		r9, r8, r6
	add		r10, r8, r7
	add		r8, r8, r5
	tst		r8, r12
	movlt	r8, #0
	lsreq	r8, #final_shift
	movgt	r8, #max_value
	tst		r9, r12
	movlt	r9, #0
	lsreq	r9, #final_shift
	movgt	r9, #max_value
	tst		r10, r12
	lsreq	r10, r10, #final_shift
	orreq	lr, r10, lsl #16
	orrgt	lr, #(max_value<<16)
	orr		lr, r9, lsl #21
	orr		lr, r8, lsl #26
	@ write double pixel
	str		lr, [r0, #(x_stride-4)]
	
	tst		r0, #0xFF		@ assumes that the output line starts at an address multiple of 256
	bne		column_loop
	
	add		r0, r0, #x_stride
	subs	r4, r4, #2
	add		r1, #(xy_stride)
#	add		r2, #(uv_stride - (width / 2))
#	add		r3, #(uv_stride - (width / 2))
	bne		row_loop
	
	ldmfd	sp!, {r4-r11, pc}

.pool

	.endfunc
