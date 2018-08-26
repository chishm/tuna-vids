/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Colorspace conversion functions -
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
 * $Id: colorspace.c,v 1.11 2005/11/22 10:23:01 suxen_drol Exp $
 *
 ****************************************************************************/

#include <string.h>				/* memcpy */

#include "../global.h"
#include "colorspace.h"

/* function pointers */

/* output */
planarFuncPtr yv12_to_yv12;

/*
extern int16_t *RGB_Y_tab;
extern int16_t *B_U_tab;
extern int16_t *G_U_tab;
extern int16_t *G_V_tab;
extern int16_t *R_V_tab;
*/


/********** generic colorspace macro **********/


#define MAKE_COLORSPACE(NAME,SIZE,PIXELS,VPIXELS,FUNC,C1,C2,C3,C4) \
void	\
NAME(uint8_t * x_ptr, int x_stride,	\
				 uint8_t * y_ptr, uint8_t * u_ptr, uint8_t * v_ptr,	\
				 int y_stride, int uv_stride,	\
				 int width, int height, int vflip)	\
{	\
	int fixed_width = (width + 1) & ~1;				\
	int x_dif = x_stride - (SIZE)*fixed_width;		\
	int y_dif = y_stride - fixed_width;				\
	int uv_dif = uv_stride - (fixed_width / 2);		\
	int x, y;										\
	if (vflip) {								\
		x_ptr += (height - 1) * x_stride;			\
		x_dif = -(SIZE)*fixed_width - x_stride;		\
		x_stride = -x_stride;						\
	}												\
	for (y = 0; y < height; y+=(VPIXELS)) {			\
		FUNC##_ROW(SIZE,C1,C2,C3,C4);				\
		for (x = 0; x < fixed_width; x+=(PIXELS)) {	\
			FUNC(SIZE,C1,C2,C3,C4);				\
			x_ptr += (PIXELS)*(SIZE);				\
			y_ptr += (PIXELS);						\
			u_ptr += (PIXELS)/2;					\
			v_ptr += (PIXELS)/2;					\
		}											\
		x_ptr += x_dif + (VPIXELS-1)*x_stride;		\
		y_ptr += y_dif + (VPIXELS-1)*y_stride;		\
		u_ptr += uv_dif + ((VPIXELS/2)-1)*uv_stride;	\
		v_ptr += uv_dif + ((VPIXELS/2)-1)*uv_stride;	\
	}												\
}



/********** colorspace output (yv12_to_xxx) functions **********/

/* yuv -> rgb def's */

#define RGB_Y_OUT		1.164
#define B_U_OUT			2.018
#define Y_ADD_OUT		16

#define G_U_OUT			0.391
#define G_V_OUT			0.813
#define U_ADD_OUT		128

#define R_V_OUT			1.596
#define V_ADD_OUT		128

#define SCALEBITS_OUT	6	// Enough for a 16 bit table with room to spare
#define SCALEBITS_DISP	(SCALEBITS_OUT + 3)	// scale from 8bpp to 5bpp
#define FIX_OUT(x)		((uint16_t) ((x) * (1L<<SCALEBITS_OUT) + 0.5))

#define MAX_RGB		31

/* rgb16/rgb16i output */

// Modified by Chishm

#define MK_RGB555(R,G,B)	\
	((MAX(0,MIN(255, B)) << 7) & 0x7c00) | \
	((MAX(0,MIN(255, G)) << 2) & 0x03e0) | \
	((MAX(0,MIN(255, R)) >> 3) & 0x001f)

#define MK_RGB565(R,G,B)	\
	((MAX(0,MIN(255, R)) << 8) & 0xf800) | \
	((MAX(0,MIN(255, G)) << 3) & 0x07e0) | \
	((MAX(0,MIN(255, B)) >> 3) & 0x001f)

#define WRITE_RGB16(ROW,UV_ROW,C1)	\
	rgb_y = RGB_Y_tab[ y_ptr[y_stride*(ROW) + 0] ];						\
	b[ROW] = (b[ROW] & 0x7) + ((rgb_y + b_u##UV_ROW) >> SCALEBITS_OUT);	\
	g[ROW] = (g[ROW] & 0x7) + ((rgb_y - g_uv##UV_ROW) >> SCALEBITS_OUT);	\
	r[ROW] = (r[ROW] & 0x7) + ((rgb_y + r_v##UV_ROW) >> SCALEBITS_OUT);		\
	*(uint16_t *) (x_ptr+((ROW)*x_stride)+0) = C1(r[ROW], g[ROW], b[ROW]);	\
	rgb_y = RGB_Y_tab[ y_ptr[y_stride*(ROW) + 1] ];				\
	b[ROW] = (b[ROW] & 0x7) + ((rgb_y + b_u##UV_ROW) >> SCALEBITS_OUT);		\
	g[ROW] = (g[ROW] & 0x7) + ((rgb_y - g_uv##UV_ROW) >> SCALEBITS_OUT);	\
	r[ROW] = (r[ROW] & 0x7) + ((rgb_y + r_v##UV_ROW) >> SCALEBITS_OUT);		\
	*(uint16_t *) (x_ptr+((ROW)*x_stride)+2) = C1(r[ROW], g[ROW], b[ROW]);

#define YV12_TO_RGB16_ROW(SIZE,C1,C2,C3,C4) \
	int r[2], g[2], b[2];					\
	r[0] = r[1] = g[0] = g[1] = b[0] = b[1] = 0;
#define YV12_TO_RGB16(SIZE,C1,C2,C3,C4)		\
	int rgb_y; 												\
	int b_u0 = B_U_tab[ u_ptr[0] ];								\
	int g_uv0 = G_U_tab[ u_ptr[0] ] + G_V_tab[ v_ptr[0] ];		\
	int r_v0 = R_V_tab[ v_ptr[0] ];								\
	WRITE_RGB16(0, 0, C1)										\
	WRITE_RGB16(1, 0, C1)

#define YV12_TO_RGB16I_ROW(SIZE,C1,C2,C3,C4) \
	int r[4], g[4], b[4];					\
	r[0] = r[1] = r[2] = r[3] = 0;			\
	g[0] = g[1] = g[2] = g[3] = 0;			\
	b[0] = b[1] = b[2] = b[3] = 0;
#define YV12_TO_RGB16I(SIZE,C1,C2,C3,C4)		\
	int rgb_y; 													\
	int b_u0 = B_U_tab[ u_ptr[0] ];								\
	int g_uv0 = G_U_tab[ u_ptr[0] ] + G_V_tab[ v_ptr[0] ];		\
	int r_v0 = R_V_tab[ v_ptr[0] ];								\
    int b_u1 = B_U_tab[ u_ptr[uv_stride] ];						\
	int g_uv1 = G_U_tab[ u_ptr[uv_stride] ] + G_V_tab[ v_ptr[uv_stride] ];	\
	int r_v1 = R_V_tab[ v_ptr[uv_stride] ];						\
    WRITE_RGB16(0, 0, C1)										\
	WRITE_RGB16(1, 1, C1)										\
    WRITE_RGB16(2, 0, C1)										\
	WRITE_RGB16(3, 1, C1)										\


/* rgb/rgbi output */

#define WRITE_RGB(SIZE,ROW,UV_ROW,C1,C2,C3,C4)	\
	rgb_y = RGB_Y_tab[ y_ptr[(ROW)*y_stride + 0] ];						\
	x_ptr[(ROW)*x_stride+(C3)] = MAX(0, MIN(255, (rgb_y + b_u##UV_ROW) >> SCALEBITS_OUT));	\
	x_ptr[(ROW)*x_stride+(C2)] = MAX(0, MIN(255, (rgb_y - g_uv##UV_ROW) >> SCALEBITS_OUT));	\
	x_ptr[(ROW)*x_stride+(C1)] = MAX(0, MIN(255, (rgb_y + r_v##UV_ROW) >> SCALEBITS_OUT));	\
	if ((SIZE)>3) x_ptr[(ROW)*x_stride+(C4)] = 0;									\
	rgb_y = RGB_Y_tab[ y_ptr[(ROW)*y_stride + 1] ];									\
	x_ptr[(ROW)*x_stride+(SIZE)+(C3)] = MAX(0, MIN(255, (rgb_y + b_u##UV_ROW) >> SCALEBITS_OUT));	\
	x_ptr[(ROW)*x_stride+(SIZE)+(C2)] = MAX(0, MIN(255, (rgb_y - g_uv##UV_ROW) >> SCALEBITS_OUT));	\
	x_ptr[(ROW)*x_stride+(SIZE)+(C1)] = MAX(0, MIN(255, (rgb_y + r_v##UV_ROW) >> SCALEBITS_OUT));	\
	if ((SIZE)>3) x_ptr[(ROW)*x_stride+(SIZE)+(C4)] = 0;


#define YV12_TO_RGB_ROW(SIZE,C1,C2,C3,C4) 	/* nothing */
#define YV12_TO_RGB(SIZE,C1,C2,C3,C4)				\
	int rgb_y;												\
	int b_u0 = B_U_tab[ u_ptr[0] ];							\
	int g_uv0 = G_U_tab[ u_ptr[0] ] + G_V_tab[ v_ptr[0] ];	\
	int r_v0 = R_V_tab[ v_ptr[0] ];							\
	WRITE_RGB(SIZE, 0, 0, C1,C2,C3,C4)						\
	WRITE_RGB(SIZE, 1, 0, C1,C2,C3,C4)

#define YV12_TO_RGBI_ROW(SIZE,C1,C2,C3,C4) 	/* nothing */
#define YV12_TO_RGBI(SIZE,C1,C2,C3,C4)				\
	int rgb_y;												\
	int b_u0 = B_U_tab[ u_ptr[0] ];							\
	int g_uv0 = G_U_tab[ u_ptr[0] ] + G_V_tab[ v_ptr[0] ];	\
	int r_v0 = R_V_tab[ v_ptr[0] ];							\
    int b_u1 = B_U_tab[ u_ptr[uv_stride] ];					\
	int g_uv1 = G_U_tab[ u_ptr[uv_stride] ] + G_V_tab[ v_ptr[uv_stride] ];	\
	int r_v1 = R_V_tab[ v_ptr[uv_stride] ];					\
	WRITE_RGB(SIZE, 0, 0, C1,C2,C3,C4)		\
	WRITE_RGB(SIZE, 1, 1, C1,C2,C3,C4)		\
	WRITE_RGB(SIZE, 2, 0, C1,C2,C3,C4)		\
	WRITE_RGB(SIZE, 3, 1, C1,C2,C3,C4)


/* yuyv/yuyvi output */

#define WRITE_YUYV(ROW,UV_ROW,C1,C2,C3,C4)	\
	x_ptr[(ROW)*x_stride+(C1)] = y_ptr[   (ROW)*y_stride +0];	\
	x_ptr[(ROW)*x_stride+(C2)] = u_ptr[(UV_ROW)*uv_stride+0];	\
	x_ptr[(ROW)*x_stride+(C3)] = y_ptr[   (ROW)*y_stride +1];	\
	x_ptr[(ROW)*x_stride+(C4)] = v_ptr[(UV_ROW)*uv_stride+0];	\

#define YV12_TO_YUYV_ROW(SIZE,C1,C2,C3,C4) 	/* nothing */
#define YV12_TO_YUYV(SIZE,C1,C2,C3,C4)	\
	WRITE_YUYV(0, 0, C1,C2,C3,C4)		\
	WRITE_YUYV(1, 0, C1,C2,C3,C4)

#define YV12_TO_YUYVI_ROW(SIZE,C1,C2,C3,C4) /* nothing */
#define YV12_TO_YUYVI(SIZE,C1,C2,C3,C4)	\
	WRITE_YUYV(0, 0, C1,C2,C3,C4)		\
	WRITE_YUYV(1, 1, C1,C2,C3,C4)		\
	WRITE_YUYV(2, 0, C1,C2,C3,C4)		\
	WRITE_YUYV(3, 1, C1,C2,C3,C4)


//MAKE_COLORSPACE(yv12_to_rgb555_c,  2,2,2, YV12_TO_RGB16,  MK_RGB555, 0,0,0)

//MAKE_COLORSPACE(yv12_to_rgb555i_c, 2,2,4, YV12_TO_RGB16I, MK_RGB555, 0,0,0)

#define WIDTH 256
#define X_STRIDE 512
#define Y_STRIDE 384
#define UV_STRIDE 192

void yv12_to_rgb555_c (uint8_t * x_ptr, uint8_t * y_ptr, uint8_t * u_ptr, uint8_t * v_ptr, int height) 
{
/*	
	for (; height > 0; height -= 2) {
		int r, g, b;
		do {
			int rgb_y;
			int b_u0 = B_U_tab[ u_ptr[0] ];
			int g_uv0 = G_U_tab[ u_ptr[0] ] + G_V_tab[ v_ptr[0] ];
			int r_v0 = R_V_tab[ v_ptr[0] ];

			rgb_y = RGB_Y_tab[ y_ptr[Y_STRIDE*(0) + 0] ];
			b = (rgb_y + b_u0) >> SCALEBITS_DISP;
			g = (rgb_y - g_uv0) >> SCALEBITS_DISP;
			r = (rgb_y + r_v0) >> SCALEBITS_DISP;
			*(uint16_t *) (x_ptr+((0)*X_STRIDE)+0) =
				((((0)>(((MAX_RGB)<(b)?(MAX_RGB):(b)))?(0):(((MAX_RGB)<(b)?(MAX_RGB):(b)))) << 10)) | 
				((((0)>(((MAX_RGB)<(g)?(MAX_RGB):(g)))?(0):(((MAX_RGB)<(g)?(MAX_RGB):(g)))) << 5)) | 
				((((0)>(((MAX_RGB)<(r)?(MAX_RGB):(r)))?(0):(((MAX_RGB)<(r)?(MAX_RGB):(r))))));
			
			rgb_y = RGB_Y_tab[ y_ptr[Y_STRIDE*(0) + 1] ];
			b = (rgb_y + b_u0) >> SCALEBITS_DISP;
			g = (rgb_y - g_uv0) >> SCALEBITS_DISP;
			r = (rgb_y + r_v0) >> SCALEBITS_DISP;
			*(uint16_t *) (x_ptr+((0)*X_STRIDE)+2) =
				((((0)>(((MAX_RGB)<(b)?(MAX_RGB):(b)))?(0):(((MAX_RGB)<(b)?(MAX_RGB):(b)))) << 10)) | 
				((((0)>(((MAX_RGB)<(g)?(MAX_RGB):(g)))?(0):(((MAX_RGB)<(g)?(MAX_RGB):(g)))) << 5)) | 
				((((0)>(((MAX_RGB)<(r)?(MAX_RGB):(r)))?(0):(((MAX_RGB)<(r)?(MAX_RGB):(r))))));
				
			rgb_y = RGB_Y_tab[ y_ptr[Y_STRIDE*(1) + 0] ];
			b = (rgb_y + b_u0) >> SCALEBITS_DISP;
			g = (rgb_y - g_uv0) >> SCALEBITS_DISP;
			r = (rgb_y + r_v0) >> SCALEBITS_DISP;
			*(uint16_t *) (x_ptr+((1)*X_STRIDE)+0) =
				((((0)>(((MAX_RGB)<(b)?(MAX_RGB):(b)))?(0):(((MAX_RGB)<(b)?(MAX_RGB):(b)))) << 10)) | 
				((((0)>(((MAX_RGB)<(g)?(MAX_RGB):(g)))?(0):(((MAX_RGB)<(g)?(MAX_RGB):(g)))) << 5)) | 
				((((0)>(((MAX_RGB)<(r)?(MAX_RGB):(r)))?(0):(((MAX_RGB)<(r)?(MAX_RGB):(r))))));
				
			rgb_y = RGB_Y_tab[ y_ptr[Y_STRIDE*(1) + 1] ];
			b = (rgb_y + b_u0) >> SCALEBITS_DISP;
			g = (rgb_y - g_uv0) >> SCALEBITS_DISP;
			r = (rgb_y + r_v0) >> SCALEBITS_DISP;
			*(uint16_t *) (x_ptr+((1)*X_STRIDE)+2) =
				((((0)>(((MAX_RGB)<(b)?(MAX_RGB):(b)))?(0):(((MAX_RGB)<(b)?(MAX_RGB):(b)))) << 10)) | 
				((((0)>(((MAX_RGB)<(g)?(MAX_RGB):(g)))?(0):(((MAX_RGB)<(g)?(MAX_RGB):(g)))) << 5)) | 
				((((0)>(((MAX_RGB)<(r)?(MAX_RGB):(r)))?(0):(((MAX_RGB)<(r)?(MAX_RGB):(r))))));
			x_ptr += 2*2;
			y_ptr += 2;
			u_ptr += 1;
			v_ptr += 1;
		} while ((int)x_ptr & (X_STRIDE-1));
		x_ptr += 2*X_STRIDE - 2*WIDTH;
		y_ptr += 2*Y_STRIDE - WIDTH;
		u_ptr += UV_STRIDE - (WIDTH / 2);
		v_ptr += UV_STRIDE - (WIDTH / 2);
	}
*/
}



/* yv12 to yv12 copy function */

void
yv12_to_yv12_c(uint8_t * y_dst, uint8_t * u_dst, uint8_t * v_dst,
				int y_dst_stride, int uv_dst_stride,
				uint8_t * y_src, uint8_t * u_src, uint8_t * v_src,
				int y_src_stride, int uv_src_stride,
				int width, int height, int vflip)
{
	int width2 = width / 2;
	int height2 = height / 2;
	int y;

	if (vflip) {
		y_src += (height - 1) * y_src_stride;
		u_src += (height2 - 1) * uv_src_stride;
		v_src += (height2 - 1) * uv_src_stride;
		y_src_stride = -y_src_stride;
		uv_src_stride = -uv_src_stride;
	}

	for (y = height; y; y--) {
		memcpy(y_dst, y_src, width);
		y_src += y_src_stride;
		y_dst += y_dst_stride;
	}

	for (y = height2; y; y--) {
		memcpy(u_dst, u_src, width2);
		u_src += uv_src_stride;
		u_dst += uv_dst_stride;
	}

	for (y = height2; y; y--) {
		memcpy(v_dst, v_src, width2);
		v_src += uv_src_stride;
		v_dst += uv_dst_stride;
	}
}



/* initialize rgb lookup tables */
/*
void
colorspace_init(void)
{
	int32_t i;
	int max_val;
	int min_val;

	for (i = 0; i < 256; i++) {
		RGB_Y_tab[i] = FIX_OUT(RGB_Y_OUT) * (i - Y_ADD_OUT);
		B_U_tab[i] = FIX_OUT(B_U_OUT) * (i - U_ADD_OUT);
		G_U_tab[i] = FIX_OUT(G_U_OUT) * (i - U_ADD_OUT);
		G_V_tab[i] = FIX_OUT(G_V_OUT) * (i - V_ADD_OUT);
		R_V_tab[i] = FIX_OUT(R_V_OUT) * (i - V_ADD_OUT);
	}

	max_val = 0; min_val = 255;
	for ( i = 0; i < 256; i++) {
		max_val = max_val > RGB_Y_tab[i] ? max_val : RGB_Y_tab[i];
		min_val = min_val < RGB_Y_tab[i] ? min_val : RGB_Y_tab[i];
	}
	iprintf ("RGB_Y_tab: %d to %d\n", min_val, max_val);

	max_val = 0; min_val = 255;
	for ( i = 0; i < 256; i++) {
		max_val = max_val > B_U_tab[i] ? max_val : B_U_tab[i];
		min_val = min_val < B_U_tab[i] ? min_val : B_U_tab[i];
	}
	iprintf ("B_U_tab: %d to %d\n", min_val, max_val);

	max_val = 0; min_val = 255;
	for ( i = 0; i < 256; i++) {
		max_val = max_val > G_U_tab[i] ? max_val : G_U_tab[i];
		min_val = min_val < G_U_tab[i] ? min_val : G_U_tab[i];
	}
	iprintf ("G_U_tab: %d to %d\n", min_val, max_val);

	max_val = 0; min_val = 255;
	for ( i = 0; i < 256; i++) {
		max_val = max_val > G_V_tab[i] ? max_val : G_V_tab[i];
		min_val = min_val < G_V_tab[i] ? min_val : G_V_tab[i];
	}
	iprintf ("G_V_tab: %d to %d\n", min_val, max_val);

	max_val = 0; min_val = 255;
	for ( i = 0; i < 256; i++) {
		max_val = max_val > R_V_tab[i] ? max_val : R_V_tab[i];
		min_val = min_val < R_V_tab[i] ? min_val : R_V_tab[i];
	}
	iprintf ("R_V_tab: %d to %d\n", min_val, max_val);
//	while(1);
}
*/
