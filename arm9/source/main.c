/*---------------------------------------------------------------------------------
  Tuna-viDS -- AVI + Xvid + MP3 player for Nintendo DS

  Copyright(C) 2007-2008 Michael "Chishm" Chisholm

  This program is free software ; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation ; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY ; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program ; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
---------------------------------------------------------------------------------*/
#include <nds.h>
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

int main(int argc, const char* argv[])
{
	FILE* aviFile = NULL;
	const char* aviFileName;


	// Exception handling, just in case
	defaultExceptionHandler();

	// Give 128KiB of VRAM to the ARM7
	vramSetBankD(VRAM_D_ARM7_0x06000000);

	// Console set up
	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
	vramSetBankH(VRAM_H_SUB_BG);
	REG_BG0CNT_SUB = BG_MAP_BASE(15);
	BG_PALETTE_SUB[0]=0;
	BG_PALETTE_SUB[255]=RGB15(31,31,31);

	consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);

	iprintf("\n");
	iprintf("Tuna-viDS v1.1\n");
	iprintf("\n");
	iprintf("AVI + Xvid + MP3 player by\n");
	iprintf("Michael Chisholm (Chishm)\n");
	iprintf("\n");
	iprintf("See documentation for a \n");
	iprintf("full list of credits.\n");

	powerOn(POWER_ALL_2D);
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
		return -1;
	}

	// xvid play
	play_movie (aviFile);

	return 0;
}
