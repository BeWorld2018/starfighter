/*
Project: Starfighter
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012 Guus Sliepen
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

int main(int argc, char *argv[])
{
	if (chdir(DATADIR) == -1)
		printf("Warning: failed to change directory to \"%s\"\n", DATADIR);

	defineGlobals(); // Must do this first!

	bool cheatAttempt = false;
	int cheatCount = 0;

	if (argc > 1)
	{
		if (strcmp("--help", argv[1]) == 0)
		{
			printf("\nProject: Starfighter %s\n", VERSION);
			printf("Copyright Parallel Realities 2003\n");
			printf("Copyright Guus Sliepen, Astrid S. de Wijn and others 2012\n");
			printf("Additional Commands\n");
			printf("\t-noaudio     Disables sound and music\n");
			printf("\t-mono        Mono sound output (best for headphones)\n\n");
			printf("http://sourceforge.net/projects/pr-starfighter/\n");
			printf("\n");
			exit(0);
		}
	}

	for (int i = 1 ; i < argc ; i++)
	{
		if (strcmp(argv[i], "-nomove") == 0)
			{printf("Enemy movement disabled\n"); dev.moveAliens = 0;}
		if (strcmp(argv[i], "-nofire") == 0)
			{printf("Enemy firing disabled\n"); dev.fireAliens = 0;}
		if (strcmp(argv[i], "-cheat") == 0)
			cheatAttempt = true;
		if (strcmp(argv[i], "-noaudio") == 0)
			{printf("No Audio\n"); engine.useAudio = false;}
		if (strcmp(argv[i], "-mono") == 0)
			{printf("Mono sound output\n"); engine.useAudio = true;}
		if ((strcmp(argv[i], "humans") == 0) && (cheatCount == 0))
			cheatCount = 1;
		if ((strcmp(argv[i], "do") == 0) && (cheatCount == 1))
			cheatCount = 2;
		if ((strcmp(argv[i], "it") == 0) && (cheatCount == 2))
			cheatCount = 3;
		if (((strcmp(argv[i], "better") == 0) && (cheatCount == 3)) ||
				(strcmp(argv[i], "humansdoitbetter") == 0))
			{printf("Humans do it better! Cheats enabled.\n"); engine.cheat = true;}
	}

	atexit(cleanUp);

	initGraphics();
	initSystem(); // Opens video mode and sound
	loadFont();

	if (cheatAttempt && !engine.cheat)
	{
		clearScreen(black);
		drawString("That doesn't work anymore", -1, 285, FONT_WHITE);
		drawString("Try harder...", -1, 315, FONT_WHITE);
		updateScreen();
		SDL_Delay(2000);
		clearScreen(black);
		updateScreen();
		SDL_Delay(500);
	}

	freeGraphics();
	loadSound();

	initWeapons();

	initVars();
	defineAliens();

	setColorIndexes();

	showStory();

	// Determine which part of the game we will go to...
	int section = 0;

	currentGame.difficulty = DIFFICULTY_NORMAL;
	newGame();

	while (true)
	{
		switch(section)
		{
			case 0:
				section = doTitle();
				break;

			case 1:
				section = galaxyMap();
				break;

			case 2:
				if (currentGame.stationedPlanet == -1) {doCutscene(0);}
				section = mainGameLoop();
				break;
		}
	}

	return(0);
}
