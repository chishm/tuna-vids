/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Colorspace related header  -
 *
 *  Copyright(C) 2001-2003 Peter Ross <pross@xvid.org>
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
 * $Id: colorspace.h,v 1.7 2004/04/05 20:36:36 edgomez Exp $
 *
 ****************************************************************************/

#ifndef _COLORSPACE_H
#define _COLORSPACE_H

#include "../portab.h"


/* colorspace conversion function (encoder) */

typedef void (packedFunc) (uint8_t * x_ptr,
								 int x_stride,
								 uint8_t * y_src,
								 uint8_t * v_src,
								 uint8_t * u_src,
								 int y_stride,
								 int uv_stride,
								 int width,
								 int height,
								 int vflip);

typedef packedFunc *packedFuncPtr;




/* yv12_to_xxx colorspace conversion functions (decoder) */


/* plain c */
extern void yv12_to_rgb555_c (uint8_t* x_ptr, uint8_t* y_ptr, uint8_t* u_ptr, uint8_t* v_ptr, int height);

typedef void (planarFunc) (
				uint8_t * y_dst, uint8_t * u_dst, uint8_t * v_dst,
				int y_dst_stride, int uv_dst_stride,
				uint8_t * y_src, uint8_t * u_src, uint8_t * v_src,
				int y_src_stride, int uv_src_stride,
				int width, int height, int vflip);
typedef planarFunc *planarFuncPtr;

extern planarFuncPtr yv12_to_yv12;

planarFunc yv12_to_yv12_c;

#ifdef ARCH_IS_IA32
planarFunc yv12_to_yv12_mmx;
planarFunc yv12_to_yv12_xmm;
#endif


#endif							/* _COLORSPACE_H_ */
