
.arm                                                                               
.global deinterleaveStereo
.func deinterleaveStereo

@ de-interleave a stereo source 
@ Code by Thoduv and ecurtz
@ r0 = left, r1 = right, r2 = interleaved data, r3 = len

deinterleaveStereo:                                                               
	stmfd sp!, {r4-r11}                                                                

	ands	r4, r3, #7					@ get the 3 low bits of len
	beq 	loop						@ if 0 jump to the loop
	rsb 	r4, #8						@ r4 = 8 - r4 (amount of loop to skip)
	sub 	r2, r4, lsl #2				@ adjust data to include what we're actually using
	ldmia	r2!, {r5-r12}				@ load data
	add 	r3, r4						@ bump count as if we were completing full loop
	add 	r4, r4, lsl #1				@ r4 = r4 * 3 instructions per sample
	add 	pc, r4, lsl #2				@ slam the pc forward into the loop (automatically skips next instruction)
                                                                     
loop:                                                                                 
	ldmia	r2!, {r5-r12}

	strh	r5, [r0], #2
	mov 	r5, r5, lsr #16
	strh	r5, [r1], #2

	strh	r6, [r0], #2
	mov 	r6, r6, lsr #16
	strh	r6, [r1], #2

	strh	r7, [r0], #2
	mov 	r7, r7, lsr #16
	strh	r7, [r1], #2

	strh	r8, [r0], #2
	mov 	r8, r8, lsr #16
	strh	r8, [r1], #2

	strh	r9, [r0], #2
	mov 	r9, r9, lsr #16
	strh	r9, [r1], #2
		                                                                               
	strh	r10, [r0], #2
	mov 	r10, r10, lsr #16
	strh	r10, [r1], #2
	
	strh	r11, [r0], #2
	mov 	r11, r11, lsr #16
	strh	r11, [r1], #2

	strh	r12, [r0], #2
	mov 	r12, r12, lsr #16
	strh	r12, [r1], #2

	subs	r3, #8
	bne 	loop

	ldmia sp!, {r4-r11}
	bx lr

.endfunc
.pool

