#include <nds.h>
#include <nds/arm9/console.h>
#include <stdio.h>
#include <string.h>

#include "controls.h"
#include "controlsImg.h"
#include "bios_decompress_callback.h"

#define TILE_BASE 0
#define STATIC_MAP_BASE 22
#define OVERLAY_MAP_BASE 23

#define SCREEN_TILE_WIDTH 32
#define TILE_WIDTH 8
#define SCREEN_TILE_HEIGHT 24

#define NUMERAL_TOP_ROW 26
#define NUMERAL_BOTTOM_ROW 27
#define PLUS_TILE_OFFSET 10
#define MINUS_TILE_OFFSET 11
#define DOT_TILE_OFFSET 12
#define COLON_TILE_OFFSET 13
#define BLANK_TILE_POSITION (SCREEN_TILE_WIDTH * 28 - 1)
#define BUTTON_HEIGHT 4
#define BUTTON_WIDTH 6
#define PLAY_TILE_OFFSET (SCREEN_TILE_WIDTH * 24 + 15)
#define PAUSE_TILE_OFFSET (SCREEN_TILE_WIDTH * 8 + 15)
#define LIGHT_TILE_OFFSET (SCREEN_TILE_WIDTH * 8 + 24)
#define SEEK_TOP_ROW 4
#define SEEK_BOTTOM_ROW 5
#define SEEK_BAR_OFFSET (SCREEN_TILE_WIDTH * SEEK_TOP_ROW)

#define CENTIHERTZ_DECIMAL 2

#define START_TIME_OFFSET (1 * SCREEN_TILE_WIDTH + 6)
#define CUR_TIME_OFFSET (1 * SCREEN_TILE_WIDTH + 19)
#define END_TIME_OFFSET (1 * SCREEN_TILE_WIDTH + 31)

#define SYNC_OFFSET (9 * SCREEN_TILE_WIDTH + 9)
#define SYNC_DISPLAY_WIDTH 6

#define AVG_FPS_OFFSET (16 * SCREEN_TILE_WIDTH + 9)
#define CUR_FPS_OFFSET (16 * SCREEN_TILE_WIDTH + 19)
#define BEHIND_OFFSET (16 * SCREEN_TILE_WIDTH + 29)
#define TIME_DISPLAY_WIDTH 8

#define PLAYBACK_BUTTON_OFFSET (SCREEN_TILE_WIDTH * 8 + 15)

#define STATIC_MAP BG_MAP_RAM_SUB(STATIC_MAP_BASE)
#define OVERLAY_MAP BG_MAP_RAM_SUB(OVERLAY_MAP_BASE)

typedef u16 (*BgMap)[SCREEN_TILE_WIDTH];

typedef struct
{
    u16 top;
    u16 bottom;
} NumeralTiles;

typedef struct
{
    u16 tiles[BUTTON_HEIGHT][BUTTON_WIDTH];
} ButtonTiles;

NumeralTiles numerals[10];
NumeralTiles plusSymbol;
NumeralTiles minusSymbol;
NumeralTiles dotSymbol;
NumeralTiles colonSymbol;

u16 blankTile;

ButtonTiles playButton;
ButtonTiles pauseButton;
ButtonTiles lightButton;

NumeralTiles seekBarBlocks[9];

static void vramcpy(void* dest, const void* src, int size)
{
    u16* destination = (u16*) dest;
    u16* source = (u16*) src;
    while (size > 0) {
        *destination++ = *source++;
        size -= 2;
    }
}

/* Return a tile from the controls image map at the specified row and column */
static inline const unsigned short getControlsImgMap(int row, int col)
{
    return controlsImgMap[SCREEN_TILE_WIDTH * row + col];
}

/* Display a numeral (or the like) on the overlay background map.
 * offset incorporates the row and column offset into one value.
 */
static inline void setMapNumeral(int offset, const NumeralTiles* numeral)
{
    BgMap overlayMap = (BgMap)(OVERLAY_MAP + offset);

    overlayMap[0][0] = numeral->top;
    overlayMap[1][0] = numeral->bottom;
}

void controlsSetup(void)
{
    // Just use BG 0 of the sub screen, with the unused VRAM blocks
    videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE);
    vramSetBankH(VRAM_H_SUB_BG);
    vramSetBankI(VRAM_I_SUB_BG_0x06208000);
    REG_BG0CNT_SUB = BG_MAP_BASE(STATIC_MAP_BASE) | BG_COLOR_256
        | BG_TILE_BASE(TILE_BASE) | BG_PRIORITY(2);
    REG_BG1CNT_SUB = BG_MAP_BASE(OVERLAY_MAP_BASE) | BG_COLOR_256
        | BG_TILE_BASE(TILE_BASE) | BG_PRIORITY(0);

    // Very important - blank tile
    blankTile = controlsImgMap[BLANK_TILE_POSITION];

    // Copy in palette and tiles
    vramcpy(&BG_PALETTE_SUB[0], controlsImgPal, controlsImgPalLen);
    swiDecompressLZSSVram((void*) controlsImgTiles,
        (void*) BG_TILE_RAM_SUB(TILE_BASE), 0, &decompressBiosCallback);

    // Initialise background to static image
    vramcpy(STATIC_MAP, (void*) controlsImgMap,
        SCREEN_TILE_WIDTH * SCREEN_TILE_HEIGHT * sizeof(controlsImgMap[0]));

    // Clear foreground
    for (int i = 0; i < SCREEN_TILE_WIDTH * SCREEN_TILE_HEIGHT; i++) {
        OVERLAY_MAP[i] = blankTile;
    }

    // Get numeral (and related) tiles
    for (int i = 0; i < 10; i++) {
        numerals[i].top = getControlsImgMap(NUMERAL_TOP_ROW, i);
        numerals[i].bottom = getControlsImgMap(NUMERAL_BOTTOM_ROW, i);
    }
    plusSymbol.top = getControlsImgMap(NUMERAL_TOP_ROW, PLUS_TILE_OFFSET);
    plusSymbol.bottom = getControlsImgMap(NUMERAL_BOTTOM_ROW, PLUS_TILE_OFFSET);
    minusSymbol.top = getControlsImgMap(NUMERAL_TOP_ROW, MINUS_TILE_OFFSET);
    minusSymbol.bottom = getControlsImgMap(NUMERAL_BOTTOM_ROW, MINUS_TILE_OFFSET);
    dotSymbol.top = getControlsImgMap(NUMERAL_TOP_ROW, DOT_TILE_OFFSET);
    dotSymbol.bottom = getControlsImgMap(NUMERAL_BOTTOM_ROW, DOT_TILE_OFFSET);
    colonSymbol.top = getControlsImgMap(NUMERAL_TOP_ROW, COLON_TILE_OFFSET);
    colonSymbol.bottom = getControlsImgMap(NUMERAL_BOTTOM_ROW, COLON_TILE_OFFSET);

    // Get buttons
    for (int i = 0; i < BUTTON_HEIGHT; i++) {
        memcpy(playButton.tiles[i],
            &controlsImgMap[SCREEN_TILE_WIDTH * i + PLAY_TILE_OFFSET],
            BUTTON_WIDTH * sizeof(u16));
        memcpy(pauseButton.tiles[i],
            &controlsImgMap[SCREEN_TILE_WIDTH * i + PAUSE_TILE_OFFSET],
            BUTTON_WIDTH * sizeof(u16));
        memcpy(lightButton.tiles[i],
            &controlsImgMap[SCREEN_TILE_WIDTH * i + LIGHT_TILE_OFFSET],
            BUTTON_WIDTH * sizeof(u16));
    }

    // Get seek bar blocks
    for (int i = 0; i < 9; i++) {
        seekBarBlocks[i].top = getControlsImgMap(SEEK_TOP_ROW, i);
        seekBarBlocks[i].bottom = getControlsImgMap(SEEK_BOTTOM_ROW, i);
    }

    updateSeekBar(0);
    updateSyncDisplay(0);
    updateTimeDisplay(0, 0, 0);
}

void consoleSetup(void)
{
    videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
    vramSetBankH(VRAM_H_SUB_BG);
    REG_BG0CNT_SUB = BG_MAP_BASE(15);
    for (int i = 0; i < 254; i++) {
        BG_PALETTE_SUB[i] = 0;
    }
    BG_PALETTE_SUB[255] = RGB15(31, 31, 31);
    consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);
}

void showPlay(void)
{
    BgMap playButtonMap = (BgMap)(OVERLAY_MAP + PLAYBACK_BUTTON_OFFSET);

    for (int row = 0; row < BUTTON_HEIGHT; row++) {
        for (int col = 0; col < BUTTON_WIDTH; col++) {
            playButtonMap[row][col] = playButton.tiles[row][col];
        }
    }
}

void showPause(void)
{
    BgMap playButtonMap = (BgMap)(OVERLAY_MAP + PLAYBACK_BUTTON_OFFSET);

    for (int row = 0; row < BUTTON_HEIGHT; row++) {
        for (int col = 0; col < BUTTON_WIDTH; col++) {
            playButtonMap[row][col] = pauseButton.tiles[row][col];
        }
    }
}

void updateSeekBar(int fraction)
{
    int col;

    if (fraction < 0)
        fraction = 0;
    if (fraction > 256)
        fraction = 256;

    BgMap seekBarMap = (BgMap)(OVERLAY_MAP + SEEK_BAR_OFFSET);

    for (col = 0;
        fraction >= TILE_WIDTH && col < SCREEN_TILE_WIDTH;
        ++col, fraction -= TILE_WIDTH)
    {
        seekBarMap[0][col] = seekBarBlocks[TILE_WIDTH].top;
        seekBarMap[1][col] = seekBarBlocks[TILE_WIDTH].bottom;
    }

    if (col < SCREEN_TILE_WIDTH) {
        seekBarMap[0][col] = seekBarBlocks[fraction].top;
        seekBarMap[1][col] = seekBarBlocks[fraction].bottom;
        ++col;
    }

    for (; col < SCREEN_TILE_WIDTH; ++col) {
        seekBarMap[0][col] = seekBarBlocks[0].top;
        seekBarMap[1][col] = seekBarBlocks[0].bottom;
    }
}

void showNumber(int num, int rightOffset, int width, int decimalPlace, bool showPlus)
{
    int digit;
    bool negative = false;

    if (num < 0) {
        negative = true;
        num = -num;
    } else if (num == 0) {
        showPlus = false;
    }

    do {
        if (decimalPlace == 0) {
            setMapNumeral(rightOffset, &dotSymbol);
        } else {
            digit = num % 10;
            num = num / 10;
            setMapNumeral(rightOffset, &numerals[digit]);
        }
        --rightOffset;
        --decimalPlace;
        --width;
    } while (num > 0 && width > 0);

    if (negative) {
        setMapNumeral(rightOffset, &minusSymbol);
        --rightOffset;
        --width;
    } else if (showPlus) {
        setMapNumeral(rightOffset, &plusSymbol);
        --rightOffset;
        --width;
    }

    while (width > 0) {
        OVERLAY_MAP[rightOffset] = blankTile;
        OVERLAY_MAP[rightOffset + SCREEN_TILE_WIDTH] = blankTile;
        --rightOffset;
        --width;
    }
}

void showTimePart(int time, int rightOffset)
{
    int ones, tens;
    ones = time % 10;
    tens = time / 10;
    setMapNumeral(rightOffset, &numerals[ones]);
    setMapNumeral(rightOffset - 1, &numerals[tens]);
}

void showTime(int time, int rightOffset)
{
    int seconds, minutes, hours;

    seconds = time % 60;
    time /= 60;
    minutes = time % 60;
    time /= 60;
    hours = time % 10;

    showTimePart(seconds, rightOffset);
    setMapNumeral(rightOffset - 2, &colonSymbol);
    showTimePart(minutes, rightOffset - 3);
    setMapNumeral(rightOffset - 5, &colonSymbol);
    setMapNumeral(rightOffset - 6, &numerals[hours]);
}

void setEndTime(int time)
{
    showTime(0, START_TIME_OFFSET);
    showTime(time, END_TIME_OFFSET);
}

void updateTimeDisplay(int time, int avgFps, int behind)
{
    showTime(time, CUR_TIME_OFFSET);
    showNumber(avgFps, AVG_FPS_OFFSET, TIME_DISPLAY_WIDTH, CENTIHERTZ_DECIMAL, false);
    showNumber(behind, BEHIND_OFFSET, TIME_DISPLAY_WIDTH, -1, true);
}

void updateSyncDisplay(int delay)
{
    showNumber(delay, SYNC_OFFSET, SYNC_DISPLAY_WIDTH, -1, true);
}

void displayError(u32 error, bool isArm9, bool halt)
{
    consoleSetup();
    iprintf("Error on ARM%c: 0x%08lx\n", isArm9 ? '9' : '7', error);

    if (halt) {
        while (true) {
            swiWaitForVBlank();
        }
    }
}
