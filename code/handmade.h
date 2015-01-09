#if !defined(HANDMADE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sky King $
   $Notice: (C) Copyright 2014 by Fiendish Enterprises, Inc. All Rights Reserved. $
   ======================================================================== */

#define HANDMADE_H

/*
  TODO(sky): Services that the game provides to the platform layer
*/

/*
  NOTE(sky): Services that the platform layer provides to the games
  (this may expand in the future - sound on a separate thread, etc.)
*/

// FOUR THINGS - timing, controller/keyboard input, bitmap buffer to use, sound buffer to use

// TODO(sky): In the future, rendering specifically will become a three-tiered abstraction!!!
struct game_offscreen_buffer
{
    // NOTE(sky): Pixels are always 32-bits wide,
    // Little Endian -x xx RR GG BB
    void *Memory;
    int Width;
    int Height;
    int Pitch;
};

struct game_sound_output_buffer
{
    int SamplesPerSecond;
    int SampleCount;
    int16_t *Samples;
};

void GameOutputSound(game_sound_output_buffer *SoundBuffer, int SampleCountToOutput);
void GameUpdateandRender(game_offscreen_buffer *Buffer, int BlueOffset, int GreenOffset,
                         game_sound_output_buffer *SoundBuffer);

#endif
