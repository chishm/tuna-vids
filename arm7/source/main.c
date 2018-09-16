/*---------------------------------------------------------------------------------

	default ARM7 core

	Copyright (C) 2005
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/
#include <nds.h>
#include "../../common/soundcommon.h"
#include "sound7.h"
#include "ipc7.h"

#define REG_VRAMSTAT (*(vu8*)0x04000240)
#define VRAM_START ((char*)0x06000000)
#define VRAM_END ((char*)0x06020000)
extern char *fake_heap_start;
extern char *fake_heap_end;


//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------
	// Not using Wifi
	//Wifi_Update();
}


//---------------------------------------------------------------------------------
void VcountHandler() {
//---------------------------------------------------------------------------------
	inputGetAndSend();
}

volatile bool exitflag = false;

//---------------------------------------------------------------------------------
void powerButtonCB() {
//---------------------------------------------------------------------------------
	exitflag = true;
}

/* Use VRAM as heap for malloc. This must be called before any malloc is done. */
static void initVramHeap(void)
{
	// Wait for VRAM bank D to become available
	while ((REG_VRAMSTAT & 0x02) == 0);

	// Clear VRAM
	memset (VRAM_START, 0, VRAM_END - VRAM_START);

	// Use VRAM as heap
	fake_heap_start = VRAM_START;
	fake_heap_end = VRAM_END;
}

void toggleBottomLight (void) {
	// Toggle lower screen's backlight
	u16 oldIME = REG_IME;
	REG_IME = 0;
	u8 oldPowerReg = readPowerManagement (PM_CONTROL_REG);
	writePowerManagement (PM_CONTROL_REG, oldPowerReg ^ PM_BACKLIGHT_BOTTOM);
	REG_IME = oldIME;
}

//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------

	// clear sound registers
	dmaFillWords(0, (void*)0x04000400, 0x100);

	REG_SOUNDCNT |= SOUND_ENABLE;
	writePowerManagement(PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE ) | PM_SOUND_AMP );
	powerOn(POWER_SOUND);

	readUserSettings();
	ledBlink(0);

	irqInit();
	// Start the RTC tracking IRQ
	initClockIRQ();
	// Setup FIFO on ARM7. This will sync with the ARM9.
	fifoInit();

	initVramHeap();

	// Not using MaxMOD
	//mmInstall(FIFO_MAXMOD);

	SetYtrigger(80);

	// Not using Wifi or normal libnds sound
	//installWifiFIFO();
	//installSoundFIFO();

	installSystemFIFO();

	irqSet(IRQ_VCOUNT, VcountHandler);
	irqSet(IRQ_VBLANK, VblankHandler);

	// IRQ_NETWORK is used by the RTC, other IRQs used above
	irqEnable(IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);

	setPowerButtonCB(powerButtonCB);

	// Communication with ARM9
	ipcInit();

	// Provide MP3 playback
	SoundInit();

	// Keep the ARM7 mostly idle
	while (!exitflag) {
		if ( 0 == (REG_KEYINPUT & (KEY_SELECT | KEY_START | KEY_L | KEY_R))) {
			exitflag = true;
		}
		SoundLoopStep();
		swiWaitForVBlank();
	}
	return 0;
}
