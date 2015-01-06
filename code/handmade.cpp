/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sky King $
   $Notice: (C) Copyright 2014 by Fiendish Enterprises, Inc. All Rights Reserved. $
   ======================================================================== */

#include "handmade.h"

internal void
RenderWeirdGradient(game_offscreen_buffer *Buffer, int BlueOffset, int GreenOffset)
{
    // TODO(sky): Lets see what the optimizer does

    uint8_t *Row = (uint8_t *)Buffer->Memory;
    for (int Y = 0;
         Y < Buffer->Height;
         ++Y)
    {
        uint32_t *Pixel = (uint32_t *)Row;
        for (int X = 0;
             X < Buffer->Width;
             ++X)
        {
            /*
			  Little Endian Architecture
              3  2  1  0
              Pixel in memory: 00 00 00 00
              BB GG RR XX
              Register:        xx RR GG BB
            */
            uint8_t Blue = (uint8_t)(X + BlueOffset);
            uint8_t Green = (uint8_t)(Y + GreenOffset);

            *Pixel++ = ((Green << 8) | Blue);
        }

        Row += Buffer->Pitch;
    }
}

internal void
GameUpdateandRender(game_offscreen_buffer *Buffer, int BlueOffset, int GreenOffset)
{
//    int BlueOffset = 0;
//    int GreenOffset = 0;
    RenderWeirdGradient(Buffer, BlueOffset, GreenOffset);
}
