
#include <stdio.h>
#include <string.h>
#include "player.h"
#include "xvid/xvid.h"
#include "avi_buffer.h"
#include "sound9.h"
#include "video.h"
#include "controls.h"
#include "ipc9.h"

static int CSP = XVID_CSP_RGB555;
static int BPP = 2;

static void *dec_handle = NULL;

#define MIN_USEFUL_BYTES 1
#define UPDATE_PERIOD 60

const ControlBounds controlSeek = {0,32,255,47};
const ControlBounds controlFpsMinus = {14,72,29,87};
const ControlBounds controlFpsPlus = {82,72,97,87};
const ControlBounds controlPausePlay = {120,64,167,95};
const ControlBounds controlLight = {192,64,239,95};

/*****************************************************************************
 * Routines for decoding: init decoder, use, and stop decoder
 ****************************************************************************/

/* init decoder before first run */
static int dec_init(int frameWidth, int frameHeight)
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
	xvid_dec_create.width = frameWidth;
	xvid_dec_create.height = frameHeight;

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
static int dec_end(void) {
	int result = xvid_decore(dec_handle, XVID_DEC_DESTROY, NULL, NULL);
	dec_handle = NULL;
	return result;
}


/* Move the video up to the first frame of the next second,
or just start at the current frame if at the start of the video (frame 0)
returns the second it's up to, or -1 on error
*/
static bool cue_video (void) {
	xvid_dec_stats_t xvid_dec_stats;
	unsigned char* frameBuffer;
	AviDataChunk nextFrameData;

	if (vidBuf_NumFreeDecodeBuffers() == 0) {
		return true;
	}

	do {
		int used_bytes = 0;

		frameBuffer = vidBuf_GetNextDecodeBuffer();

		/* This loop is needed to handle VOL/NVOP reading */
		do {
			/* Decode frame */
			nextFrameData = aviGetNextVideoChunk ();
			if (nextFrameData.size <= 0) {
				break;
			}
			used_bytes = dec_main(nextFrameData.start, frameBuffer, nextFrameData.size, &xvid_dec_stats);
			if (used_bytes >= 0) {
				aviVideoUsedAmount (used_bytes);
			}
		} while (xvid_dec_stats.type <= 0 && aviBuffer.videoAmountLeft >= MIN_USEFUL_BYTES);
		vidBuf_CurrentDecodeComplete ();
	} while (vidBuf_NumFreeDecodeBuffers() > 0 && aviBuffer.videoAmountLeft >= MIN_USEFUL_BYTES && nextFrameData.size > 0);

	if (vidBuf_NumFreeDecodeBuffers() > 0 || nextFrameData.size <= 0) {
		return false;
	}

	return true;
}

/* Flush the video buffers
Also has effect of calling xvid_decore with XVID_DISCONTINUITY set
 */
static void flush_video (void) {
	xvid_dec_stats_t xvid_dec_stats;
	unsigned char* frameBuffer;
	int used_bytes = 0;

	do {
		if (vidBuf_NumFreeDecodeBuffers > 0) {

			frameBuffer = vidBuf_GetNextDecodeBuffer();

			do {
				used_bytes = dec_main(NULL, frameBuffer, -1, &xvid_dec_stats);
			} while(used_bytes>=0 && xvid_dec_stats.type <= 0);

			vidBuf_CurrentDecodeComplete ();
		}
	} while (used_bytes >= 0);

	return;
}

static void seekVideo (int offset, bool paused) {
	// Stop currently playing video + audio
	if (!paused) {
		vidBuf_StopVideo();
	}
	mp3PlayerStop(true);
	// Clean up buffers
	vidBuf_ClearFutureBuffers();
	flush_video();
	// Jump to desired location in file. There are 2 chunks per frame, one for audio, one for video
	aviSeekOffset (offset);
	// Start playing video, but it won't actually play until the audio is started too
	vidBuf_ClearFutureBuffers();
	cue_video();
	mp3PlayerStart();
	vidBuf_StartVideo();
	if (paused) {
		// Stop playback, but keep the video queued-up
		vidBuf_StopVideo();
		cue_video();
	} else {
		// Starting sound will start the video too (video sync'd to audio)
		mp3PlayerPlay(true);
	}
}

static bool videoLoopStep (void) {
	xvid_dec_stats_t xvid_dec_stats;
	unsigned char* frameBuffer;
	AviDataChunk nextFrameData;
	int used_bytes = 0;

	nextFrameData.size = 1;

	if (vidBuf_NumFreeDecodeBuffers() > 0) {
		frameBuffer = vidBuf_GetNextDecodeBuffer();

		/* This loop is needed to handle VOL/NVOP reading */
		do {
			/* Decode frame */
			nextFrameData = aviGetNextVideoChunk ();
			used_bytes = dec_main(nextFrameData.start, frameBuffer, nextFrameData.size, &xvid_dec_stats);
			if (used_bytes >= 0) {
				aviVideoUsedAmount (used_bytes);
			}
		} while (xvid_dec_stats.type <= 0 && aviBuffer.videoAmountLeft >= MIN_USEFUL_BYTES && nextFrameData.size > 0);

		/* Check if there is a negative number of useful bytes left in buffer
		 * This means we went too far */
		if(aviBuffer.videoAmountLeft < 0 || nextFrameData.size <= 0)
			return false;

		vidBuf_CurrentDecodeComplete ();

	} else if (aviBuffer.videoAmountLeft < AVI_BUFFER_SIZE) {
		aviBufferRefill ();
	} else {
		swiWaitForVBlank();
	}

	return true;
}

int play_movie (FILE* aviFile) {
	int status;
	bool paused = false;
	bool videoPlaying;
	int frameWidth, frameHeight, frameRate, numFrames=0;
	VidBufStats videoStats;
	int frameRateHz, oldVblanks;
	int audioDelay = 0;
	int framesShownOffset = 0;

	/* Memory for encoded m4v stream */
	vidBuf_StopVideo ();
	if (!aviBufferInit (aviFile, &frameWidth, &frameHeight, &frameRate, &numFrames)) {
		iprintf ("Couldn't allocate file buffer\n");
		goto free_all_memory;
	}

	vidBuf_ResetVideo();
	vidBuf_AdjustDelay (audioDelay);
	vidBuf_SetFrameRate (frameRate);
	vidBuf_SetHeight (frameHeight);

	if (frameWidth != SCREEN_WIDTH) {
		iprintf ("Video is not 256px wide!\n");
	}

	if (frameHeight > SCREEN_HEIGHT) {
		iprintf ("Video too tall!\n");
	}


	/* Memory for encoded mp3 stream */
	mp3PlayerStart();


/*****************************************************************************
 *        XviD PART  Start
 ****************************************************************************/

	status = dec_init(frameWidth, frameHeight);
	if (status) {
		iprintf("Decore INIT problem, return value %d\n", status);
		goto release_all;
	}

/*****************************************************************************
 *	                         Main loop
 ****************************************************************************/

	// Fill the buffers

	if (!cue_video()) {
		iprintf("Not enough video data\n");
		goto release_all;
	}

	// Now that it's passed all points where an error will be reported, initialise
	// the GUI, removing the text display
	controlsSetup();
	frameRateHz = frameRate / 100;
	oldVblanks = 0;
	setEndTime(numFrames/frameRateHz);
	showPause();

	// Start playback
	mp3PlayerLoop();
	vidBuf_StartVideo();
	mp3PlayerPlay(true);

	videoPlaying = true;

	while (videoPlaying && aviBuffer.videoAmountLeft >= MIN_USEFUL_BYTES && (paused || !mp3PlayerHasEnded())) {
		// Handle requests for MP3 stuff
		mp3PlayerLoop();

		touchPosition penPos;
		touchRead(&penPos);
		scanKeys();
		uint32 keys = keysDown();

		if ((keys & KEY_LEFT) || hitCheck (&penPos, &controlFpsMinus, keys)) {
			// Pause for a frame
			--audioDelay;
			vidBuf_AdjustDelay(audioDelay);
			updateSyncDisplay(audioDelay);
		} else if ((keys & KEY_RIGHT) || hitCheck (&penPos, &controlFpsPlus, keys)) {
			// Skip a frame
			++audioDelay;
			vidBuf_AdjustDelay(audioDelay);
			updateSyncDisplay(audioDelay);
		} else if ((keys & KEY_B) || hitCheck (&penPos, &controlPausePlay, keys)) {
			if (paused) {
				vidBuf_ResumeVideo();
				mp3PlayerPlay(true);
				showPause();
			} else {
				vidBuf_StopVideo();
				mp3PlayerPause();
				showPlay();
			}
			paused = !paused;
		} else if (hitCheck (&penPos, &controlSeek, keys)) {
			seekVideo (penPos.px * (AVI_SEEK_OFFSET_FACTOR/SCREEN_WIDTH), paused);
			framesShownOffset = penPos.px * numFrames / 255;
		} else if ((keys & KEY_X) || hitCheck (&penPos, &controlLight, keys)) {
			toggleBacklight();
		} else {
			videoPlaying = videoLoopStep();
		}

		if ((vidBuf_vblanks - oldVblanks >= UPDATE_PERIOD) || keys != 0)  {
			oldVblanks = vidBuf_vblanks;
			// Update display
			vidBuf_GetStats(&videoStats);
			updateTimeDisplay ((framesShownOffset + videoStats.framesShown) / frameRateHz,
				videoStats.avgFrameRate, videoStats.framesBehind);
			updateSeekBar (256 * (framesShownOffset + videoStats.framesShown) / numFrames);
		}
	};

	// Restore console so we can print text again.
	consoleSetup ();
	iprintf ("Video stopped\n");

/*****************************************************************************
 *     Flush decoder buffers
 ****************************************************************************/
	flush_video();

	// Replenish MP3 buffers
	mp3PlayerLoop();


/*****************************************************************************
 *      XviD PART  Stop
 ****************************************************************************/

 release_all:
	// Stop MP3
	mp3PlayerStop(true);
	mp3PlayerLoop();

  	if (dec_handle) {
	  	status = dec_end();
		if (status) {
			iprintf("decore RELEASE problem, return value %d\n", status);
		}
	}

 free_all_memory:
	aviBufferFree();

	iprintf("Finished playback\n");

	return 0;

}
