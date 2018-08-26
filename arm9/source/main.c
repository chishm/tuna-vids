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

#include "ipc9.h"
#include "video.h"
#include "player.h"

const char *DEFAULTFILE = "fat:/tuna-vids.avi";

//---------------------------------------------------------------------------------
int main(int argc, const char* argv[]) {
//---------------------------------------------------------------------------------
	FILE* aviFile = NULL;
	const char* aviFileName;
	
	
	// Exception handling, just in case
	defaultExceptionHandler();
	
	// Give 128KiB of VRAM to the ARM7
	vramSetBankD(VRAM_D_ARM7_0x06000000);
	
	// Console set up
	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
	vramSetBankH(VRAM_H_SUB_BG);
	SUB_BG0_CR = BG_MAP_BASE(15);
	BG_PALETTE_SUB[0]=0;   
	BG_PALETTE_SUB[255]=RGB15(31,31,31); 
	consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(15), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);


	iprintf("\n");
	iprintf("tuna-viDS\n");
	iprintf("\n");
	iprintf("xvid player by\n");
	iprintf("Michael Chisholm (Chishm) 2007\n");
	iprintf("Based on xvid_decraw written by\n");
	iprintf("Christoph Lampert 2002-2003\n");
	iprintf("MP3 playback code contributed by");
	iprintf("ThomasS, LiraNuna\n");
	iprintf("strager & DarkEz\n");
	iprintf("Skin by Ferrie\n");
	
	powerSET (POWER_ALL_2D);
	lcdMainOnTop ();

	// File set up
	if (!fatInitDefault()){
		iprintf ("Failed to init FAT\n");
		return -1;
	}
	
	// Video set up
	vramSetBankA(VRAM_A_LCD);
	vramSetBankB(VRAM_B_LCD);
	vramSetBankC(VRAM_C_LCD);
	
	// Interrupt set up
	irqInit();
	irqEnable(IRQ_VBLANK);
	irqSet (IRQ_VBLANK, vidBuf_VblankHandler);

	// Communication with ARM7
	ipcInit();

	// Get file name
	if (argc >= 2) {
		aviFileName = argv[1];
	} else {
		aviFileName = DEFAULTFILE;
	}
	
	// Load Video
	aviFile = fopen(aviFileName, "rb");
	if (!aviFile) {
		iprintf("Error opening input file\n%s\n%s\n", aviFileName, strerror(errno));
		return(-1);
	}

	// xvid play
	play_movie (aviFile);
	
	return 0;
}

