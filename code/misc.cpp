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

void clearInfoLines()
{
	for (int i = 0 ; i < 4 ; i++)
	{
		textShape[i].life = 0;
	}
}

// from a to b
static void copyInfoLine(int a, int b)
{
	textSurface(b, textShape[a].text, -1, 0, textShape[a].fontColor);
	textShape[b].life = textShape[a].life;
}

/*
Sets one of the three information lines on the screen. The accepts the
string and colors. It will set the information to the first free infoline
it finds (top to bottom). If it doesn't find any free ones, it will push
all the other info lines down one and add itself to the top.
*/
void setInfoLine(const char *in, int color)
{
	int index = -1;

	for (int i = 0 ; i < 3 ; i++)
	{
		if ((textShape[i].life == 0) && (index == -1))
		{
			index = i;
		}
	}

	// Bump down
	if (index == -1)
	{
		index = 2;
		copyInfoLine(1, 0);
		copyInfoLine(2, 1);
	}

	textSurface(index, in, -1, 0, color);
	textShape[index].life = 240;
}

/*
Sets a radio message that appears at the top of the screen. Used for
script events, etc. We send a message priority too, since we don't want
Phoebe or Ursula's banter to interrupt an important message
*/
void setRadioMessage(signed char face, const char *in, int priority)
{
	if ((textShape[3].life > 0) && (priority == 0))
		return;

	textSurface(3, in, -1, 50, FONT_WHITE);
	textShape[3].life = 240;

	SDL_Surface *faceShape = NULL;
	if (face > -1)
		faceShape = shape[face];

	createMessageBox(faceShape, in, 1);
}

static void doTargetArrow()
{
	if (engine.targetIndex < 0)
		return;

	if ((engine.targetArrowTimer == 0) || (enemy[engine.targetIndex].shield < 1))
		return;

	if (enemy[engine.targetIndex].flags & FL_ISCLOAKED)
		return;
		
	if (textShape[3].life > 0)
		return;

	engine.targetArrow = -1;

	if (engine.targetArrowTimer > 0)
		engine.targetArrowTimer--;

	int distX = (int)(enemy[engine.targetIndex].x - player.x);
	int distY = (int)(enemy[engine.targetIndex].y - player.y);

	if (distY < -300)
		engine.targetArrow = 36;

	if (distY > 300)
		engine.targetArrow = 40;

	if (distX < -400)
		engine.targetArrow = 42;

	if (distX > 400)
		engine.targetArrow = 38;

	if ((distY < -300) && (distX > 400))
		engine.targetArrow = 37;

	if ((distY > 300) && (distX > 400))
		engine.targetArrow = 39;

	if ((distY > 300) && (distX < -400))
		engine.targetArrow = 41;

	if ((distY < -300) && (distX < -400))
		engine.targetArrow = 43;

	if (engine.targetArrow != -1)
	{
		blit(shape[engine.targetArrow], 380, 50);
		blit(shape[44], 365, 70);
	}
}

/*
Fill in later...
*/
void doInfo()
{
	int shieldColor = 0;
	SDL_Rect bar;
	signed char fontColor;
	char text[25];

	addBuffer(0, 20, 800, 25);
	addBuffer(0, 545, 800, 25);

	if (engine.minutes > -1)
	{
		if ((engine.minutes == 0) && (engine.seconds <= 29))
			fontColor = FONT_RED;
		else if ((engine.minutes == 0) && (engine.seconds > 29))
			fontColor = FONT_YELLOW;
		else
			fontColor = FONT_WHITE;
		blitText(10); // time remaining
		sprintf(text, "%.2d:%.2d", engine.minutes, engine.seconds);
		textSurface(30, text, 410, 21, fontColor);
		blitText(30);
	}

	if (currentGame.area != MAX_MISSIONS - 1)
	{
		blitText(9); // mission objectives
		sprintf(text, "%d", (currentMission.remainingObjectives1 + currentMission.remainingObjectives2));
		textSurface(39, text, 745, 21, FONT_WHITE);
		blitText(39);
	}

	blitText(8); // cash
	sprintf(text, "%.6d", currentGame.cash);
	textSurface(38, text, 90, 21, FONT_WHITE);
	blitText(38);

	doTargetArrow();

	fontColor = FONT_WHITE;
	if (player.ammo[0] > 0)
	{
		if (player.ammo[0] <= 25) fontColor = FONT_YELLOW;
		if (player.ammo[0] <= 10) fontColor = FONT_RED;
	}
	blitText(5); // plasma ammo
	sprintf(text, "%.3d", player.ammo[0]);
	textSurface(35, text, 320, 551, fontColor);
	blitText(35);

	blitText(6);

	if ((player.weaponType[1] != W_CHARGER) && (player.weaponType[1] != W_LASER))
	{
		if (player.ammo[1] == 1)
			fontColor = FONT_RED;
		else
			fontColor = FONT_WHITE;
		sprintf(text, "%.3d", player.ammo[1]); // rocket ammo
		textSurface(36, text, 465, 551, fontColor);
		blitText(36);
	}

	if (((player.weaponType[1] == W_CHARGER) || (player.weaponType[1] == W_LASER)) && (player.ammo[1] > 0))
	{
		int c = white;
		if (player.ammo[1] > 100)
			c = red;

		bar.x = 450;
		bar.y = 550;
		bar.h = 12;

		for (int i = 0 ; i < (player.ammo[1] / 5) ; i++)
		{
			bar.w = 1;
			SDL_FillRect(screen, &bar, c);
			bar.x += 2;
		}
	}

	if ((!allMissionsCompleted()) && (SDL_GetTicks() >= engine.counter2))
	{
		engine.timeTaken++;
		engine.counter2 = SDL_GetTicks() + 1000;
		if (engine.missionCompleteTimer == 0)
			checkScriptEvents();
	}

	if ((engine.timeMission) && (!engine.cheatTime) && (player.shield > 0))
	{
		if (SDL_GetTicks() >= engine.counter)
		{
			if ((engine.seconds > 1) && (engine.seconds <= 11) && (engine.minutes == 0))
			{
				playSound(SFX_CLOCK);
			}

			if (engine.seconds > 0)
			{
				engine.seconds--;
				engine.counter = (SDL_GetTicks() + 1000);
			}
			else if ((engine.seconds == 0) && (engine.minutes > 0))
			{
				engine.minutes--;
				engine.seconds = 59;
				engine.counter = (SDL_GetTicks() + 1000);
				for (int i = 0 ; i < 3 ; i++)
				{
					if (currentMission.timeLimit1[i] > -1)
						currentMission.timeLimit1[i]--;
					if (currentMission.timeLimit2[i] > -1)
						currentMission.timeLimit2[i]--;
				}
				checkTimer();
				checkScriptEvents();
			}

			if ((engine.seconds == 0) && (engine.minutes == 0))
			{
				for (int i = 0 ; i < 3 ; i++)
				{
					if (currentMission.timeLimit1[i] > -1)
						currentMission.timeLimit1[i]--;
					if (currentMission.timeLimit2[i] > -1)
						currentMission.timeLimit2[i]--;
				}
				checkTimer();
				checkScriptEvents();
				//engine.counter = 0;
				engine.counter = (SDL_GetTicks() + 1000);
			}
		}
	}

	for (int i = 0 ; i < 3 ; i++)
	{
		if (textShape[i].life > 0)
		{
			textShape[i].y = (525 - (i * 20));
			blitText(i);
			textShape[i].life--;

			if (textShape[i].life == 0)
			{
				copyInfoLine(i + 1, i);
				copyInfoLine(i + 2, i + 1);
				textShape[2].life = 0;
			}
		}
	}

	// Show the radio message if there is one
	if (textShape[3].life > 0)
	{
		blit(messageBox, (800 - messageBox->w) / 2, 50);
		textShape[3].life--;
	}

	// Do the target's remaining shield (if required)
	if (currentGame.area != 10)
	{
		if ((engine.targetIndex > -1) && (enemy[engine.targetIndex].shield > 0) && (engine.targetIndex > 9))
		{
			blitText(7);
			bar.w = 1;
			bar.h = 12;
			bar.x = 620;
			bar.y = 550;

			for (float i = 0 ; i < (engine.targetShield * enemy[engine.targetIndex].shield) ; i++)
			{
				if (i > 50)
					shieldColor = green;
				else if ((i >= 25) && (i <= 50))
					shieldColor = yellow;
				else
					shieldColor = red;
				SDL_FillRect(screen, &bar, shieldColor);
				bar.x += 2;
			}
		}
	}

	blitText(11);

	bar.w = 25;
	bar.h = 12;
	bar.x = 80;
	bar.y = 571;
	SDL_FillRect(screen, &bar, green);

	for (int i = 1 ; i < currentGame.maxPlasmaDamage ; i++)
	{
		bar.x += 30;
		if (weapon[1].damage >= (i + 1))
			SDL_FillRect(screen, &bar, green);
		else
			SDL_FillRect(screen, &bar, darkGreen);
	}

	blitText(12);

	bar.w = 25;
	bar.h = 12;
	bar.x = 315;
	bar.y = 571;
	SDL_FillRect(screen, &bar, yellow);

	for (int i = 1 ; i < currentGame.maxPlasmaDamage ; i++)
	{
		bar.x += 30;
		if (weapon[1].ammo[0] >= (i + 1))
			SDL_FillRect(screen, &bar, yellow);
		else
			SDL_FillRect(screen, &bar, darkYellow);
	}

	blitText(13);

	bar.w = 25;
	bar.h = 12;
	bar.x = 550;
	bar.y = 571;

	for (int i = 15 ; i > (currentGame.maxPlasmaRate - 1) ; i -= 2)
	{
		if (weapon[1].reload[0] <= i)
			SDL_FillRect(screen, &bar, blue);
		else
			SDL_FillRect(screen, &bar, darkerBlue);
		bar.x += 30;
	}

	blitText(4);
	if (player.shield < 1)
		return;

	if ((!engine.keyState[SDLK_SPACE]) && (player.weaponType[1] == W_LASER) && (engine.eventTimer % 8 == 1))
		limitCharAdd(&player.ammo[1], -1, 1, 255);

	if ((engine.eventTimer < 30) && (player.shield <= engine.lowShield))
		return;

	signed char blockSize = 1;

	bar.w = blockSize;
	bar.h = 12;
	bar.x = 95;
	bar.y = 550;

	for (int i = 0 ; i < player.shield ; i += blockSize)
	{
		if (i >= engine.averageShield)
			shieldColor = green;
		else if ((i >= engine.lowShield) && (i < engine.averageShield))
			shieldColor = yellow;
		else
			shieldColor = red;
		SDL_FillRect(screen, &bar, shieldColor);
		bar.x += blockSize;
		if (player.maxShield < 75)
			bar.x++;
	}
}

int getFace(const char *face)
{
	for (int i = 0 ; i < 7 ; i++)
	{
		if (strcmp(faces[i], face) == 0)
			return 90 + i;
	}

	return -1;
}

void resetLists()
{
	object *ob, *ob2;
	collectables *c1, *c2;
	bRect *r1, *r2;

	ob = engine.bulletHead->next;
	while(ob != NULL)
	{
		ob2 = ob;
		ob = ob->next;
		delete ob2;
	}
	engine.bulletHead->next = NULL;
	engine.bulletTail = engine.bulletHead;

	ob = engine.explosionHead->next;
	while(ob != NULL)
	{
		ob2 = ob;
		ob = ob->next;
		delete ob2;
	}
	engine.explosionHead->next = NULL;
	engine.explosionTail = engine.explosionHead;

	c1 = engine.collectableHead->next;
	while (c1 != NULL)
	{
		c2 = c1;
		c1 = c1->next;
		delete c2;
	}

	engine.collectableHead->next = NULL;
	engine.collectableTail = engine.collectableHead;
	
	r1 = bufferHead->next;
	while (r1 != NULL)
	{
		r2 = r1;
		r1 = r1->next;
		delete r2;
	}
	
	bufferHead->next = NULL;
	bufferTail = bufferHead;

	ob = engine.debrisHead->next;
	while(ob != NULL)
	{
		ob2 = ob;
		ob = ob->next;
		delete ob2;
	}
	engine.debrisHead->next = NULL;
	engine.debrisTail = engine.debrisHead;
}


