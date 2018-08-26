/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Decoder Module -
 *
 *  Copyright(C) 2002      MinChen <chenm001@163.com>
 *               2002-2004 Peter Ross <pross@xvid.org>
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Id: decoder.c,v 1.75.2.2 2006/07/10 15:05:30 Isibaar Exp $
 *
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef BFRAMES_DEC_DEBUG
#define BFRAMES_DEC
#endif

#include "xvid.h"
#include "portab.h"
#include "global.h"

#include "decoder.h"
#include "bitstream/bitstream.h"
#include "bitstream/mbcoding.h"

#include "quant/quant.h"
#include "quant/quant_matrix.h"
#include "dct/idct.h"
#include "utils/mem_transfer.h"
#include "image/interpolate8x8.h"
#include "image/font.h"
#include "image/qpel.h"

#include "bitstream/mbcoding.h"
#include "prediction/mbprediction.h"
#include "utils/timer.h"
#include "utils/emms.h"
#include "motion/motion.h"
#include "motion/gmc.h"

#include "image/image.h"
#include "image/colorspace.h"
#include "image/postprocessing.h"
#include "utils/mem_align.h"

#define DIV2ROUND(n)  (((n)>>1)|((n)&1))
#define DIV2(n)       ((n)>>1)
#define DIVUVMOV(n) (((n) >> 1) + roundtab_79[(n) & 0x3]) //

static void __inline
validate_vector(VECTOR * mv, unsigned int x_pos, unsigned int y_pos, const DECODER * dec)
{
  /* clip a vector to valid range
     prevents crashes if bitstream is broken
  */
  int shift = 5 + dec->quarterpel;
  int xborder_high = (int)(dec->mb_width - x_pos) << shift;
  int xborder_low = (-(int)x_pos-1) << shift;
  int yborder_high = (int)(dec->mb_height - y_pos) << shift;
  int yborder_low = (-(int)y_pos-1) << shift;

#define CHECK_MV(mv) \
  do { \
  if ((mv).x > xborder_high) { \
    DPRINTF(XVID_DEBUG_MV, "mv.x > max -- %d > %d, MB %d, %d", (mv).x, xborder_high, x_pos, y_pos); \
    (mv).x = xborder_high; \
  } else if ((mv).x < xborder_low) { \
    DPRINTF(XVID_DEBUG_MV, "mv.x < min -- %d < %d, MB %d, %d", (mv).x, xborder_low, x_pos, y_pos); \
    (mv).x = xborder_low; \
  } \
  if ((mv).y > yborder_high) { \
    DPRINTF(XVID_DEBUG_MV, "mv.y > max -- %d > %d, MB %d, %d", (mv).y, yborder_high, x_pos, y_pos); \
    (mv).y = yborder_high; \
  } else if ((mv).y < yborder_low) { \
    DPRINTF(XVID_DEBUG_MV, "mv.y < min -- %d < %d, MB %d, %d", (mv).y, yborder_low, x_pos, y_pos); \
    (mv).y = yborder_low; \
  } \
  } while (0)

  CHECK_MV(mv[0]);
  CHECK_MV(mv[1]);
  CHECK_MV(mv[2]);
  CHECK_MV(mv[3]);
}



/* decode an intra macroblock */
void
decoder_mbintra(DECODER * dec,
        MACROBLOCK * pMB,
        const uint32_t x_pos,
        const uint32_t y_pos,
        const uint32_t acpred_flag,
        const uint32_t cbp,
        Bitstream * bs,
        const uint32_t quant,
        const uint32_t intra_dc_threshold,
        const unsigned int bound)
{

  DECLARE_ALIGNED_MATRIX(block, 6, 64, int16_t, CACHE_LINE);
  DECLARE_ALIGNED_MATRIX(data, 6, 64, int16_t, CACHE_LINE);

  uint32_t stride = dec->edged_width;
  uint32_t stride2 = stride / 2;
  uint32_t next_block = stride * 8;
  uint32_t i;
  uint32_t iQuant = pMB->quant;
  uint8_t *pY_Cur, *pU_Cur, *pV_Cur;

  pY_Cur = dec->cur.y + (y_pos << 4) * stride + (x_pos << 4);
  pU_Cur = dec->cur.u + (y_pos << 3) * stride2 + (x_pos << 3);
  pV_Cur = dec->cur.v + (y_pos << 3) * stride2 + (x_pos << 3);

  //memset(block, 0, 6 * 64 * sizeof(int16_t)); /* clear */
  mem_clear_arm (block, 6 * 64 * sizeof(int16_t));

  for (i = 0; i < 6; i++) {
    uint32_t iDcScaler = get_dc_scaler(iQuant, i < 4);
    int16_t predictors[8];
    int start_coeff;

    start_timer();
    predict_acdc(dec->mbs, x_pos, y_pos, dec->mb_width, i, &block[i * 64],
           iQuant, iDcScaler, predictors, bound);
    if (!acpred_flag) {
      pMB->acpred_directions[i] = 0;
    }
    stop_prediction_timer();

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

      DPRINTF(XVID_DEBUG_COEFF,"block[0] %i\n", dc_dif);
    } else {
      start_coeff = 0;
    }

    start_timer();
    if (cbp & (1 << (5 - i))) /* coded */
    {
      int direction = dec->alternate_vertical_scan ?
        2 : pMB->acpred_directions[i];

      get_intra_block(bs, &block[i * 64], direction, start_coeff);
    }
    stop_coding_timer();

    start_timer();
    add_acdc(pMB, i, &block[i * 64], iDcScaler, predictors, dec->bs_version);
    stop_prediction_timer();

    start_timer();
    if (dec->quant_type == 0) {
      dequant_h263_intra(&data[i * 64], &block[i * 64], iQuant, iDcScaler, dec->mpeg_quant_matrices);
    } else {
      dequant_mpeg_intra(&data[i * 64], &block[i * 64], iQuant, iDcScaler, dec->mpeg_quant_matrices);
    }
    stop_iquant_timer();

    start_timer();
    idct((short * const)&data[i * 64]);
    stop_idct_timer();

  }

  if (dec->interlacing && pMB->field_dct) {
    next_block = stride;
    stride *= 2;
  }

  start_timer();
  transfer_16to8copy(pY_Cur, &data[0 * 64], stride);
  transfer_16to8copy(pY_Cur + 8, &data[1 * 64], stride);
  transfer_16to8copy(pY_Cur + next_block, &data[2 * 64], stride);
  transfer_16to8copy(pY_Cur + 8 + next_block, &data[3 * 64], stride);
  transfer_16to8copy(pU_Cur, &data[4 * 64], stride2);
  transfer_16to8copy(pV_Cur, &data[5 * 64], stride2);
  stop_transfer_timer();
}

void
decoder_mb_decode(DECODER * dec,
        const uint32_t cbp,
        Bitstream * bs,
        uint8_t * pY_Cur,
        uint8_t * pU_Cur,
        uint8_t * pV_Cur,
        const MACROBLOCK * pMB)
{
  DECLARE_ALIGNED_MATRIX(data, 1, 64, int16_t, CACHE_LINE);

  int stride = dec->edged_width;
  int i;
  const uint32_t iQuant = pMB->quant;
  const int direction = dec->alternate_vertical_scan ? 2 : 0;
  typedef void (*get_inter_block_function_t)(
      Bitstream * bs,
      int16_t * block,
      int direction,
      const int quant,
      const uint16_t *matrix);
  typedef void (*add_residual_function_t)(
      uint8_t *predicted_block,
      const int16_t *residual,
      int stride);

  const get_inter_block_function_t get_inter_block = (dec->quant_type == 0)
    ? (get_inter_block_function_t)get_inter_block_h263
    : (get_inter_block_function_t)get_inter_block_mpeg;

  uint8_t *dst[6];
  int strides[6];


  if (dec->interlacing && pMB->field_dct) {
    dst[0] = pY_Cur;
    dst[1] = pY_Cur + 8;
    dst[2] = pY_Cur + stride;
    dst[3] = dst[2] + 8;
    dst[4] = pU_Cur;
    dst[5] = pV_Cur;
    strides[0] = strides[1] = strides[2] = strides[3] = stride*2;
    strides[4] = stride/2;
    strides[5] = stride/2;
  } else {
    dst[0] = pY_Cur;
    dst[1] = pY_Cur + 8;
    dst[2] = pY_Cur + 8*stride;
    dst[3] = dst[2] + 8;
    dst[4] = pU_Cur;
    dst[5] = pV_Cur;
    strides[0] = strides[1] = strides[2] = strides[3] = stride;
    strides[4] = stride/2;
    strides[5] = stride/2;
  }

  for (i = 0; i < 6; i++) {
    /* Process only coded blocks */
    if (cbp & (1 << (5 - i))) {

      /* Clear the block */
      memset(&data[0], 0, 64*sizeof(int16_t));

      /* Decode coeffs and dequantize on the fly */
      start_timer();
      get_inter_block(bs, &data[0], direction, iQuant, get_inter_matrix(dec->mpeg_quant_matrices));
      stop_coding_timer();

      /* iDCT */
      start_timer();
      idct((short * const)&data[0]);
      stop_idct_timer();

      /* Add this residual to the predicted block */
      start_timer();
      transfer_16to8add(dst[i], &data[0], strides[i]);
      stop_transfer_timer();
    }
  }
}

/* Up to this version, chroma rounding was wrong with qpel. 
 * So we try to be backward compatible to avoid artifacts */
#define BS_VERSION_BUGGY_CHROMA_ROUNDING 1

/* decode an inter macroblock */
void
decoder_mbinter(DECODER * dec,
        const MACROBLOCK * pMB,
        const uint32_t x_pos,
        const uint32_t y_pos,
        const uint32_t cbp,
        Bitstream * bs,
        const uint32_t rounding,
        const int ref,
		const int bvop)
{
  uint32_t stride = dec->edged_width;
  uint32_t stride2 = stride / 2;
  uint32_t i;

  uint8_t *pY_Cur, *pU_Cur, *pV_Cur;

  int uv_dx, uv_dy;
  VECTOR mv[4]; /* local copy of mvs */

  pY_Cur = dec->cur.y + (y_pos << 4) * stride + (x_pos << 4);
  pU_Cur = dec->cur.u + (y_pos << 3) * stride2 + (x_pos << 3);
  pV_Cur = dec->cur.v + (y_pos << 3) * stride2 + (x_pos << 3);
  for (i = 0; i < 4; i++)
    mv[i] = pMB->mvs[i];

  validate_vector(mv, x_pos, y_pos, dec);

  start_timer();

  if ((pMB->mode != MODE_INTER4V) || (bvop)) { /* INTER, INTER_Q, NOT_CODED, FORWARD, BACKWARD */

    uv_dx = mv[0].x;
    uv_dy = mv[0].y;
    if (dec->quarterpel) {
			if (dec->bs_version <= BS_VERSION_BUGGY_CHROMA_ROUNDING) {
  				uv_dx = (uv_dx>>1) | (uv_dx&1);
				uv_dy = (uv_dy>>1) | (uv_dy&1);
			}
			else {
        uv_dx /= 2;
        uv_dy /= 2;
      }
    }
    uv_dx = (uv_dx >> 1) + roundtab_79[uv_dx & 0x3];
    uv_dy = (uv_dy >> 1) + roundtab_79[uv_dy & 0x3];

    if (dec->quarterpel)
      interpolate16x16_quarterpel(dec->cur.y, dec->refn[ref].y, dec->qtmp.y, dec->qtmp.y + 64,
                  dec->qtmp.y + 128, 16*x_pos, 16*y_pos,
                      mv[0].x, mv[0].y, stride, rounding);
    else
      interpolate16x16_switch(dec->cur.y, dec->refn[ref].y, 16*x_pos, 16*y_pos,
                  mv[0].x, mv[0].y, stride, rounding);

  } else {  /* MODE_INTER4V */

    if(dec->quarterpel) {
			if (dec->bs_version <= BS_VERSION_BUGGY_CHROMA_ROUNDING) {
				int z;
				uv_dx = 0; uv_dy = 0;
				for (z = 0; z < 4; z++) {
				  uv_dx += ((mv[z].x>>1) | (mv[z].x&1));
				  uv_dy += ((mv[z].y>>1) | (mv[z].y&1));
				}
			}
			else {
        uv_dx = (mv[0].x / 2) + (mv[1].x / 2) + (mv[2].x / 2) + (mv[3].x / 2);
        uv_dy = (mv[0].y / 2) + (mv[1].y / 2) + (mv[2].y / 2) + (mv[3].y / 2);
      }
    } else {
      uv_dx = mv[0].x + mv[1].x + mv[2].x + mv[3].x;
      uv_dy = mv[0].y + mv[1].y + mv[2].y + mv[3].y;
    }

    uv_dx = (uv_dx >> 3) + roundtab_76[uv_dx & 0xf];
    uv_dy = (uv_dy >> 3) + roundtab_76[uv_dy & 0xf];

    if (dec->quarterpel) {
      interpolate8x8_quarterpel(dec->cur.y, dec->refn[0].y , dec->qtmp.y, dec->qtmp.y + 64,
                  dec->qtmp.y + 128, 16*x_pos, 16*y_pos,
                  mv[0].x, mv[0].y, stride, rounding);
      interpolate8x8_quarterpel(dec->cur.y, dec->refn[0].y , dec->qtmp.y, dec->qtmp.y + 64,
                  dec->qtmp.y + 128, 16*x_pos + 8, 16*y_pos,
                  mv[1].x, mv[1].y, stride, rounding);
      interpolate8x8_quarterpel(dec->cur.y, dec->refn[0].y , dec->qtmp.y, dec->qtmp.y + 64,
                  dec->qtmp.y + 128, 16*x_pos, 16*y_pos + 8,
                  mv[2].x, mv[2].y, stride, rounding);
      interpolate8x8_quarterpel(dec->cur.y, dec->refn[0].y , dec->qtmp.y, dec->qtmp.y + 64,
                  dec->qtmp.y + 128, 16*x_pos + 8, 16*y_pos + 8,
                  mv[3].x, mv[3].y, stride, rounding);
    } else {
      interpolate8x8_switch(dec->cur.y, dec->refn[0].y , 16*x_pos, 16*y_pos,
                mv[0].x, mv[0].y, stride, rounding);
      interpolate8x8_switch(dec->cur.y, dec->refn[0].y , 16*x_pos + 8, 16*y_pos,
                mv[1].x, mv[1].y, stride, rounding);
      interpolate8x8_switch(dec->cur.y, dec->refn[0].y , 16*x_pos, 16*y_pos + 8,
                mv[2].x, mv[2].y, stride, rounding);
      interpolate8x8_switch(dec->cur.y, dec->refn[0].y , 16*x_pos + 8, 16*y_pos + 8,
                mv[3].x, mv[3].y, stride, rounding);
    }
  }

  /* chroma */
  interpolate8x8_switch(dec->cur.u, dec->refn[ref].u, 8 * x_pos, 8 * y_pos,
              uv_dx, uv_dy, stride2, rounding);
  interpolate8x8_switch(dec->cur.v, dec->refn[ref].v, 8 * x_pos, 8 * y_pos,
              uv_dx, uv_dy, stride2, rounding);

  stop_comp_timer();

  if (cbp)
    decoder_mb_decode(dec, cbp, bs, pY_Cur, pU_Cur, pV_Cur, pMB);
}

/* decode an inter macroblock in field mode */
void
decoder_mbinter_field(DECODER * dec,
        const MACROBLOCK * pMB,
        const uint32_t x_pos,
        const uint32_t y_pos,
        const uint32_t cbp,
        Bitstream * bs,
        const uint32_t rounding,
        const int ref,
		const int bvop)
{
  uint32_t stride = dec->edged_width;
  uint32_t stride2 = stride / 2;

  uint8_t *pY_Cur, *pU_Cur, *pV_Cur;

  int uvtop_dx, uvtop_dy;
  int uvbot_dx, uvbot_dy;
  VECTOR mv[4]; /* local copy of mvs */

  /* Get pointer to memory areas */
  pY_Cur = dec->cur.y + (y_pos << 4) * stride + (x_pos << 4);
  pU_Cur = dec->cur.u + (y_pos << 3) * stride2 + (x_pos << 3);
  pV_Cur = dec->cur.v + (y_pos << 3) * stride2 + (x_pos << 3);
  
  mv[0] = pMB->mvs[0];
  mv[1] = pMB->mvs[1];
  memset(&mv[2],0,2*sizeof(VECTOR));

  validate_vector(mv, x_pos, y_pos, dec);

  start_timer();

  if((pMB->mode!=MODE_INTER4V) || (bvop))   /* INTER, INTER_Q, NOT_CODED, FORWARD, BACKWARD */
  { 
    /* Prepare top field vector */
    uvtop_dx = DIV2ROUND(mv[0].x);
    uvtop_dy = DIV2ROUND(mv[0].y);

    /* Prepare bottom field vector */
    uvbot_dx = DIV2ROUND(mv[1].x);
    uvbot_dy = DIV2ROUND(mv[1].y);

    if(dec->quarterpel)
    {
      /* NOT supported */
    }
    else
    {
      /* Interpolate top field left part(we use double stride for every 2nd line) */
      interpolate8x8_switch(dec->cur.y,dec->refn[ref].y+pMB->field_for_top*stride,
                            16*x_pos,8*y_pos,mv[0].x, mv[0].y>>1,2*stride, rounding);
      /* top field right part */
      interpolate8x8_switch(dec->cur.y,dec->refn[ref].y+pMB->field_for_top*stride,
                            16*x_pos+8,8*y_pos,mv[0].x, mv[0].y>>1,2*stride, rounding);

      /* Interpolate bottom field left part(we use double stride for every 2nd line) */
      interpolate8x8_switch(dec->cur.y+stride,dec->refn[ref].y+pMB->field_for_bot*stride,
                            16*x_pos,8*y_pos,mv[1].x, mv[1].y>>1,2*stride, rounding);
      /* Bottom field right part */
      interpolate8x8_switch(dec->cur.y+stride,dec->refn[ref].y+pMB->field_for_bot*stride,
                            16*x_pos+8,8*y_pos,mv[1].x, mv[1].y>>1,2*stride, rounding);

      /* Interpolate field1 U */
      interpolate8x4_switch(dec->cur.u,dec->refn[ref].u+pMB->field_for_top*stride2,
                            8*x_pos,4*y_pos,uvtop_dx,DIV2ROUND(uvtop_dy),stride,rounding);
      
      /* Interpolate field1 V */
      interpolate8x4_switch(dec->cur.v,dec->refn[ref].v+pMB->field_for_top*stride2,
                            8*x_pos,4*y_pos,uvtop_dx,DIV2ROUND(uvtop_dy),stride,rounding);
    
      /* Interpolate field2 U */
      interpolate8x4_switch(dec->cur.u+stride2,dec->refn[ref].u+pMB->field_for_bot*stride2,
                            8*x_pos,4*y_pos,uvbot_dx,DIV2ROUND(uvbot_dy),stride,rounding);
    
      /* Interpolate field2 V */
      interpolate8x4_switch(dec->cur.v+stride2,dec->refn[ref].v+pMB->field_for_bot*stride2,
                            8*x_pos,4*y_pos,uvbot_dx,DIV2ROUND(uvbot_dy),stride,rounding);
    }
  } 
  else 
  {
    /* We don't expect 4 motion vectors in interlaced mode */
  }

  stop_comp_timer();

  /* Must add error correction? */
  if(cbp)
   decoder_mb_decode(dec, cbp, bs, pY_Cur, pU_Cur, pV_Cur, pMB);
}

