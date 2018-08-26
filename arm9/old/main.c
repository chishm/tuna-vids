/*---------------------------------------------------------------------------------
	$Id: template.c,v 1.4 2005/09/17 23:15:13 wntrmute Exp $

	Basic Hello World

	$Log: template.c,v $
	Revision 1.4  2005/09/17 23:15:13  wntrmute
	corrected iprintAt in templates
	
	Revision 1.3  2005/09/05 00:32:20  wntrmute
	removed references to IPC struct
	replaced with API functions
	
	Revision 1.2  2005/08/31 01:24:21  wntrmute
	updated for new stdio support

	Revision 1.1  2005/08/03 06:29:56  wntrmute
	added templates


---------------------------------------------------------------------------------*/
#include "nds.h"
#include <nds/arm9/console.h> //basic print funcionality
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <fat.h>
#include <errno.h>
#include "xvid/xvid.h"


static int XDIM = 0;
static int YDIM = 0;
static char *ARG_INPUTFILE = "fat:/s.m4v";
static int CSP = XVID_CSP_RGB555;
static int BPP = 2;

static void *dec_handle = NULL;

#define VIDEO_BUFFER_SIZE	(1*1024*1024)
#define VIDEO_CHUNK_SIZE	(32*1024)

static const int display_buffer_bytes = 0;

#define MIN_USEFUL_BYTES 1


// The following is used for page flipping / frame buffer
typedef struct {
	void*	buffer;
	int		videoMode;
	bool	readyForDisplay;
} VID_PAGE;

#define NUM_VIDEO_PAGES 3

VID_PAGE videoPages[NUM_VIDEO_PAGES] = {
	{(void*)0x06800000, (0<<18) | 0x00020000, false},
	{(void*)0x06820000, (1<<18) | 0x00020000, false},
	{(void*)0x06840000, (2<<18) | 0x00020000, false},
//	{(void*)0x06860000, (3<<18) | 0x00020000, false}
};

int currentDisplayPage = 0;
int lastDecodePage = 3;

static int vblanks = 0;
void vblankHandler (void) {
	static int waitFrame = 0;
	vblanks++;
	waitFrame++;
	if (waitFrame >= 6) {
		// Move onto next decoded frame
	}
}

void* pageFlip (void) {
	static int curPage = 0;
	curPage ^= 1;
	
	if (curPage == 0) {
		videoSetMode(MODE_FB0);
		return (void*)0x06820000;
	} else {
		videoSetMode(MODE_FB1);
		return (void*)0x06800000;
	}	
}

/*****************************************************************************
 * Routines for decoding: init decoder, use, and stop decoder
 ****************************************************************************/

/* init decoder before first run */
static int dec_init()
{
	int ret;

	xvid_gbl_init_t   xvid_gbl_init;
	xvid_dec_create_t xvid_dec_create;

	/* Reset the structure with zeros */
	memset(&xvid_gbl_init, 0, sizeof(xvid_gbl_init_t));
	memset(&xvid_dec_create, 0, sizeof(xvid_dec_create_t));

	/*------------------------------------------------------------------------
	 * XviD core initialization
	 *----------------------------------------------------------------------*/

	/* Version */
	xvid_gbl_init.version = XVID_VERSION;

	/* Assembly setting */
	xvid_gbl_init.cpu_flags = XVID_CPU_FORCE;

	xvid_gbl_init.debug = 0;

	xvid_global(NULL, 0, &xvid_gbl_init, NULL);

	/*------------------------------------------------------------------------
	 * XviD encoder initialization
	 *----------------------------------------------------------------------*/

	/* Version */
	xvid_dec_create.version = XVID_VERSION;

	/*
	 * Image dimensions -- set to 0, xvidcore will resize when ever it is
	 * needed
	 */
	xvid_dec_create.width = 0;
	xvid_dec_create.height = 0;

	ret = xvid_decore(NULL, XVID_DEC_CREATE, &xvid_dec_create, NULL);

	dec_handle = xvid_dec_create.handle;

	return(ret);
}

/* decode one frame  */
static int dec_main(unsigned char *istream,  unsigned char *ostream, int istream_size, xvid_dec_stats_t *xvid_dec_stats) {
	int ret;

	xvid_dec_frame_t xvid_dec_frame;

	/* Reset all structures */
	memset(&xvid_dec_frame, 0, sizeof(xvid_dec_frame_t));
	memset(xvid_dec_stats, 0, sizeof(xvid_dec_stats_t));

	/* Set version */
	xvid_dec_frame.version = XVID_VERSION;
	xvid_dec_stats->version = XVID_VERSION;

	/* No general flags to set */
	xvid_dec_frame.general          = 0;

	/* Input stream */
	xvid_dec_frame.bitstream        = istream;
	xvid_dec_frame.length           = istream_size;

	/* Output frame structure */
	xvid_dec_frame.output.plane[0]  = ostream;
	xvid_dec_frame.output.stride[0] = SCREEN_WIDTH*BPP;
	xvid_dec_frame.output.csp = CSP;

	ret = xvid_decore(dec_handle, XVID_DEC_DECODE, &xvid_dec_frame, xvid_dec_stats);

	return(ret);
}

/* close decoder to release resources */
static int dec_stop() {
	return xvid_decore(dec_handle, XVID_DEC_DESTROY, NULL, NULL);
}

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	// Console set up
	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
	vramSetBankH(VRAM_H_SUB_BG);
	SUB_BG0_CR = BG_MAP_BASE(15);
	BG_PALETTE_SUB[0]=0;   
	BG_PALETTE_SUB[255]=RGB15(31,31,31); 
	consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(15), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);

	iprintf("Console running\n");
	
	// Exception handling, just in case
	defaultExceptionHandler();
	
	// File set up
	if (fatInitDefault()) {
		iprintf ("FAT running\n");
	} else {
		iprintf ("Failed to init FAT\n");
		return -1;
	}
	
	// Video set up
	vramSetBankA(VRAM_A_LCD);
	vramSetBankB(VRAM_B_LCD);
	vramSetBankC(VRAM_C_LCD);
	vramSetBankE(VRAM_E_LCD);
	vramSetBankF(VRAM_F_LCD);
	vramSetBankG(VRAM_G_LCD);
	unsigned char* out_buffer = pageFlip();
	
	// Give 128KiB of VRAM to the ARM7
	vramSetBankD(VRAM_D_ARM7_0x06000000);
	
	// Interrupt set up
	irqInit();
	irqEnable(IRQ_VBLANK);
	irqSet (IRQ_VBLANK, vblankHandler);

	// Main Xvid decoder
	
	unsigned char *mp4_buffer = NULL;
	unsigned char *mp4_ptr    = NULL;
	int useful_bytes;
	int chunk;
	xvid_dec_stats_t xvid_dec_stats;
	
	double totaldectime;
  
	long totalsize;
	int status;
  
	FILE *in_file;
	int filenr;

	iprintf("xvid_decraw - raw mpeg4 bitstream decoder ");
	iprintf("written by\nChristoph Lampert 2002-2003\n");
	iprintf("Adapted by Michael Chisholm (Chishm)\n");

	in_file = fopen(ARG_INPUTFILE, "rb");
	if (in_file == NULL) {
		iprintf("Error opening input file\n%s\n%s\n", ARG_INPUTFILE, strerror(errno));
		return(-1);
	}

	/* Memory for encoded mp4 stream */
	mp4_buffer = (unsigned char *) malloc(VIDEO_BUFFER_SIZE);
	mp4_ptr = mp4_buffer;
	if (!mp4_buffer) {
		iprintf ("Couldn't allocate file buffer\n");
		goto free_all_memory;
	}
   
/*****************************************************************************
 *        XviD PART  Start
 ****************************************************************************/

	status = dec_init();
	if (status) {
		iprintf("Decore INIT problem, return value %d\n", status);
		goto release_all;
	}


/*****************************************************************************
 *	                         Main loop
 ****************************************************************************/

	/* Fill the buffer */
	useful_bytes = fread(mp4_buffer, 1, VIDEO_BUFFER_SIZE, in_file);

	totaldectime = 0;
	totalsize = 0;
	filenr = 0;
	mp4_ptr = mp4_buffer;
	chunk = 0;
	
	vblanks = 0;
	
	do {
		int used_bytes = 0;

		/*
		 * If the buffer is half empty or there are no more bytes in it
		 * then fill it.
		 */
		if (mp4_ptr > mp4_buffer + VIDEO_BUFFER_SIZE/2) {
			int already_in_buffer = (mp4_buffer + VIDEO_BUFFER_SIZE - mp4_ptr);

			/* Move data if needed */
			if (already_in_buffer > 0)
				memcpy(mp4_buffer, mp4_ptr, already_in_buffer);

			/* Update mp4_ptr */
			mp4_ptr = mp4_buffer; 

			/* read new data */
            if(!feof(in_file)) {
				useful_bytes += fread(mp4_buffer + already_in_buffer,
									  1, VIDEO_BUFFER_SIZE - already_in_buffer,
									  in_file);
			}
		}


		/* This loop is needed to handle VOL/NVOP reading */
		do {

			/* Decode frame */
			used_bytes = dec_main(mp4_ptr, out_buffer, useful_bytes, &xvid_dec_stats);

			/* Resize image buffer if needed */
			if(xvid_dec_stats.type == XVID_TYPE_VOL) {
				/* Copy new width and new height from the vol structure */
				XDIM = xvid_dec_stats.data.vol.width;
				YDIM = xvid_dec_stats.data.vol.height;

				if (XDIM != SCREEN_WIDTH) {
					iprintf ("Video is not 256px wide!\n");
				}
				
				if (YDIM > SCREEN_HEIGHT) {
					iprintf ("Video too tall!\n");
				}
				
//				iprintf("Resized frame buffer to %dx%d\n", XDIM, YDIM);
			}

			/* Update buffer pointers */
			if(used_bytes > 0) {
				mp4_ptr += used_bytes;
				useful_bytes -= used_bytes;

				/* Total size */
				totalsize += used_bytes;
			}

//			if (display_buffer_bytes) {
//				iprintf("Data chunk %d: %d bytes consumed, %d bytes in buffer\n", chunk++, used_bytes, useful_bytes);
//			}
		} while (xvid_dec_stats.type <= 0 && useful_bytes > MIN_USEFUL_BYTES);

		/* Check if there is a negative number of useful bytes left in buffer
		 * This means we went too far */
        if(useful_bytes < 0)
            break;
		
//		if (!display_buffer_bytes) {
//			printf("Frame %5d: type = %s, dectime(ms) =%6.1f, length(bytes) =%7d\n",
//					filenr, type2str(xvid_dec_stats.type), dectime, used_bytes);
//		}
		
		// Page flip
		out_buffer = pageFlip();

		filenr++;
		int fps = filenr * 60 * 10 / vblanks;	// Multiplied by 10 for decimal point
		iprintf ("FPS: %2d.%1d\n", fps / 10, fps % 10);
//		if (vblanks >= 60) {
//			iprintf ("FPS: %d  (%d)\n", filenr, vblanks);
//			vblanks = 0;
//			filenr = 0;
//		}

	} while (useful_bytes>MIN_USEFUL_BYTES || !feof(in_file));

	useful_bytes = 0; /* Empty buffer */

/*****************************************************************************
 *     Flush decoder buffers
 ****************************************************************************/

	do {
		int used_bytes;

        do {
		    used_bytes = dec_main(NULL, out_buffer, -1, &xvid_dec_stats);
//			if (display_buffer_bytes) {
//				printf("Data chunk %d: %d bytes consumed, %d bytes in buffer\n", chunk++, used_bytes, useful_bytes);
//			}
        } while(used_bytes>=0 && xvid_dec_stats.type <= 0);

        if (used_bytes < 0) {   /* XVID_ERR_END */
            break;
        }

		// Page flip
		out_buffer = pageFlip();

		filenr++;

	}while(1);
	
/*****************************************************************************
 *      XviD PART  Stop
 ****************************************************************************/

 release_all:
  	if (dec_handle) {
	  	status = dec_stop();
		if (status) {
			iprintf("decore RELEASE problem return value %d\n", status);
		}
	}

 free_all_memory:
	free(mp4_buffer);
	
	iprintf ("Finished\n");

	return 0;
}

