/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2015 Julian Marchant

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

static cutMsg cutMessage[10];
static event gameEvent[10];

static void setKlineGreeting()
{
	static const char *greet[] = {
		"How nice to see you again, Bainfield!",
		"It all ends here, rebel!",
		"I hope you won't disappoint me this time...",
		"Do you really think you can defeat us?!"
	};

	gameEvent[0].time = 2;
	gameEvent[0].face = FACE_KLINE;
	strcpy(gameEvent[0].message, greet[rand() % 4]);
	gameEvent[0].entity = -1;
	gameEvent[0].flag = 0;
}

void loadScriptEvents()
{
	for (int i = 0 ; i < 10 ; i++)
	{
		gameEvent[i].time = 0;
		strcpy(gameEvent[i].message, "");
		gameEvent[i].entity = -1;
		gameEvent[i].flag = 0;
	}
	
	if ((currentGame.area == 27) && (enemy[WC_KLINE].classDef == CD_KLINE))
		setKlineGreeting();

	char filename[255];
	sprintf(filename, "data/script%d.txt", currentGame.area);

	FILE *fp;
	int i = 0;

	int time, entity, flags;
	char face[255], message[255];

	fp = fopen(filename, "rb");
	if (fp == NULL)
		return;

	while (fscanf(fp, "%d %s %d %d ", &time, face, &entity, &flags) == 4)
	{
		if (fscanf(fp, "%[^\n]%*c", message) < 1)
			strcpy(message, "Error: Text missing");

		gameEvent[i].time = time;
		gameEvent[i].face = getFace(face);
		gameEvent[i].entity = entity;
		gameEvent[i].flag = flags;
		strcpy(gameEvent[i].message, message);

		i++;
	}

	fclose(fp);
}

void checkScriptEvents()
{
	for (int i = 0 ; i < 10 ; i++)
	{
		if (engine.timeTaken == gameEvent[i].time)
		{
			if (strcmp(gameEvent[i].message, "@none@") != 0)
			{
				setRadioMessage(gameEvent[i].face, gameEvent[i].message, 1);
			}

			if (gameEvent[i].entity > -1)
			{
				if (gameEvent[i].flag != -FL_ACTIVATE)
				{
					enemy[gameEvent[i].entity].flags += gameEvent[i].flag;
				}
				else
				{
					enemy[gameEvent[i].entity].active = true;
					enemy[gameEvent[i].entity].x = rrand((int)player.x + 400, (int)player.x + 800);
					enemy[gameEvent[i].entity].y = rrand((int)player.y - 400, (int)player.y + 800);
				}
			}

			gameEvent[i].time = 0;
		}
	}
}

void syncScriptEvents()
{
	for (int i = 0 ; i < 10 ; i++)
	{
		if (gameEvent[i].time < 0)
		{
			gameEvent[i].time = engine.timeTaken + abs(gameEvent[i].time);
		}
	}
}

static void setScene(int scene)
{
	FILE *fp;
	char string[255], face[255];
	float sx, sy, x, y, speed;
	int index, shape;

	sprintf(string, "data/cutscene%d.dat", scene);

	fp = fopen(string, "rb");

	// Load in the specified background
	if (fscanf(fp, "%s", string) < 1)
	{
		printf("Warning: didn't find a background definition for \"%s\"\n", string);
		strcpy(string, "gfx/spirit.jpg");
	}
	loadBackground(string);

	// Set the star speed
	if (fscanf(fp, "%f %f", &sx, &sy) < 2)
		printf("Warning: failed to read star speed data for cutscene");
	engine.ssx = sx;
	engine.ssy = sy;

	// Read in the specs for each ship
	while (fscanf(fp, "%d %d %f %f %f", &index, &shape, &x, &y, &speed) == 5)
	{
		if (x < 0) x = (rand() % abs((int)x));
		if (y < 0) y = (rand() % abs((int)y));
		if (speed <= -1) speed = 1 + (rand() % abs((int)speed));

		if (shape > -1)
		{
			enemy[index].image[0] = shipShape[shape];
			enemy[index].x = x;
			enemy[index].y = y;
			enemy[index].dx = speed;
			enemy[index].active = true;
		}
	}

	// And finally read in the messages
	index = 0;
	while (fscanf(fp, "%s%*c %[^\n]", face, string) == 2)
	{
		if (strcmp(string, "@none@") == 0)
			break;

		cutMessage[index].face = getFace(face);
		strcpy(cutMessage[index].message, string);

		index++;
	}

	fclose(fp);
}

void doCutscene(int scene)
{
	clearScreen(black);
	updateScreen();
	clearScreen(black);

	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;

	engine.ssx = -0.5;
	engine.ssy = 0;
	engine.smx = 0;
	engine.smy = 0;

	flushBuffer();
	freeGraphics();
	resetLists();
	loadGameGraphics();

	for (int i = 0 ; i < 15 ; i++)
	{
		enemy[i] = defEnemy[0];
		enemy[i].face = 0;
		enemy[i].active = false;
	}

	for (int i = 0 ; i < 10 ; i++)
	{
		strcpy(cutMessage[i].message, "");
		cutMessage[i].face = -1;
	}

	setScene(scene);

	/*
		Because we can fiddle with the images, we need to set the engines to 
		the correct places on the craft. Otherwise it will look wrong
	*/
	for (int i = 0 ; i < 15 ; i++)
	{
		enemy[i].engineX = enemy[i].image[0]->w;
		enemy[i].engineY = (enemy[i].image[0]->h / 2);
	}

	bool showMessage = false;
	signed char currentMessage = -1;
	int timer = 60 * 4;

	drawBackGround();

	SDL_Surface *face;
	
	flushInput();

	while (true)
	{
		updateScreen();
		unBuffer();
		getPlayerInput();
		doStarfield();
		doExplosions();

		for (int i = 0 ; i < 15 ; i++)
		{
			if (enemy[i].active)
			{
				addEngine(&enemy[i]);
				if (scene == 0 && i > 0 && (timer % 15) == i) {
					enemy[i].dx += (drand48() - 0.5) * 0.1;
					enemy[i].dy += (drand48() - 0.5) * 0.1;
					if (enemy[i].x > 500 - timer)
						enemy[i].dx -= 0.2;
					if (enemy[i].x < 0)
						enemy[i].dx += 0.2;
				}
				enemy[i].x += enemy[i].dx;
				enemy[i].y += enemy[i].dy;
				enemy[i].x += engine.ssx + engine.smx;
				blit(enemy[i].image[0], (int)enemy[i].x, (int)enemy[i].y);
				if (enemy[i].x > (screen->w + 50))
				{
					enemy[i].x = -50;
					enemy[i].y = rand() % (screen->h - 40);
				}
				if (enemy[i].y < -50)
					enemy[i].y = (screen->h + 50);
				if (enemy[i].y > (screen->h + 50))
					enemy[i].y = -50;
			}
		}

		timer--;
		if (timer == 0)
		{
			showMessage = !showMessage;
			timer = 120;
			if (showMessage)
			{
				timer = 60 * 7;
				currentMessage++;

				if (currentMessage == 10)
					break;

				if (strcmp(cutMessage[currentMessage].message, "") == 0)
					break;

				face = NULL;
				if (cutMessage[currentMessage].face != -1)
					face = shape[cutMessage[currentMessage].face];
				createMessageBox(face, cutMessage[currentMessage].message, 0);
			}
		}

		if ((showMessage) && (messageBox != NULL))
			blit(messageBox, (screen->w - messageBox->w) / 2, screen->h - 100);

		drawString("Press [Escape] to skip", -1, 580, FONT_WHITE);

		delayFrame();

		if (engine.keyState[KEY_ESCAPE])
			break;
	}

	flushBuffer();
	freeGraphics();
	clearScreen(black);
	updateScreen();
}
