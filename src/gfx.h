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

#ifndef GFX_H
#define GFX_H

#include "Starfighter.h"

extern SDL_Surface *background;
extern SDL_Surface *shape[MAX_SHAPES];
extern SDL_Surface *shipShape[MAX_SHIPSHAPES];
extern SDL_Surface *fontShape[MAX_FONTSHAPES];
extern SDL_Surface *shopSurface[MAX_SHOPSHAPES];
extern textObject gfx_text[MAX_TEXTSHAPES];
extern SDL_Surface *messageBox;


void gfx_init();
SDL_Surface *gfx_setTransparent(SDL_Surface *sprite);
void gfx_blit(SDL_Surface *image, int x, int y, SDL_Surface *dest);
extern int drawString(const char *in, int x, int y, int fontColor, signed char wrap, SDL_Surface *dest);
extern int drawString(const char *in, int x, int y, int fontColor, SDL_Surface *dest);
extern int drawString(const char *in, int x, int y, int fontColor);
extern void drawBackGround();
extern void clearScreen(Uint32 color);
extern void delayFrame();
extern void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
extern void drawLine(SDL_Surface *dest, int x1, int y1, int x2, int y2, int col);
extern void drawLine(int x1, int y1, int x2, int y2, int col);
extern void circle(int xc, int yc, int R, SDL_Surface *PIX, int col);
extern void blevelRect(SDL_Surface *dest, int x, int y, int w, int h, Uint8 red, Uint8 green, Uint8 blue);
extern void blevelRect(int x, int y, int w, int h, Uint8 red, Uint8 green, Uint8 blue);
extern SDL_Surface *createSurface(int width, int height);
extern SDL_Surface *textSurface(const char *inString, int color);
extern void textSurface(int index, const char *inString, int x, int y, int fontColor);
extern SDL_Surface *alphaRect(int width, int height, Uint8 red, Uint8 green, Uint8 blue);
extern void createMessageBox(SDL_Surface *face, const char *message, signed char transparent);
extern void freeGraphics();

extern SDL_Surface *loadImage(const char *filename);

#endif
