/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Native API implementation  -
 *
 *  Copyright(C) 2001-2004 Peter Ross <pross@xvid.org>
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
 * $Id: xvid.c,v 1.65.2.5 2007/06/27 18:57:42 Isibaar Exp $
 *
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "xvid.h"
#include "decoder.h"
#include "bitstream/cbp.h"
#include "dct/idct.h"
#include "image/colorspace.h"
#include "image/interpolate8x8.h"
#include "utils/mem_transfer.h"
#include "utils/mbfunctions.h"
#include "quant/quant.h"
#include "motion/motion.h"
#include "motion/sad.h"
#include "utils/emms.h"
#include "utils/timer.h"
#include "bitstream/mbcoding.h"
#include "image/qpel.h"
#include "image/postprocessing.h"

#if defined(_DEBUG)
unsigned int xvid_debug = 0; /* xvid debug mask */
#endif

#if (defined(ARCH_IS_IA32) || defined(ARCH_IS_X86_64)) && defined(_MSC_VER)
#	include <windows.h>
#elif defined(ARCH_IS_IA32) || defined(ARCH_IS_X86_64) || defined(ARCH_IS_PPC)
#	include <signal.h>
#	include <setjmp.h>

	static jmp_buf mark;

	static void
	sigill_handler(int signal)
	{
	   longjmp(mark, 1);
	}
#endif


/*
 * Calls the funcptr, and returns whether SIGILL (illegal instruction) was
 * signalled
 *
 * Return values:
 *  -1 : could not determine
 *   0 : SIGILL was *not* signalled
 *   1 : SIGILL was signalled
 */
#if (defined(ARCH_IS_IA32) || defined(ARCH_IS_X86_64)) && defined(_MSC_VER)
static int
sigill_check(void (*func)())
{
	_try {
		func();
	} _except(EXCEPTION_EXECUTE_HANDLER) {

		if (_exception_code() == STATUS_ILLEGAL_INSTRUCTION)
			return(1);
	}
	return(0);
}
#elif defined(ARCH_IS_IA32) || defined(ARCH_IS_X86_64) || defined(ARCH_IS_PPC)
static int
sigill_check(void (*func)())
{
    void *old_handler;
    int jmpret;

    /* Set our SIGILL handler */
    old_handler = signal(SIGILL, sigill_handler);

    /* Check for error */
    if (old_handler == SIG_ERR) {
        return(-1);
    }

    /* Save stack context, so if func triggers a SIGILL, we can still roll
	 * back to a valid CPU state */
	jmpret = setjmp(mark);

	/* If setjmp returned directly, then its returned value is 0, and we still
	 * have to test the passed func. Otherwise it means the stack context has
	 * been restored by a longjmp() call, which in our case happens only in the
	 * signal handler */
    if (jmpret == 0) {
        func();
    }

    /* Restore old signal handler */
    signal(SIGILL, old_handler);

    return(jmpret);
}
#endif


/* detect cpu flags  */
static unsigned int
detect_cpu_flags(void)
{
	/* enable native assembly optimizations by default */
	unsigned int cpu_flags = XVID_CPU_ASM;

#if defined(ARCH_IS_IA32) || defined(ARCH_IS_X86_64)
	cpu_flags |= check_cpu_features();
	if ((cpu_flags & XVID_CPU_SSE) && sigill_check(sse_os_trigger))
		cpu_flags &= ~XVID_CPU_SSE;

	if ((cpu_flags & XVID_CPU_SSE2) && sigill_check(sse2_os_trigger))
		cpu_flags &= ~XVID_CPU_SSE2;
#endif

#if defined(ARCH_IS_PPC)
	if (!sigill_check(altivec_trigger))
		cpu_flags |= XVID_CPU_ALTIVEC;
#endif

	return cpu_flags;
}


/*****************************************************************************
 * XviD Init Entry point
 *
 * Well this function initialize all internal function pointers according
 * to the CPU features forced by the library client or autodetected (depending
 * on the XVID_CPU_FORCE flag). It also initializes vlc coding tables and all
 * image colorspace transformation tables.
 *
 * Returned value : XVID_ERR_OK
 *                  + API_VERSION in the input XVID_INIT_PARAM structure
 *                  + core build  "   "    "       "               "
 *
 ****************************************************************************/
static
int xvid_gbl_init(xvid_gbl_init_t * init)
{
	unsigned int cpu_flags;

	if (XVID_VERSION_MAJOR(init->version) != 1) /* v1.x.x */
		return XVID_ERR_VERSION;

	cpu_flags = (init->cpu_flags & XVID_CPU_FORCE) ? init->cpu_flags : detect_cpu_flags();

	/* Initialize the function pointers */
	idct_int32_init();
	init_vlc_tables();

	/* Fixed Point Forward/Inverse DCT transformations */
//	fdct = fdct_int32;
//	idct = idct_int32;
//	idct = simple_idct_armv5te;
//	idct = simple_idct_ARM;

	/* Only needed on PPC Altivec archs */
	sadInit = NULL;

	/* Restore FPU context : emms_c is a nop functions */
	emms = emms_c;

	/* Qpel stuff */
	xvid_QP_Funcs = &xvid_QP_Funcs_C;
	xvid_QP_Add_Funcs = &xvid_QP_Add_Funcs_C;
	xvid_Init_QP();

	/* Quantization functions */
	quant_h263_intra   = quant_h263_intra_c;
	quant_h263_inter   = quant_h263_inter_c;
	dequant_h263_intra = dequant_h263_intra_c;
	dequant_h263_inter = dequant_h263_inter_c;

	quant_mpeg_intra   = quant_mpeg_intra_c;
	quant_mpeg_inter   = quant_mpeg_inter_c;
	dequant_mpeg_intra = dequant_mpeg_intra_c;
	dequant_mpeg_inter = dequant_mpeg_inter_c;

	/* Block transfer related functions */

//extern void transfer_16to8add_arm (uint8_t * const dst, const int16_t * const src, uint32_t stride);
extern void transfer8x8_copy_arm (uint8_t * const dst, const uint8_t * const src, const uint32_t stride);
extern void transfer_16to8copy_arm (uint8_t * const dst, const int16_t * const src, uint32_t stride);
	
	transfer_8to16copy = transfer_8to16copy_c;
	transfer_16to8copy = transfer_16to8copy_c;
	transfer_8to16sub  = transfer_8to16sub_c;
	transfer_8to16subro  = transfer_8to16subro_c;
	transfer_8to16sub2 = transfer_8to16sub2_c;
	transfer_8to16sub2ro = transfer_8to16sub2ro_c;
	transfer_16to8add  = transfer_16to8add_c;
	transfer8x8_copy   = transfer8x8_copy_arm;
	transfer8x4_copy   = transfer8x4_copy_c;

	/* Interlacing functions */
//	MBFieldTest = MBFieldTest_c;

	/* Image interpolation related functions */
	interpolate8x8_halfpel_h  = interpolate8x8_halfpel_h_c;
	interpolate8x8_halfpel_v  = interpolate8x8_halfpel_v_c;
	interpolate8x8_halfpel_hv = interpolate8x8_halfpel_hv_c;

	interpolate8x4_halfpel_h  = interpolate8x4_halfpel_h_c;
	interpolate8x4_halfpel_v  = interpolate8x4_halfpel_v_c;
	interpolate8x4_halfpel_hv = interpolate8x4_halfpel_hv_c;

	interpolate8x8_halfpel_add = interpolate8x8_halfpel_add_c;
	interpolate8x8_halfpel_h_add = interpolate8x8_halfpel_h_add_c;
	interpolate8x8_halfpel_v_add = interpolate8x8_halfpel_v_add_c;
	interpolate8x8_halfpel_hv_add = interpolate8x8_halfpel_hv_add_c;

	interpolate16x16_lowpass_h = interpolate16x16_lowpass_h_c;
	interpolate16x16_lowpass_v = interpolate16x16_lowpass_v_c;
	interpolate16x16_lowpass_hv = interpolate16x16_lowpass_hv_c;

	interpolate8x8_lowpass_h = interpolate8x8_lowpass_h_c;
	interpolate8x8_lowpass_v = interpolate8x8_lowpass_v_c;
	interpolate8x8_lowpass_hv = interpolate8x8_lowpass_hv_c;

	interpolate8x8_6tap_lowpass_h = interpolate8x8_6tap_lowpass_h_c;
	interpolate8x8_6tap_lowpass_v = interpolate8x8_6tap_lowpass_v_c;

	interpolate8x8_avg2 = interpolate8x8_avg2_c;
	interpolate8x8_avg4 = interpolate8x8_avg4_c;

	/* postprocessing */
	image_brightness = image_brightness_c;


	/* Functions used in motion estimation algorithms */
	calc_cbp   = calc_cbp_c;
	sad16      = sad16_c;
	sad8       = sad8_c;
	sad16bi    = sad16bi_c;
	sad8bi     = sad8bi_c;
	dev16      = dev16_c;
	sad16v	   = sad16v_c;
	sse8_16bit = sse8_16bit_c;
	sse8_8bit  = sse8_8bit_c;



#if defined(_DEBUG)
    xvid_debug = init->debug;
#endif

    return(0);
}


static int
xvid_gbl_info(xvid_gbl_info_t * info)
{
	if (XVID_VERSION_MAJOR(info->version) != 1) /* v1.x.x */
		return XVID_ERR_VERSION;

	info->actual_version = XVID_VERSION;
	info->build = "xvid-1.1.3";
	info->cpu_flags = detect_cpu_flags();

#if defined(_SMP) && defined(WIN32)
	info->num_threads = pthread_num_processors_np();;
#else
	info->num_threads = 0;
#endif

	return 0;
}


static int
xvid_gbl_convert(xvid_gbl_convert_t* convert)
{
	int width;
	int height;
	int width2;
	int height2;
	IMAGE img;

	if (XVID_VERSION_MAJOR(convert->version) != 1)   /* v1.x.x */
	      return XVID_ERR_VERSION;

#if 0
	const int flip1 = (convert->input.colorspace & XVID_CSP_VFLIP) ^ (convert->output.colorspace & XVID_CSP_VFLIP);
#endif
	width = convert->width;
	height = convert->height;
	width2 = convert->width/2;
	height2 = convert->height/2;

	switch (convert->input.csp & ~XVID_CSP_VFLIP)
	{
		case XVID_CSP_YV12 :
			img.y = convert->input.plane[0];
			img.v = (uint8_t*)convert->input.plane[0] + convert->input.stride[0]*height;
			img.u = (uint8_t*)convert->input.plane[0] + convert->input.stride[0]*height + (convert->input.stride[0]/2)*height2;
			image_output(&img, width, height, width,
						(uint8_t**)convert->output.plane, convert->output.stride,
						convert->output.csp, convert->interlacing);
			break;

		default :
			return XVID_ERR_FORMAT;
	}


	emms();
	return 0;
}

/*****************************************************************************
 * XviD Global Entry point
 *
 * Well this function initialize all internal function pointers according
 * to the CPU features forced by the library client or autodetected (depending
 * on the XVID_CPU_FORCE flag). It also initializes vlc coding tables and all
 * image colorspace transformation tables.
 *
 ****************************************************************************/


int
xvid_global(void *handle,
		  int opt,
		  void *param1,
		  void *param2)
{
	switch(opt)
	{
		case XVID_GBL_INIT :
			return xvid_gbl_init((xvid_gbl_init_t*)param1);

        case XVID_GBL_INFO :
            return xvid_gbl_info((xvid_gbl_info_t*)param1);

		case XVID_GBL_CONVERT :
			return xvid_gbl_convert((xvid_gbl_convert_t*)param1);

		default :
			return XVID_ERR_FAIL;
	}
}

/*****************************************************************************
 * XviD Native decoder entry point
 *
 * This function is just a wrapper to all the option cases.
 *
 * Returned values : XVID_ERR_FAIL when opt is invalid
 *                   else returns the wrapped function result
 *
 ****************************************************************************/

int
xvid_decore(void *handle,
			int opt,
			void *param1,
			void *param2)
{
	switch (opt) {
	case XVID_DEC_CREATE:
		return decoder_create((xvid_dec_create_t *) param1);

	case XVID_DEC_DESTROY:
		return decoder_destroy((DECODER *) handle);

	case XVID_DEC_DECODE:
		return decoder_decode((DECODER *) handle, (xvid_dec_frame_t *) param1, (xvid_dec_stats_t*) param2);

	default:
		return XVID_ERR_FAIL;
	}
}
