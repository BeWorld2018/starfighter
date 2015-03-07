/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
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
	char string[75] = "";
	FILE *fp;
	uint32_t *p32;
	uint16_t *p16;
	uint8_t *p8;

	freeGraphics();

	shipShape[0] = loadImage("gfx/firefly1.png");
	shipShape[1] = loadImage("gfx/firefly2.png");

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

	fp = fopen(string, "rb");

	if (fp == NULL)
		exit(1);

	while (fscanf(fp, "%d %s", &index, string) == 2)
	{
		shipShape[index] = loadImage(string);
	}

	fclose(fp);

	/*
	Create images of ships being hit that show a lot of red
	*/
	for (int i = SHIP_HIT_INDEX ; i < MAX_SHIPSHAPES ; i++)
	{
		if (shipShape[i - SHIP_HIT_INDEX] == NULL)
			continue;
		shipShape[i] = createSurface(shipShape[i - SHIP_HIT_INDEX]->w,
			shipShape[i- SHIP_HIT_INDEX]->h);
		blit(shipShape[i - SHIP_HIT_INDEX], 0, 0, shipShape[i]);

		switch (shipShape[i]->format->BytesPerPixel)
		{
			case 4:
				p32 = (uint32_t *)shipShape[i]->pixels;
				for (int j = 0; j < shipShape[i]->w * shipShape[i]->h; j++)
				{
					if (p32[j])
						p32[j] |= shipShape[i]->format->Rmask;
				}
				break;

			case 2:
				p16 = (uint16_t *)shipShape[i]->pixels;
				for (int j = 0; j < shipShape[i]->w * shipShape[i]->h; j++)
				{
					if (p16[j])
						p16[j] |= shipShape[i]->format->Rmask;
				}
				break;

			case 1:
				p8 = (uint8_t *)shipShape[i]->pixels;
				for (int j = 0; j < shipShape[i]->w * shipShape[i]->h; j++)
				{
					if (p8[j])
						p8[j] = SDL_MapRGB(shipShape[i]->format, 255, 0, 0);
				}
				break;
		}

		SDL_SetColorKey(shipShape[i], SDL_TRUE,
			SDL_MapRGB(shipShape[i]->format, 0, 0, 0));
	}

	strcpy(string, "data/resources_all.dat");

	fp = fopen(string, "rb");

	while (fscanf(fp, "%d %s", &index, string) == 2)
	{
		shape[index] = loadImage(string);
	}

	fclose(fp);

	loadBackground(systemBackground[currentGame.system]);

	for (int i = 0 ; i < MAX_DEFALIENS ; i++)
	{
		if (shipShape[defEnemy[i].imageIndex[0]] != NULL)
		{
			defEnemy[i].image[0] = shipShape[defEnemy[i].imageIndex[0]];
			defEnemy[i].image[1] = shipShape[defEnemy[i].imageIndex[1]];
			defEnemy[i].engineX = defEnemy[i].image[0]->w;
			defEnemy[i].engineY = (defEnemy[i].image[0]->h / 2);
		}
	}

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


/*
Custom loading to alter the font color before doing
all other things
*/
void loadFont()
{
	SDL_Surface *image, *newImage;

	for (int i = 0 ; i < MAX_FONTSHAPES ; i++)
	{
		image = IMG_Load("gfx/smallFont.png");

		if (image == NULL) {
			printf("Couldn't load game font! (%s) Exitting.\n", SDL_GetError());
		   exit(1);
		}

		switch(i)
		{
			case 1:
				SDL_SetSurfaceColorMod(image, 255, 0, 0);
				break;
			case 2:
				SDL_SetSurfaceColorMod(image, 255, 255, 0);
				break;
			case 3:
				SDL_SetSurfaceColorMod(image, 0, 255, 0);
				break;
			case 4:
				SDL_SetSurfaceColorMod(image, 0, 255, 255);
				break;
			case 5:
				SDL_SetSurfaceColorMod(image, 0, 0, 10);
				break;
		}

		newImage = SDL_ConvertSurface(image, screen->format, 0);

		fontShape[i] = setTransparent(newImage);

		SDL_FreeSurface(image);
 	}
}
