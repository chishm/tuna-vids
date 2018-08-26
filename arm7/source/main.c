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

#define VRAM_START ((char*)0x06000000)
#define VRAM_END ((char*)0x06020000)
extern char *fake_heap_start;
extern char *fake_heap_end;

touchPosition first,tempPos;

//---------------------------------------------------------------------------------
void VcountHandler() {
//---------------------------------------------------------------------------------
	static int lastbut = -1;
	
	uint16 but=0, x=0, y=0, xpx=0, ypx=0, z1=0, z2=0;

	but = REG_KEYXY;

	if (!( (but ^ lastbut) & (1<<6))) {
 
		tempPos = touchReadXY();

		if ( tempPos.x == 0 || tempPos.y == 0 ) {
			but |= (1 <<6);
			lastbut = but;
		} else {
			x = tempPos.x;
			y = tempPos.y;
			xpx = tempPos.px;
			ypx = tempPos.py;
			z1 = tempPos.z1;
			z2 = tempPos.z2;
		}
		
	} else {
		lastbut = but;
		but |= (1 <<6);
	}

	IPC->touchX			= x;
	IPC->touchY			= y;
	IPC->touchXpx		= xpx;
	IPC->touchYpx		= ypx;
	IPC->touchZ1		= z1;
	IPC->touchZ2		= z2;
	IPC->buttons		= but;

}

//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------

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
int main(int argc, char ** argv) {
//---------------------------------------------------------------------------------
	// Wait for VRAM to become available
	while ((*(vuint8*)0x04000240 & 0x02) == 0);
	// Clear VRAM
	memset (VRAM_START, 0, VRAM_END - VRAM_START);
	
	// Use VRAM as heap
	fake_heap_start = VRAM_START;
	fake_heap_end = VRAM_END;

	// read User Settings from firmware
	readUserSettings();

	//enable sound
	powerON(POWER_SOUND);
	writePowerManagement(PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE ) | PM_SOUND_AMP );
	SOUND_CR = SOUND_ENABLE | SOUND_VOL(0x7F);

	irqInit();

	// Start the RTC tracking IRQ
	initClockIRQ();

	SetYtrigger(80);
	irqSet(IRQ_VCOUNT, VcountHandler);
//	irqSet(IRQ_VBLANK, VblankHandler);

	irqEnable( IRQ_VBLANK | IRQ_VCOUNT);

	ipcInit();
	SoundInit();

	while (1) {
		SoundLoopStep();
		swiWaitForVBlank();
	}
}


