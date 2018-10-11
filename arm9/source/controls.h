#ifndef CONTROLS_H_
#define CONTROLS_H_

#include <nds.h>

typedef struct
{
    int left;
    int top;
    int right;
    int bottom;
} ControlBounds;

void controlsSetup(void);
void consoleSetup(void);
void showPlay(void);
void showPause(void);
void updateSeekBar(int fraction);
void setEndTime(int time);
void updateTimeDisplay(int time, int avgFps, int behind);
void updateSyncDisplay(int delay);

void displayError(u32 error, bool isArm9, bool halt);

static inline bool hitCheck(const touchPosition* penPos,
    const ControlBounds* control, const uint32 keys)
{
    return penPos->px >= control->left && penPos->px <= control->right
        && penPos->py >= control->top && penPos->py <= control->bottom
        && (keys & KEY_TOUCH);
}

#endif // CONTROLS_H_
