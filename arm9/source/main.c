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
#include <errno.h>
#include <fat.h>
#include <malloc.h>
#include <nds.h>
#include <nds/arm9/console.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "controls.h"
#include "ipc9.h"
#include "player.h"
#include "video.h"

const char *DEFAULTFILE = "fat:/tuna-vids.avi";

int main(int argc, const char* argv[])
{
    FILE* aviFile = NULL;
    const char* aviFileName;

    // Exception handling, just in case
    defaultExceptionHandler();

    // Give 128KiB of VRAM to the ARM7
    vramSetBankD(VRAM_D_ARM7_0x06000000);

    consoleSetup();

    iprintf("\n");
    iprintf("Tuna-viDS v1.2\n");
    iprintf("\n");
    iprintf("AVI + Xvid + MP3 player by\n");
    iprintf("Michael Chisholm (Chishm)\n");
    iprintf("\n");
    iprintf("See documentation for a \n");
    iprintf("full list of credits.\n");

    powerOn(POWER_ALL_2D);
    lcdMainOnTop();

    // File set up
    if (!fatInitDefault()) {
        iprintf("Failed to init FAT\n");
        return -1;
    }

    // Video set up
    vramSetBankA(VRAM_A_LCD);
    vramSetBankB(VRAM_B_LCD);
    vramSetBankC(VRAM_C_LCD);

    /* Most setup is done in libnds's initSystem() function, like:
     * irqInit()
     * irqEnable(IRQ_VBLANK)
     * fifoInit()
     */

    // VBLANK interrupt setup.
    irqSet(IRQ_VBLANK, vidBuf_VblankHandler);

    // Communication with ARM7
    if (!ipcInit()) {
        iprintf("Failed to init IPC\n");
        return -1;
    }

    // Get file name
    if (argc >= 2) {
        aviFileName = argv[1];
    } else {
        aviFileName = DEFAULTFILE;
    }

    // Load Video
    aviFile = fopen(aviFileName, "rb");
    if (!aviFile) {
        iprintf("Error opening AVI file\n%s\n%s\n", aviFileName, strerror(errno));
        return -1;
    }

    // xvid play
    play_movie(aviFile);

    if (fclose(aviFile) != 0) {
        iprintf("Error closing AVI file\n%s\n", strerror(errno));
        return -1;
    }

    iprintf("Exiting\n");

    ipcSend_Exit();

    return 0;
}
