/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011 Guus Sliepen
Copyright (C) 2015 Julian Marchant

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SCREEN_H
#define SCREEN_H

#include "structs.h"

extern SDL_Surface *screen;
extern bRect *screen_bufferHead;
extern bRect *screen_bufferTail;

void screen_blit(SDL_Surface *image, int x, int y);
void screen_blitText(int i);
int screen_renderString(const char *in, int x, int y, int fontColor);
void screen_addBuffer(int x, int y, int w, int h);
void screen_flushBuffer();
void screen_unBuffer();

#endif
