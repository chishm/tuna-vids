
.equ	SCALEBITS_OUT,	6
.equ	SCALEBITS_DISP, 9
.equ	MAX_RGB,		31
.equ	WIDTH,			256
.equ	X_STRIDE,		512
.equ	Y_STRIDE,		384
.equ	UV_STRIDE,		192

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
	
@ cache table pointers into stack for quick access
	ldr		r4, = B_U_tab
	ldr		r5, = G_U_tab
	ldr		r6, = G_V_tab
	ldr		r7, = R_V_tab
@	ldr		lr, = RGB_Y_tab
@	mov		r4, #129
@	mov		r5, #25
@	mov		r6, #52
@	mov		r7, #102
	mov		lr, #74
	stmfd	sp!, {r4-r7}
	
b_u0	.req r4
g_uv0	.req r5
r_v0	.req r6
rgb_y	.req r7
b_val	.req r8
g_val	.req r9
r_val	.req r10

column_loop:
	
row_loop:
	
	ldmfd	sp, {r7-r10}		@ load table offsets
	ldrb	r11, [r2], #1		@ load U value
	ldrb	r12, [r3], #1		@ load V value
	@ lookup component values from tables
	lsl		r11, #1
	lsl		r12, #1
@sub	r11, #128
@sub r12, #128
	ldrsh	r6, [r8, r11]
	ldrsh	r5,	[r9, r12]
	ldrsh	b_u0, [r7, r11]
	add		g_uv0, r6, r5
	ldrsh	r_v0, [r10, r12]
@mul b_u0, r7, r11
@mul	g_uv0, r8, r11
@mla	g_uv0, r9, r12, g_uv0
@mul r_v0, r10, r12
	
	@ top row
	ldrh	r11, [r1], #2		@ load Y value for 2 pixels
	
	@ top left pixel
	and		r12, r11, #0xFF
@	lsl		r12, #1
@	ldrsh	rgb_y, [lr, r12]	@ lookup brightness correction value
sub		r12, #16
mul		rgb_y, lr, r12
	adds	b_val, rgb_y, b_u0
	movlt	b_val, #0
	lsrge	b_val, #SCALEBITS_DISP
	subs	g_val, rgb_y, g_uv0
	movlt	g_val, #0
	lsrge	g_val, #SCALEBITS_DISP
	adds	r_val, rgb_y, r_v0
	movlt	r12, #0
	lsrge	r12, r_val, #SCALEBITS_DISP
	cmp		r12, #MAX_RGB
	movge	r12, #MAX_RGB
	cmp		g_val, #MAX_RGB
	orrge	r12, #(MAX_RGB<<5)
	orrlt	r12, g_val, lsl #5
	cmp		b_val, #MAX_RGB
	orrge	r12, #(MAX_RGB<<10)
	orrlt	r12, b_val, lsl #10
	
	@ top right pixel
@	lsr		r11, r11, #7
@	ldrsh	rgb_y, [lr, r11]	@ lookup brightness correction value
lsr		r11, r11, #8
sub		r11, #16
mul		rgb_y, lr, r11
	adds	b_val, rgb_y, b_u0
	movlt	b_val, #0
	lsrge	b_val, #SCALEBITS_DISP
	subs	g_val, rgb_y, g_uv0
	movlt	g_val, #0
	lsrge	g_val, #SCALEBITS_DISP
	adds	r_val, rgb_y, r_v0
	movlt	r_val, #0
	lsrge	r_val, #SCALEBITS_DISP
	cmp		r_val, #MAX_RGB
	orrge	r12, #(MAX_RGB<<16)
	orrlt	r12, r_val, lsl #16
	cmp		g_val, #MAX_RGB
	orrge	r12, #(MAX_RGB<<21)
	orrlt	r12, g_val, lsl #21
	cmp		b_val, #MAX_RGB
	orrge	r12, #(MAX_RGB<<26)
	orrlt	r12, b_val, lsl #26
	
	@ store 2 pixels
	str		r12, [r0], #4
	
	@ bottom row
	add		r11, r1, #Y_STRIDE
	ldrh	r11, [r11, #-2]		@ load Y value for 2 pixels
	
	@ bottom left pixel
	and		r12, r11, #0xFF
@	lsl		r12, #1
@	ldrsh	rgb_y, [lr, r12]	@ lookup brightness correction value
sub		r12, #16
mul		rgb_y, lr, r12
	adds	b_val, rgb_y, b_u0
	movlt	b_val, #0
	lsrge	b_val, #SCALEBITS_DISP
	subs	g_val, rgb_y, g_uv0
	movlt	g_val, #0
	lsrge	g_val, #SCALEBITS_DISP
	adds	r_val, rgb_y, r_v0
	movlt	r12, #0
	lsrge	r12, r_val, #SCALEBITS_DISP
	cmp		r12, #MAX_RGB
	movge	r12, #MAX_RGB
	cmp		g_val, #MAX_RGB
	orrge	r12, #(MAX_RGB<<5)
	orrlt	r12, g_val, lsl #5
	cmp		b_val, #MAX_RGB
	orrge	r12, #(MAX_RGB<<10)
	orrlt	r12, b_val, lsl #10
	
	@ bottom right pixel
@	lsr		r11, r11, #7
@	ldrsh	rgb_y, [lr, r11]	@ lookup brightness correction value
lsr	r11, #8
sub	r11, #16
mul	rgb_y, lr, r11
	adds	b_val, rgb_y, b_u0
	movlt	b_val, #0
	lsrge	b_val, #SCALEBITS_DISP
	subs	g_val, rgb_y, g_uv0
	movlt	g_val, #0
	lsrge	g_val, #SCALEBITS_DISP
	adds	r_val, rgb_y, r_v0
	movlt	r_val, #0
	lsrge	r_val, #SCALEBITS_DISP
	cmp		r_val, #MAX_RGB
	orrge	r12, #(MAX_RGB<<16)
	orrlt	r12, r_val, lsl #16
	cmp		g_val, #MAX_RGB
	orrge	r12, #(MAX_RGB<<21)
	orrlt	r12, g_val, lsl #21
	cmp		b_val, #MAX_RGB
	orrge	r12, #(MAX_RGB<<26)
	orrlt	r12, b_val, lsl #26
	
	@ store 2 pixels
	str		r12, [r0, #(X_STRIDE-4)]

	tst		r0, #0x1FC
	bne		row_loop

	ldr		r12, [sp, #52]
	add		r0, #(2*X_STRIDE - 2*WIDTH)
	add		r1, #(2*Y_STRIDE - WIDTH)
	subs	r12, #2
	add		r2, #(UV_STRIDE - (WIDTH / 2))
	add		r3, #(UV_STRIDE - (WIDTH / 2))
	str		r12, [sp, #52]
	bne		column_loop
	
	add		sp, #16
	ldmfd	sp!, {r4-r11, pc}

.endfunc

.pool

.align 1

RGB_Y_tab:
	.space 512
B_U_tab:
	.space 512
G_U_tab:
	.space 512
G_V_tab:
	.space 512
R_V_tab:
	.space 512

.func colorspace_init
.global colorspace_init


colorspace_init:
	stmfd	sp!, {r4-r9}
	
	ldr		r5, =RGB_Y_tab
	ldr		r6, =B_U_tab
	ldr		r7, =G_U_tab
	ldr		r8, =G_V_tab
	ldr		r9, =R_V_tab
	
	mov		r0, #0
colorspace_init_loop:
	lsl		r4, r0, #1
	mov		r1, #74
	sub		r2, r0, #16
	mul		r3, r1, r2
	strh	r3, [r5,r4]
	sub		r2, r0, #128
	mov		r1, #129
	mul		r3, r1, r2
	strh	r3, [r6,r4]
	mov		r1, #25
	mul		r3, r1, r2
	strh	r3, [r7,r4]
	mov		r1, #52
	mul		r3, r1, r2
	strh	r3, [r8,r4]
	mov		r1, #102
	mul		r3, r1, r2
	strh	r3, [r9,r4]

	add		r0, #1
	cmp		r0, #256
	blt		colorspace_init_loop
	
	ldmfd	sp!, {r4-r9}
	bx 		lr

.endfunc

