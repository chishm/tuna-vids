@ YUV-> RGB conversion code Copyright (C) 2008 Robin Watts (robin@wss.co.uk).
@
@ Licensed under the GPL. If you need it under another license, contact me
@ and ask.
@
@  This program is free software ; you can redistribute it and/or modify
@  it under the terms of the GNU General Public License as published by
@  the Free Software Foundation ; either version 2 of the License, or
@  (at your option) any later version.
@
@  This program is distributed in the hope that it will be useful,
@  but WITHOUT ANY WARRANTY ; without even the implied warranty of
@  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
@  GNU General Public License for more details.
@
@  You should have received a copy of the GNU General Public License
@  along with this program ; if not, write to the Free Software
@  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
@
@
@ The algorithm used here is based heavily on one created by Sophie Wilson
@ of Acorn/e-14/Broadcomm. Many thanks.
@
@ Additional tweaks (in the fast fixup code) are from Paul Gardiner.
@
@ The old implementation of YUV -> RGB did:
@
@ R = CLAMP((Y-16)*1.164 +           1.596*V)
@ G = CLAMP((Y-16)*1.164 - 0.391*U - 0.813*V)
@ B = CLAMP((Y-16)*1.164 + 2.018*U          )
@
@ We're going to bend that here as follows:
@
@ R = CLAMP(y +           1.596*V)
@ G = CLAMP(y - 0.383*U - 0.813*V)
@ B = CLAMP(y + 1.976*U          )
@
@ where y = 0               for       Y <=  16,
@       y = (  Y-16)*1.164, for  16 < Y <= 239,
@       y = (239-16)*1.164, for 239 < Y
@
@ i.e. We clamp Y to the 16 to 239 range (which it is supposed to be in
@ anyway). We then pick the B_U factor so that B never exceeds 511. We then
@ shrink the G_U factor in line with that to avoid a colour shift as much as
@ possible.
@
@ We're going to use tables to do it faster, but rather than doing it using
@ 5 tables as as the above suggests, we're going to do it using just 3.
@
@ We do this by working in parallel within a 32 bit word, and using one
@ table each for Y U and V.
@
@ Source Y values are    0 to 255, so    0.. 260 after scaling
@ Source U values are -128 to 127, so  -49.. 49(G), -253..251(B) after
@ Source V values are -128 to 127, so -204..203(R), -104..103(G) after
@
@ So total summed values:
@ -223 <= R <= 481, -173 <= G <= 431, -253 <= B < 511
@
@ We need to pack R G and B into a 32 bit word, and because of Bs range we
@ need 2 bits above the valid range of B to detect overflow, and another one
@ to detect the sense of the overflow. We therefore adopt the following
@ representation:
@
@ osGGGGGgggggosBBBBBbbbosRRRRRrrr
@
@ Each such word breaks down into 3 ranges.
@
@ osGGGGGggggg   osBBBBBbbb   osRRRRRrrr
@
@ Thus we have 8 bits for each B and R table entry, and 10 bits for G (good
@ as G is the most noticable one). The s bit for each represents the sign,
@ and o represents the overflow.
@
@ For R and B we pack the table by taking the 11 bit representation of their
@ values, and toggling bit 10 in the U and V tables.
@
@ For the green case we calculate 4*G (thus effectively using 10 bits for the
@ valid range) truncate to 12 bits. We toggle bit 11 in the Y table.

.equ   UV_STRIDE,   192
.equ   Y_STRIDE,   384
.equ   X_STRIDE,   512
.equ   WIDTH,      256

.section ".itcm"
.align
.global   yv12_to_rgb555_asm
.func   yv12_to_rgb555_asm

@ void yv12_to_rgb555_asm
@  uint8_t * x_ptr
@  uint8_t * y_ptr
@  uint8_t * u_ptr
@  uint8_t * v_ptr
@  int height

yv12_to_rgb555_asm:

   @ r0 = x_ptr
   @ r1 = y_ptr
   @ r2 = u_ptr
   @ r3 = v_ptr
   @ <> = height
   STMFD   r13!,{r4-r11,r14}

   LDR   r8, =y_table
   LDRB   r14,[r13,#9*4]      @ r14 = height
   ADD   r9, r8,#1024   @ u_table
   ADD   r10,r9,#1024   @ v_table
   LDR   r4, =0x07C07C1F
   LDR   r5, =0x40040100
yloop:
   SUB   r14,r14,#WIDTH<<8   @ r14 = height-(WIDTH<<8)
xloop:
   LDRB   r11,[r2],#1      @ r11 = u  = *u_ptr++
   LDRB   r12,[r3],#1      @ r12 = v  = *v_ptr++
   LDRB   r7, [r1, #Y_STRIDE]   @ r7  = y2 = y_ptr[stride]
   LDRB   r6, [r1],#1      @ r6  = y0 = *y_ptr++
   LDR   r11,[r9, r11,LSL #2]   @ r11 = u  = u_table[u]
   LDR   r12,[r10,r12,LSL #2]   @ r12 = v  = v_table[v]
   LDR   r7, [r8, r7, LSL #2]   @ r7  = y2 = y_table[y2]
   LDR   r6, [r8, r6, LSL #2]   @ r6  = y0 = y_table[y0]
   ADD   r11,r11,r12      @ r11 = uv = u+v

   ADD   r12,r11,r5, LSR #8
   ADD   r7, r7,r12      @ r7  = y0 + uv
   ADD   r6, r6,r12      @ r6  = y2 + uv
   ADD   r6, r6, r5, LSR #7
   ANDS   r12, r7, r5
   TSTEQ   r6, r5
   BNE   fix01
return01:
   AND   r7, r4, r7, LSR #3
   AND   r6, r4, r6, LSR #3
   ORR   r7, r7, r7, LSR #17
   ORR   r6, r6, r6, LSR #17
   ADD   r12,r0, #X_STRIDE
   STRH   r7, [r12]
   LDRB   r12,[r1, #Y_STRIDE]   @ r12 = y3 = y_ptr[stride]
   LDRB   r7, [r1],#1      @ r6  = y1 = *y_ptr++
   STRH   r6, [r0],#2

   LDR   r12,[r8, r12,LSL #2]   @ r7  = y3 = y_table[y2]
   LDR   r6, [r8, r7, LSL #2]   @ r6  = y1 = y_table[y0]

   ADD   r7, r12,r11      @ r7  = y3 + uv
   ADD   r6, r6, r11      @ r6  = y1 + uv
   ADD   r7, r7, r5, LSR #7
   ANDS   r12, r7, r5
   TSTEQ   r6, r5
   BNE   fix02
return02:
   AND   r7, r4, r7, LSR #3
   AND   r6, r4, r6, LSR #3
   ORR   r7, r7, r7, LSR #17
   ORR   r6, r6, r6, LSR #17
   ADD   r12,r0, #X_STRIDE
   STRH   r7, [r12]
   STRH   r6, [r0],#2
   ADDS   r14,r14,#2<<8
   BLT   xloop

   ADD   r0, r0, #2*X_STRIDE - 2*WIDTH   @ x_ptr to next line
   ADD   r1, r1, #2*Y_STRIDE -   WIDTH   @ y_ptr to next line
   ADD   r2, r2, #UV_STRIDE - (WIDTH/2)
   ADD   r3, r3, #UV_STRIDE - (WIDTH/2)

   SUBS   r14,r14,#2
   BGT   yloop

   LDMFD   r13!,{r4-r11, pc}
fix01:
   @ r7 and r6 are the values, at least one of which has overflowed
   @ r12 = r7 & mask = .s......s......s......
   SUB   r12,r12,r12,LSR #8   @ r12 = ..SSSSSS.SSSSSS.SSSSSS
   ORR   r7, r7, r12      @ r7 |= ..SSSSSS.SSSSSS.SSSSSS
   BIC   r12,r5, r7, LSR #1   @ r12 = .o......o......o......
   ADD   r7, r7, r12,LSR #8   @ r7  = fixed value

   AND   r12, r6, r5      @ r12 = .S......S......S......
   SUB   r12,r12,r12,LSR #8   @ r12 = ..SSSSSS.SSSSSS.SSSSSS
   ORR   r6, r6, r12      @ r6 |= ..SSSSSS.SSSSSS.SSSSSS
   BIC   r12,r5, r6, LSR #1   @ r12 = .o......o......o......
   ADD   r6, r6, r12,LSR #8   @ r6  = fixed value
   B   return01
fix02:
   @ r7 and r6 are the values, at least one of which has overflowed
   @ r12 = r7 & mask = .s......s......s......
   SUB   r12,r12,r12,LSR #8   @ r12 = ..SSSSSS.SSSSSS.SSSSSS
   ORR   r7, r7, r12      @ r7 |= ..SSSSSS.SSSSSS.SSSSSS
   BIC   r12,r5, r7, LSR #1   @ r12 = .o......o......o......
   ADD   r7, r7, r12,LSR #8   @ r7  = fixed value

   AND   r12, r6, r5      @ r12 = .S......S......S......
   SUB   r12,r12,r12,LSR #8   @ r12 = ..SSSSSS..SSSSS.SSSSSS
   ORR   r6, r6, r12      @ r6 |= ..SSSSSS..SSSSS.SSSSSS
   BIC   r12,r5, r6, LSR #1   @ r12 = .o......o......o......
   ADD   r6, r6, r12,LSR #8   @ r6  = fixed value
   B   return02
.endfunc
.pool

