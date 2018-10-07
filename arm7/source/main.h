#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#ifndef __MAIN_H__
#define __MAIN_H__

#include "../../common/soundcommon.h"
#include "sound7.h"
#include "ipc7.h"

void SoundInit(void);

void toggleBottomLight(void);
void exitMainLoop(void);

#endif
