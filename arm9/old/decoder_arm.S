
.equ STACK_NEEDED, (32 + 1536 + 32)
.equ STACK_USED, (STACK_NEEDED + 52)

@ static void
@ decoder_mbintra

@  DECODER * dec 				r0
.equ ARG_dec, (STACK_USED - 16)
@  MACROBLOCK * pMB				r1
.equ ARG_pMB, (STACK_USED - 12)
@  const uint32_t x_pos			r2
.equ ARG_x_pos, (STACK_USED - 8)
@  const uint32_t y_pos			r3
.equ ARG_y_pos, (STACK_USED - 4)
@  const uint32_t acpred_flag	stack[0]
.equ ARG_acpred_flag, (STACK_USED + 0)
@  const uint32_t cbp			stack[4]
.equ ARG_cbp, (STACK_USED + 4)
@  Bitstream * bs				stack[8]
.equ ARG_bs, (STACK_USED + 8)
@  const uint32_t quant			stack[12]
.equ ARG_quant, (STACK_USED + 12)
@  const uint32_t intra_dc_threshold	stack[16]
.equ ARG_intra_dc_threshold, (STACK_USED + 16)
@  const unsigned int bound		stack[20]
.equ ARG_bound, (STACK_USED + 20)

@ following take 32 bytes total
@  uint32_t stride = dec->edged_width;
.equ VAR_stride, (0)
@  uint32_t stride2 = stride / 2;
.equ VAR_stride2, (4)
@  uint32_t next_block = stride * 8;
.equ VAR_next_block, (8)
@  uint32_t i;
.equ VAR_i, (12)
@  uint32_t iQuant = pMB->quant;
.equ VAR_iQuant, (16)
@  uint8_t *pY_Cur, *pU_Cur, *pV_Cur;
.equ VAR_pY_Cur, (20)
.equ VAR_pU_Cur, (24)
.equ VAR_pV_Cur, (28)

@ following take 1536 bytes total, and must be aligned to a multiple of 32 bytes
@  DECLARE_ALIGNED_MATRIX(block, 6, 64, int16_t, CACHE_LINE);
@  DECLARE_ALIGNED_MATRIX(data, 6, 64, int16_t, CACHE_LINE);
.equ VAR_block, (59)
.equ VAR_matrix_size (768)
.equ VAR_data, (VAR_block + VAR_matrix_size)

@ Macroblock
.equ MB_quant, (240)


	.align
	.global decoder_mbintra_arm
	.func decoder_mbintra_arm
	
decoder_mbintra_arm:

	stmfd	sp!, {r0-r3}
	stmfd	sp!, {r4-r11, lr}
	sub		sp, #STACK_NEEDED
	

  pY_Cur = dec->cur.y + (y_pos << 4) * stride + (x_pos << 4);
  pU_Cur = dec->cur.u + (y_pos << 3) * stride2 + (x_pos << 3);
  pV_Cur = dec->cur.v + (y_pos << 3) * stride2 + (x_pos << 3);

@  memset(block, 0, 6 * 64 * sizeof(int16_t)); /* clear */

	add		r4, sp, #VAR_block
	bic		r4, #0x1F			@ align to cache line
	add		r5, r4, #VAR_matrix_size
	mov		r6, #0
	mov		r7, #0
	mov		r8, #0
	mov		r9, #0
	mov		r10, #0
	mov		r11, #0
	mov		r12, #0
	mov		lr, #0
clear_block_loop:
	stmia	r4!, {r6-r12, lr}
	cmp		r4, r5
	blt		clear_block_loop
	
	@ i = r11

  for (i = 0; i < 6; i++) {
	@ iQuant = pMB->quant
	ldr	r4, [sp, #ARG_pMB]
	ldr	r0, [r4, #MB_quant]
  
	@ iDcScaler = r5
	@ uint32_t iDcScaler = get_dc_scaler(iQuant, i < 4);
	cmp		r0, #5
	movlt	r5, #8
	blt		get_dc_scaler_done
	cmp		r0, #25
	bge		get_dc_scaler_part2
	cmp		r11, #4
	addge	r5, r0, #13
	lsrge	r5, r5, #1
	bge		get_dc_scaler_done
get_dc_scaler_part2:
	cmp		r0, #9
	movlt	r5, r0, lsl #1
	blt		get_dc_scaler_done
	cmp		r0, #25
	addlt	r5, r0, #8
	blt		get_dc_scaler_done
	cmp		r11, #4
	movlt	r5, r0, lsl #1
	sublt	r5, r5, #16
	subgt	r5, r0, #6
get_dc_scaler_done:

	


		
		int16_t predictors[8];
		int start_coeff;

		predict_acdc(dec->mbs, x_pos, y_pos, dec->mb_width, i, &block[i * 64],
			   iQuant, iDcScaler, predictors, bound);
		if (!acpred_flag) {
		  pMB->acpred_directions[i] = 0;
		}

		if (quant < intra_dc_threshold) {
		  int dc_size;
		  int dc_dif;

		  dc_size = i < 4 ? get_dc_size_lum(bs) : get_dc_size_chrom(bs);
		  dc_dif = dc_size ? get_dc_dif(bs, dc_size) : 0;

		  if (dc_size > 8) {
			BitstreamSkip(bs, 1); /* marker */
		  }

		  block[i * 64 + 0] = dc_dif;
		  start_coeff = 1;

		} else {
		  start_coeff = 0;
		}

		if (cbp & (1 << (5 - i))) /* coded */
		{
		  int direction = dec->alternate_vertical_scan ? 2 : pMB->acpred_directions[i];

		  get_intra_block(bs, &block[i * 64], direction, start_coeff);
		}

		add_acdc(pMB, i, &block[i * 64], iDcScaler, predictors, dec->bs_version);

		if (dec->quant_type == 0) {
		  dequant_h263_intra(&data[i * 64], &block[i * 64], iQuant, iDcScaler, dec->mpeg_quant_matrices);
		} else {
		  dequant_mpeg_intra(&data[i * 64], &block[i * 64], iQuant, iDcScaler, dec->mpeg_quant_matrices);
		}

		idct((short * const)&data[i * 64]);

	  }

	  if (dec->interlacing && pMB->field_dct) {
		next_block = stride;
		stride *= 2;
  }

  transfer_16to8copy(pY_Cur, &data[0 * 64], stride);
  transfer_16to8copy(pY_Cur + 8, &data[1 * 64], stride);
  transfer_16to8copy(pY_Cur + next_block, &data[2 * 64], stride);
  transfer_16to8copy(pY_Cur + 8 + next_block, &data[3 * 64], stride);
  transfer_16to8copy(pU_Cur, &data[4 * 64], stride2);
  transfer_16to8copy(pV_Cur, &data[5 * 64], stride2);
}

