/*
Copyright (C) 2003 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "Starfighter.h"

void loadBackground(const char *filename)
{
	if (background != NULL)
	{
		SDL_FreeSurface(background);
		background = NULL;
	}
	background = loadImage(filename);
	SDL_SetColorKey(background, 0, 0);
}

void loadGameGraphics()
{
	int index;
	char string[75];

	freeGraphics();

	shipShape[0] = loadImage("gfx/firefly1.png");
	shipShape[1] = loadImage("gfx/firefly2.png");

	strcpy(string, "");
	switch(currentGame.system)
	{
		case 0:
			strcpy(string, "data/resources_spirit.dat");
			break;
		case 1:
			strcpy(string, "data/resources_eyananth.dat");
			break;
		case 2:
			strcpy(string, "data/resources_mordor.dat");
			break;
		case 3:
			strcpy(string, "data/resources_sol.dat");
			break;
	}

	FILE *fp;

	#if USEPACK
	int dataLocation = locateDataInPak(string, 1);
	fp = fopen(PACKLOCATION, "rb");
	fseek(fp, dataLocation, SEEK_SET);
	#else
	fp = fopen(string, "rb");
	#endif

	if (fp == NULL)
		exit(1);

	fscanf(fp, "%d %s", &index, string);
	while (index != -1)
	{
		shipShape[index] = loadImage(string);
		fscanf(fp, "%d %s", &index, string);
	}

	fclose(fp);

	/*
	Overlay a red alpha surface onto
	*/
	SDL_Surface *hitRect;
	for (int i = SHIP_HIT_INDEX ; i < MAX_SHIPSHAPES ; i++)
	{
		if (shipShape[i - SHIP_HIT_INDEX] == NULL)
			continue;
		shipShape[i] = createSurface(shipShape[i - SHIP_HIT_INDEX]->w, shipShape[i- SHIP_HIT_INDEX]->h);
		blit(shipShape[i - SHIP_HIT_INDEX], 0, 0, shipShape[i]);
		hitRect = alphaRect(shipShape[i]->w, shipShape[i]->h, 255, 0, 0);
		blit(hitRect, 0, 0, shipShape[i]);
		SDL_SetColorKey(shipShape[i], (SDL_SRCCOLORKEY|SDL_RLEACCEL), SDL_MapRGB(shipShape[i]->format, 127, 0, 0));
		SDL_FreeSurface(hitRect);
	}

	strcpy(string, "data/resources_all.dat");

	#if USEPACK
	dataLocation = locateDataInPak(string, 1);
	fp = fopen(PACKLOCATION, "rb");
	fseek(fp, dataLocation, SEEK_SET);
	#else
	fp = fopen(string, "rb");
	#endif

	fscanf(fp, "%d %s", &index, string);
	while (index != -1)
	{
		shape[index] = loadImage(string);
		fscanf(fp, "%d %s", &index, string);
	}

	fclose(fp);

	loadBackground((char *)systemBackground[currentGame.system]);

	setAlienShapes();

	setWeaponShapes();
}

void loadSound()
{
	sound[SFX_EXPLOSION] = loadSound("sound/explode.ogg");
	sound[SFX_HIT] = loadSound("sound/explode2.ogg");
	sound[SFX_DEATH] = loadSound("sound/maledeath.ogg");
	sound[SFX_MISSILE] = loadSound("sound/missile.ogg");
	sound[SFX_PLASMA] = loadSound("sound/plasma.ogg");
	sound[SFX_CLOCK] = loadSound("sound/clock.ogg");
	sound[SFX_FLY] = loadSound("sound/flyby.ogg");
	sound[SFX_ENERGYRAY] = loadSound("sound/beamLaser.ogg");
	sound[SFX_PICKUP] = loadSound("sound/item.ogg");
	sound[SFX_SHIELDUP] = loadSound("sound/shield.ogg");
	sound[SFX_CLOAK] = loadSound("sound/cloak.ogg");
	sound[SFX_DEBRIS] = loadSound("sound/explode3.ogg");
	sound[SFX_DEBRIS2] = loadSound("sound/explode4.ogg");
	sound[SFX_LASER] = loadSound("sound/laser.ogg");
	sound[SFX_PLASMA2] = loadSound("sound/plasma2.ogg");
	sound[SFX_PLASMA3] = loadSound("sound/plasma3.ogg");
}

void freeSound()
{
	for (int i = 0 ; i < MAX_SOUNDS ; i++)
	{
		if (sound[i] != NULL)
			Mix_FreeChunk(sound[i]);
	}

	if (engine.music != NULL)
		Mix_FreeMusic(engine.music);
}


static void setFontColor(SDL_Surface *image, int red, int green, int blue)
{
	SDL_Color colors[256];
	colors[0].r = 0;
	colors[0].g = 0;
	colors[0].b = 0;
	for (int i = 1 ; i < 256 ; i++)
	{
		colors[i].r = red;
		colors[i].g = green;
		colors[i].b = blue;
	}

	SDL_SetPalette(image, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, 256);
}

/*
Custom loading to alter the font color before doing
all other things
*/
void loadFont()
{
	SDL_Surface *image, *newImage;

	for (int i = 0 ; i < MAX_FONTSHAPES ; i++)
	{
		#if USEPACK
		unpack("gfx/smallFont.png", PAK_FONT);
		image = IMG_Load_RW(engine.sdlrw, 1);
		#else
		image = IMG_Load("gfx/smallFont.png");
		#endif

		if (image == NULL) {
			printf("Couldn't load game font! (%s) Exitting.\n", SDL_GetError());
		   exit(1);
		}

		switch(i)
		{
			case 1:
				setFontColor(image, 255, 0, 0);
				break;
			case 2:
				setFontColor(image, 255, 255, 0);
				break;
			case 3:
				setFontColor(image, 0, 255, 0);
				break;
			case 4:
				setFontColor(image, 0, 255, 255);
				break;
			case 5:
				setFontColor(image, 0, 0, 10);
				break;
		}

		newImage = SDL_DisplayFormat(image);

    	fontShape[i] = setTransparent(newImage);

		SDL_FreeSurface(image);
 	}
}
