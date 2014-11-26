/* ========================================================================
   $File: win32_handmade.cpp$
   $Date: 11.19.14$
   $Revision: 1.1$
   $Creator: Sky King $
   $Notice: (C) Copyright 2014 by Fiendish Enterprises, Inc. All Rights Reserved. $
   ======================================================================== */

#include <windows.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

// TODO(sky): This is a global for now.
global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable int BytesPerPixel = 4;

internal void
RenderWeirdGradient(int BlueOffset, int GreenOffset)
{
    int Width = BitmapWidth;
    int Height = BitmapHeight;
    
    int Pitch = Width * BytesPerPixel;
    uint8_t *Row = (uint8_t *)BitmapMemory;
    for (int Y = 0; Y < BitmapHeight; ++Y)
    {
        uint32_t *Pixel = (uint32_t *)Row;
        for (int X = 0; X < BitmapWidth; ++X)
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

        Row += Pitch;
    }
}

internal void
Win32ResizeDIBSection(int Width, int Height)
{
    // TODO(sky): Bulletproof this
    // Maybe dont free first, free after, then free first if that fails

    if (BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapWidth = Width;
    BitmapHeight = Height;
    
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    // Negative Height indicates top-down (as opposed to bottom-up) bitmap in memory.
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    // only need 24 bits (R,G,B) but keep 4 byte aligned
    BitmapInfo.bmiHeader.biBitCount = 32; 
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    // Note(sky): Thank you to Chris Hecker of Spy Party fame
    // for clarifying the dela with StretchDIBits and BitBlt!
    // No more DC for us.
    int BitmapMemorySize = (BitmapWidth * BitmapHeight) * BytesPerPixel;
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    // TODO(sky): Proabably clear this to black
}

internal void
Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height)
{
    int WindowWidth = WindowRect->right - WindowRect->left;
    int WindowHeight = WindowRect->bottom - WindowRect->top;
    
    StretchDIBits(DeviceContext,
                  /*
                    X, Y, Width, Height,
                    X, Y, Width, Height,
                  */
                  0, 0, BitmapWidth, BitmapHeight,
                  0, 0, WindowWidth, WindowHeight,
                  BitmapMemory,
                  &BitmapInfo,
                  DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
                        UINT Message,
                        WPARAM WParam,
                        LPARAM LParam)
{
    LRESULT Result = 0;

    switch(Message)
    {
        case WM_SIZE:
        {
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(Width, Height);
            OutputDebugStringA("WM_SIZE\n");
        } break;
        
        case WM_CLOSE:
        {
            Running = false;
            OutputDebugStringA("WM_CLOSE\n");
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_DESTROY:
        {
            Running = false;
            OutputDebugStringA("WM_DESTROY\n");
        } break;

#if 0
        case WM_SETCURSOR:
        {
            SetCursor(0);
        } break;
#endif        
        case WM_CHAR:
        {
            OutputDebugStringA("WM_CHAR\n");
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;

            RECT ClientRect;
            GetClientRect(Window, &ClientRect);

            Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
            EndPaint(Window, &Paint);
        } break;
        
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }
    
    return(Result);
}

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
    WNDCLASS WindowClass = {};
    // TODO(sky): Check if HREDRAW/VREDRAW/OWNDC still matter
    //WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
    // WindowClass.hIcon = ;
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";
    
    if (RegisterClassA(&WindowClass))
    {
        HWND Window = 
            CreateWindowExA(
                0,
                WindowClass.lpszClassName,
                "Handmade Hero",
                WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                0,
                0,
                Instance,
                0);
        if (Window)
        {
            int XOffset = 0;
            int YOffset = 0;

            Running = true;
            while (Running)
            {
                MSG Message;
                while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if (Message.message == WM_QUIT)
                    {
                        Running = false;
                    }
                    
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                
                RenderWeirdGradient(XOffset, YOffset);
                HDC DeviceContext = GetDC(Window);
                RECT ClientRect;
                GetClientRect(Window, &ClientRect);
                int WindowWidth = ClientRect.right - ClientRect.left;
                int WindowHeight = ClientRect.bottom - ClientRect.top;
                Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
                ReleaseDC(Window, DeviceContext);
                
                ++XOffset;
                --YOffset;
            }
        }
        else
        {
            // TODO(Sky): logging
        }
    }
    else
    {
        // TODO(Sky): logging
    }
    
    return(0);
}
