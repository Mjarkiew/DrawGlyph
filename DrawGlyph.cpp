#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <X11/Xlib.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

int main (int argc, char *argv[])
{
    char* FileName;
    char* FontName;
    int Codepoint = (int)'g';

    Display *Disp = XOpenDisplay(0);
    int Screen = DefaultScreen(Disp);
    Visual* Vis = DefaultVisual(Disp, Screen);
    int Depth = DefaultDepth(Disp, Screen);
    XSetWindowAttributes Attribs;

    // this is AARRGGBB (AA seems to have no effect)
    Attribs.background_pixel = 0xFFFFFFFF;

    Window Wnd = XCreateWindow(Disp, XRootWindow(Disp, Screen),
                            200, 200, 350, 200, 5, Depth,  InputOutput,
                            Vis ,CWBackPixel, &Attribs);

    XSelectInput(Disp, Wnd, ExposureMask | KeyPressMask);

    int32 MaxFontHeight = 60;
    int32 MaxFontWidth = 60;

    Pixmap PMap = XCreatePixmap(Disp, DefaultRootWindow(Disp), MaxFontHeight + 10, MaxFontWidth + 10, Depth);

    GC GraphicsContext = XDefaultGC(Disp, Screen);

    // Draw black background
    XSetForeground(Disp, GraphicsContext, 0x00000000);
    XFillRectangle(Disp, PMap, GraphicsContext, 0, 0, MaxFontWidth, MaxFontHeight);
    int32 FontStartX = 20;
    int32 FontStartY = 40;

    // Draw white character on the background
    XSetForeground(Disp, GraphicsContext, 0xFFFFFFFF);
    XChar2b MultiChar;
    MultiChar.byte2 = (Codepoint & 0xFF);
    MultiChar.byte1 = (Codepoint >> 8);
    XDrawString16(Disp, PMap, GraphicsContext, FontStartX, FontStartY, &MultiChar, 1);

    // Trim?
    XImage* CharBitmap = XGetImage(Disp, PMap, 0, 0, MaxFontWidth, MaxFontHeight, AllPlanes, ZPixmap);

    int32 MinX = 10000;
    int32 MinY = 10000;
    int32 MaxX = -10000;
    int32 MaxY = -10000;

    uint32* Row = (uint32*)CharBitmap->data;
    for(int32 Y = 0;
        Y < MaxFontHeight;
        ++Y)
    {
        uint32* Pixel = Row;
        for(int32 X = 0;
            X < MaxFontWidth;
            ++X)
        {
            if((*Pixel & 0xFFFFFF) != 0)
            {
                if(MinX > X)
                {
                    MinX = X;
                }

                if(MinY > Y)
                {
                    MinY = Y;
                }

                if(MaxX < X)
                {
                    MaxX = X;
                }

                if(MaxY < Y)
                {
                    MaxY = Y;
                }
            }
            ++Pixel;
        }
        Row += MaxFontWidth;
    }

    // try paint new with trimmed dimensions
    --MinX;
    --MinY;
    ++MaxX;
    ++MaxY;

    int32 CharWidth = (MaxX - MinX) + 1;
    int32 CharHeight = (MaxY - MinY) + 1;

    PMap = XCreatePixmap(Disp, DefaultRootWindow(Disp), CharWidth, CharHeight, Depth);
    XSetForeground(Disp, GraphicsContext, 0x00000000);
    XFillRectangle(Disp, PMap, GraphicsContext, 0, 0, CharWidth, CharHeight);
    XSetForeground(Disp, GraphicsContext, 0xFFFFFFFF);
    XDrawString16(Disp, PMap, GraphicsContext, 1, CharHeight, &MultiChar, 1);

    /*
    XCopyArea(Disp, PMap, Wnd, GraphicsContext,
              0, 0,
              MaxFontWidth, MaxFontHeight,
              0, 0);
    */

    XFlush(Disp);
    XMapWindow(Disp, Wnd);
    XFlush(Disp);

    XEvent Ev;

     while(1)
     {
        XNextEvent(Disp, &Ev);

        switch(Ev.type)
        {
            case Expose:
                 //XDrawLine(Disp, Wnd, GraphicsContext, 0, 0, 100, 100);
                 XCopyArea(Disp, PMap, Wnd, GraphicsContext,
                           0, 0,
                           MaxFontWidth, MaxFontHeight,
                           30, 30);
                 //XDrawString(display, window, gr_context, 100, 100, "hello", 5);
                 break;
            case KeyPress:
                 XCloseDisplay(Disp);
                 exit(0);
        }
     }
}
