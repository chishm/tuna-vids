Tuna-viDS
=========

By Chishm

Tuna-viDS is an Xvid video player for the Nintendo DS. It plays AVI files that
contain Xvid encoded video and MP3 encoded audio.

The newest release is available to download from the
[Tuna-viDS web page](https://www.chishm.com/tuna-vids/index.html).

Controls
--------

Either touch the controls on screen, or use the following buttons:
* **B**: Play/Pause
* **X**: Toggle bottom backlight
* **Left**: Increase video delay by one frame
* **Right**: Decrease video delay by one frame

Converting Videos
-----------------

Tuna-viDS plays videos with the following format:
* Video: 256px wide, up to 192px tall, up to 12fps, Xvid encoded, non-interlaced
* Audio: Stereo or mono MP3
* Container: AVI with chunk index, video must be the first stream (fourcc 00dc)

You can use [FFmpeg](http://ffmpeg.mplayerhq.hu/) to convert videos for
Tuna-viDS with this command:

```sh
ffmpeg -i <input-file> -f avi -r 10 -s 256x192 -b 192k -bt 64k \
	-vcodec mpeg4 -deinterlace \
	-acodec libmp3lame -ar 32000 -ab 96k -ac 2 tuna-vids.avi
```

This encodes the video as Xvid at 256×192px, 10fps, 192±64kbps. You can try up
to 12fps, but anything higher will slow down the decoder too much. It encodes
the audio as stereo MP3 at 32kHz, 96kbps. It muxes it into an AVI container with
the filename `tuna-vids.avi`. Feel free to experiment with bit rates, sample
rates, and frame rates. Just keep the video exactly 256px wide and no more than
192px tall.

Loader
======

Tuna-viDS will load files from the path supplied by a homebrew loader. One such
loader is
[NDS Homebrew Menu](https://github.com/devkitPro/nds-hb-menu/releases).

Usage
-----

1. Install NDS Hombrew Menu to your flash card.
2. Copy `tuna-vids.nds` to the `nds` folder on your flash card.
3. Put converted AVI files on your flash card.
4. Create a `<name>.argv` file for each AVI file with the following content:

   ```
   tuna-vids.nds </path/to/video.avi>
   ```

   Here `<name>.argv` is a text file that has the extension `.argv` (not
   `.txt`) and `</path/to/video.avi>` is the full path the AVI file.

Acknowledgements
================

Thanks to (in no particular order):
* [Xvid team](https://www.xvid.org) - The Xvid codec
* Christoph Lampert - video playback example
* Robin Watts, Sophie Wilson & Paul Gardiner - YUV to RGB colour space converter
* [HelixCommunity](https://helixcommunity.org) - MP3 decoder
* ThomasS, [LiraNuna](http://www.liranuna.com), strager & DarkEz -
  Helix MP3 decoder port
* Costas - changes necessary for stereo MP3 support
* Thoduv & ecurtz - stereo audio deinterleaver
* [Noda](http://noda.free.fr) - some MP3 player ideas
* [abraxas](http://abraxas.no-ip.org/ndsdev/mpeg4/) - giving me the idea to port
  Xvid to the DS
* Ferrie - GUI's skin.

License
=======

The ARM9 binary is licensed under GPL v2.
The ARM7 binary is licensed under GPL v2 and RealNetworks Public Source License.
