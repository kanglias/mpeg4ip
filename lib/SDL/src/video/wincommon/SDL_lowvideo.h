/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997, 1998, 1999, 2000, 2001  Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@devolution.com
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id: SDL_lowvideo.h,v 1.2 2001/08/23 00:09:18 wmaycisco Exp $";
#endif

#ifndef _SDL_lowvideo_h
#define _SDL_lowvideo_h

#include <windows.h>

#include "SDL_sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS	SDL_VideoDevice *this

#define DDRAW_FULLSCREEN() 						\
(									\
	((SDL_VideoSurface->flags & SDL_FULLSCREEN) == SDL_FULLSCREEN) && \
	((SDL_VideoSurface->flags & SDL_OPENGL    ) != SDL_OPENGL    ) && \
	(strcmp(this->name, "directx") == 0)				\
)

#define DINPUT_FULLSCREEN() 						\
(									\
	((SDL_VideoSurface->flags & SDL_FULLSCREEN) == SDL_FULLSCREEN) && \
	(strcmp(this->name, "directx") == 0)				\
)

/* The main window -- and a function to set it for the audio */
extern const char *SDL_Appname;
extern HINSTANCE SDL_Instance;
extern HWND SDL_Window;
extern const char *SDL_windowid;

/* Variables and functions exported to other parts of the native video
   subsystem (SDL_sysevents.c)
*/
/* Called by windows message loop when system palette is available */
extern void (*WIN_RealizePalette)(_THIS);

/* Called by windows message loop when the system palette changes */
extern void (*WIN_PaletteChanged)(_THIS, HWND window);

/* Called by windows message loop when losing or gaining gamma focus */
extern void (*WIN_SwapGamma)(_THIS);

/* Called by windows message loop when a portion of the screen needs update */
extern void (*WIN_WinPAINT)(_THIS, HDC hdc);

/* Called by windows message loop when the message isn't handled */
extern LONG (*HandleMessage)(_THIS, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/* The window cursor (from SDL_sysmouse.c) */
extern HCURSOR SDL_hcursor;

/* The bounds of the window in screen coordinates */
extern RECT SDL_bounds;

/* Flag -- SDL is performing a resize, rather than the user */
extern int SDL_resizing;

/* Flag -- the mouse is in relative motion mode */
extern int mouse_relative;

/* This is really from SDL_dx5audio.c */
extern void DX5_SoundFocus(HWND window);

#endif /* SDL_lowvideo_h */