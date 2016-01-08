/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2015, 2016 onpon4 <onpon4@riseup.net>

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

/*
Drives the cursor. Is used by some other screens too
*/
static void intermission_doCursor()
{
	getPlayerInput();

	LIMIT(engine.cursor_x, 10, screen->w - 10 - gfx_sprites[SP_CURSOR]->w);
	LIMIT(engine.cursor_y, 10, screen->h - 10 - gfx_sprites[SP_CURSOR]->h);
	screen_blit(gfx_sprites[SP_CURSOR], engine.cursor_x, engine.cursor_y);
}

/*
Sets the player's current status information lines. These are the lines
that are scrolled up the screen when the player clicks on Current Status
These are set only once.
*/
static void intermission_setStatusLines()
{
	char string[50];
	int total = 0;
	int completed = 0;
	int timeTaken = game.timeTaken;
	int y;

	for (int i = 0 ; i < 10 ; i++)
	{
		if (systemPlanet[i].missionNumber > -1)
		{
			switch(systemPlanet[i].missionCompleted)
   		{
				case 0:
					total++;
					break;
				case 1:
					total++;
					completed++;
					break;
			}
		}
	}

	sprintf(string, "Missions Completed : %d/%d", completed, total);
	gfx_createTextObject(TS_MISSIONS_COMPLETED, string, 0, 0, FONT_WHITE);

	sprintf(string, "Shots Fired : %d", game.shots);
	gfx_createTextObject(TS_SHOTS_FIRED, string, 0, 0, FONT_WHITE);

	sprintf(string, "Hits Scored : %d", game.hits);
	gfx_createTextObject(TS_HITS_SCORED, string, 0, 0, FONT_WHITE);

	sprintf(string, "Accuracy : %d%%", game.accuracy);
	gfx_createTextObject(TS_ACCURACY, string, 0, 0, FONT_WHITE);

	sprintf(string, "Enemies Killed by Others : %d", game.totalOtherKills);
	gfx_createTextObject(TS_OTHER_KILLS, string, 0, 0, FONT_WHITE);

	sprintf(string, "Total Cash Earned : %d", game.cashEarned);
	gfx_createTextObject(TS_CASH_EARNED, string, 0, 0, FONT_WHITE);

	gfx_createTextObject(TS_CHRIS_HEADER, "*** Chris ***", 0, 0, FONT_WHITE);

	sprintf(string, "Enemies Killed : %d", game.totalKills);
	gfx_createTextObject(TS_CHRIS_KILLS, string, 0, 0, FONT_WHITE);

	sprintf(string, "Shield Restores Picked Up : %d", game.shieldPickups);
	gfx_createTextObject(TS_CHRIS_SHIELD_PICKUPS, string, 0, 0, FONT_WHITE);

	sprintf(string, "Plasma Cells Picked Up : %d", game.cellPickups);
	gfx_createTextObject(TS_CHRIS_PLASMA_PICKUPS, string, 0, 0, FONT_WHITE);

	sprintf(string, "Rockets Picked Up : %d", game.rocketPickups);
	gfx_createTextObject(TS_CHRIS_ROCKET_PICKUPS, string, 0, 0, FONT_WHITE);

	sprintf(string, "Powerups Picked Up : %d", game.rocketPickups);
	gfx_createTextObject(TS_CHRIS_POWERUP_PICKUPS, string, 0, 0, FONT_WHITE);

	sprintf(string, "Mines Destroyed : %d", game.minesKilled);
	gfx_createTextObject(TS_CHRIS_MINES_KILLED, string, 0, 0, FONT_WHITE);

	sprintf(string, "Slaves Rescued : %d", game.slavesRescued);
	gfx_createTextObject(TS_CHRIS_SLAVES_RESCUED, string, 0, 0, FONT_WHITE);

	sprintf(string, "Cargo Picked Up : %d", game.cargoPickups);
	gfx_createTextObject(TS_CHRIS_CARGO_PICKUPS, string, 0, 0, FONT_WHITE);

	if (game.hasWingMate1)
	{
		gfx_createTextObject(TS_PHOEBE_HEADER, "*** Phoebe ***", 0, 0, FONT_WHITE);

		sprintf(string, "Enemies Killed : %d", game.wingMate1Kills);
		gfx_createTextObject(TS_PHOEBE_KILLS, string, 0, 0, FONT_WHITE);

		sprintf(string, "Ejections : %d", game.wingMate1Ejects);
		gfx_createTextObject(TS_PHOEBE_DEATHS, string, 0, 0, FONT_WHITE);
	}

	if (game.hasWingMate2)
	{
		gfx_createTextObject(TS_URSULA_HEADER, "*** Ursula ***", 0, 0, FONT_WHITE);

		sprintf(string, "Enemies Killed : %d", game.wingMate2Kills);
		gfx_createTextObject(TS_URSULA_KILLS, string, 0, 0, FONT_WHITE);

		sprintf(string, "Ejections : %d", game.wingMate2Ejects);
		gfx_createTextObject(TS_URSULA_DEATHS, string, 0, 0, FONT_WHITE);
	}

	gfx_createTextObject(TS_STATUS_HEADER, "Current Status", -1, 83, FONT_WHITE);

	snprintf(string, sizeof string, "Total Time : %2d:%02d:%02d", timeTaken / 3600, (timeTaken / 60) % 60, timeTaken % 60);
	gfx_createTextObject(TS_STATUS_FOOTER, string, -1, 404, FONT_WHITE);

	y = 400;
	for (int i = TS_STATUS_HEADER + 1 ; i < TS_STATUS_FOOTER ; i++)
	{
		y += 20;
		if ((i == TS_CHRIS_HEADER) || (i == TS_PHOEBE_HEADER) ||
				(i == TS_URSULA_HEADER))
			y += 25;

		gfx_textSprites[i].x = 150;
		gfx_textSprites[i].y = y;
	}
}

/*
Sets the names and stats of the planets within the current system.
*/
static void intermission_setSystemPlanets()
{
	FILE *fp;

	char string[100];
	strcpy(string, "");

	switch (game.system)
	{
		case SYSTEM_SPIRIT:
			strcpy(string, "data/planets_spirit.dat");
			break;
		case SYSTEM_EYANANTH:
			strcpy(string, "data/planets_eyananth.dat");
			break;
		case SYSTEM_MORDOR:
			strcpy(string, "data/planets_mordor.dat");
			break;
		case SYSTEM_SOL:
			strcpy(string, "data/planets_sol.dat");
			break;
	}

	fp = fopen(string, "rb");

	int distance;
	char name[50];
	int image;

	for (int i = 0 ; i < 10 ; i++)
	{
		if (fscanf(fp, "%d %s %d", &distance, name, &image) < 3)
		{
			printf("Warning: Data for planet %i in \"%s\" is not correctly formatted\n", i, string);
			break;
		}

		systemPlanet[i].y = distance;
		strcpy(systemPlanet[i].name, name);
		systemPlanet[i].image = gfx_sprites[image];
	}

 	int messageMission;
	int messageSlot;
	char face[50];
	char from[100];
	char subject[100];

	for (int i = 0 ; i < 10 ; i++)
	{
		if ((fscanf(fp, "%d %d %s%*c", &messageMission, &messageSlot, face) < 3) ||
			(fscanf(fp, "%[^\n]%*c", from) < 1) ||
			(fscanf(fp, "%[^\n]%*c", subject) < 1))
		{
			printf("Warning: Mission data for planet %i in \"%s\" is not correctly formatted\n", i, string);
			break;
		}

		systemPlanet[i].messageMission = messageMission;
		systemPlanet[i].messageSlot = messageSlot;
		systemPlanet[i].faceImage = getFace(face);

		strcpy(systemPlanet[i].from, from);
		strcpy(systemPlanet[i].subject, subject);
	}

	fclose(fp);
}

/*
Spins the planets around the sun, spaced according to their Y value
as defined in intermission_setSystemPlanets(). Moving the cursor over the planet
will show their name and their current status
*/
static bool intermission_showSystem(float x, float y, bool selectable)
{
	SDL_Rect r;
	int planet = 0;
	int planetSpace = systemPlanet[planet].y;
	bool rtn = false;

	// Blit the sun
	screen_blit(gfx_sprites[SP_SUN], 370, 220);

	for (int i = 50 ; i < 300 ; i+= planetSpace)
	{
		x *= 0.75;
		y *= 0.75;

		gfx_drawCircle(400, 250, i, screen, darkGrey);

		r.x = int(400 + (sinf(x) * i));
		r.y = int(250 + (cosf(y) * i));
		r.w = 10;
		r.h = 10;

		r.x -= (systemPlanet[planet].image->w / 2);
		r.y -= (systemPlanet[planet].image->h / 2);
		screen_blit(systemPlanet[planet].image, r.x, r.y);

		if (selectable &&
			game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6,
				r.x, r.y, systemPlanet[planet].image->w,
				systemPlanet[planet].image->h))
		{
			screen_renderString(systemPlanet[planet].name, -1, 545, FONT_WHITE);
			if ((engine.keyState[KEY_FIRE]))
			{
				if (game.system == SYSTEM_SPIRIT)
				{
					game.stationedPlanet = planet;
					game.destinationPlanet = planet;
					game.area = systemPlanet[game.stationedPlanet].missionNumber;
					strcpy(game.stationedName, systemPlanet[game.stationedPlanet].name);
					saveGame(0);
				}
				else
				{
					game.destinationPlanet = planet;
					strcpy(game.destinationName, systemPlanet[game.destinationPlanet].name);
				}

				rtn = true;
				engine.keyState[KEY_FIRE] = 0;
			}
		}

		planet++;
		if (systemPlanet[planet].y == -1)
			break;
		planetSpace = systemPlanet[planet].y;
	}

	return rtn;
}

/*
Scrolls the player's current information up the screen. When
the specified status line reaches a certain Y value, the entire
list is reset and the information lines begin again from the bottom
(in other words, they loop around).
*/
static void intermission_showStatus(SDL_Surface *infoSurface)
{
	int y;
	float speed = 0.25;

	if(engine.keyState[KEY_DOWN])
		speed = 1;
	else if(engine.keyState[KEY_UP])
		speed = -1;

	screen_blit(infoSurface, 100, 80);

	for (int i = TS_STATUS_HEADER + 1 ; i < TS_STATUS_FOOTER ; i++)
	{
		gfx_textSprites[i].y -= speed;
		if ((gfx_textSprites[i].y > 80) && (gfx_textSprites[i].y < 400))
			screen_blitText(i);
	}

	if (gfx_textSprites[TS_STATUS_FOOTER - 1].y < 65)
	{
		y = 400;
		for (int i = TS_STATUS_HEADER + 1 ; i < TS_STATUS_FOOTER ; i++)
		{
			y += 20;
			if ((i == TS_CHRIS_HEADER) || (i == TS_PHOEBE_HEADER) ||
					(i == TS_URSULA_HEADER))
				y += 25;

			gfx_textSprites[i].y = y;
		}
	}

	screen_drawRect(100, 80, 600, 20, 0x00, 0x00, 0x99);

	screen_drawRect(100, 400, 600, 20, 0x00, 0x00, 0x99);

	screen_blitText(TS_STATUS_HEADER);
	screen_blitText(TS_STATUS_FOOTER);
}

static void intermission_updateCommsSurface(SDL_Surface *comms)
{
	if (engine.commsSection == 1)
		return;

	char string[255];

	gfx_drawRect(comms, 0, 10, comms->w - 1, 55, 0x00, 0x22, 0x00);
	gfx_blit(gfx_faceSprites[FS_CHRIS], 20, 15, comms);
	gfx_renderString("Chris Bainfield", 80, 15, FONT_WHITE, 0, comms);
	sprintf(string, "Current Location: %s", systemPlanet[game.stationedPlanet].name);
	gfx_renderString(string, 80, 35, FONT_WHITE, 0, comms);
}

static void intermission_createCommsSurface(SDL_Surface *comms)
{
	engine.commsSection = 0;

	gfx_drawRect(comms, 0, 0, comms->w - 1, comms->h - 1, 0x00, 0x00, 0x25);

	gfx_renderString("+++ RECEIVED MESSAGES +++", 115, 80, FONT_GREEN, 0, comms);

	int yOffset;

	for (int i = 0 ; i < 10 ; i++)
	{
		if ((systemPlanet[i].messageSlot != -1) && (systemPlanet[i].missionCompleted == 0))
		{
			yOffset = systemPlanet[i].messageSlot * 60;
			gfx_drawRect(comms, 0, 105 + yOffset, comms->w - 1, 55, 0x00, 0x00, 0x77);
			gfx_blit(gfx_faceSprites[systemPlanet[i].faceImage], 20, 110 + yOffset, comms);
			gfx_renderString(systemPlanet[i].from, 80, 110 + yOffset, FONT_WHITE, 0, comms);
			gfx_renderString(systemPlanet[i].subject, 80, 130 + yOffset, FONT_CYAN, 0, comms);
			gfx_renderString("INCOMPLETE", 350, 110 + yOffset, FONT_RED, 0, comms);
		}
	}

	intermission_updateCommsSurface(comms);
}

static int intermission_renderDialog(SDL_Surface *comms, int y, int face, const char *string)
{
	int newY;
	gfx_blit(gfx_faceSprites[face], 10, y, comms);
	newY = gfx_renderString(string, 80, y, FONT_WHITE, 1, comms) + 25;
	if (newY < y + 60)
		newY += (60 - (newY - y));
	return newY;
}

static void intermission_createMissionDetailSurface(SDL_Surface *comms, int missionSlot)
{
	char name[50];
	char string[2000];
	int y = 50;
	int mission = -1;

	// XXX: Magic number
	for (int i = 0 ; i < 10 ; i++)
	{
		if ((systemPlanet[i].messageSlot == missionSlot) && (systemPlanet[i].missionCompleted == 0))
		{
			mission = systemPlanet[i].messageMission;
		}
	}

	if (mission == -1)
		return;

	gfx_drawRect(comms, 0, 0, comms->w - 1, comms->h - 1, 0x00, 0x00, 0x25);

	// XXX: The "name" should correspond with whatever name was presented in
	// the screen listing all of the comms. For some reason, this has always
	// been defined twice, which is redundant and has led to inconsistencies in
	// the past.
	switch (mission)
	{
		case MISN_HAIL:
			strcpy(name, "Krass Tyler");

			strcpy(string, "Hey, boy! You still owe me money for the Firefly I stole for you! But instead, I want you to go to the WEAPCO training ground and destroy all the craft there.");
			y = intermission_renderDialog(comms, y, FS_KRASS, string);

			strcpy(string, "Oh? That's the job I contracted you to do, was it not?");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "I know, but this way we can resolve your debt right now. Do this job, and also collect $500, and we will call it quits. And if you die... well, I guess the ship was not worth stealing! HA HA HA!");
			y = intermission_renderDialog(comms, y, FS_KRASS, string);

			strcpy(string, "As usual, you take me too lightly, Krass.");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_CERADSE:
			strcpy(name, "Sid Wilson");

			strcpy(string, "Hey, Sid, what's up?");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "Chris, I've intercepted a communication from WEAPCO. Seems they're transporting some medical supplies around Ceradse. We need to get hold of those pods to save some lives!");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "How many do we need?");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "All six, Chris! If you lose even a single one, thousands of people could perish in Spirit within the next few months.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_HINSTAG:
			strcpy(name, "Sid Wilson");

			strcpy(string, "Wow! Missile boats?");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "Yup. Looks like WEAPCO is starting to take notice of your actions.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "Sounds like fun! This will really put the Firefly's fighting ability to the test!");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "Please be careful, Chris. A single missile boat carries enough rockets to level most major cities. Try not to face them head-on, and keep your distance.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_JOLDAR:
			strcpy(name, "Sid Wilson");

			strcpy(string, "We're going to have to get rid of the mine deployment unit around Joldar. The minefield is stopping interplanetary traffic.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "Are any fighters around to keep me entertained?");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "Not at the moment, but that doesn't mean they won't turn up. Be very careful of those mines! They'll only explode when they encounter a ship that's not transmitting a WEAPCO signal. Shoot them down if they get in your way.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_MOEBO:
			strcpy(name, "Sid Wilson");

			strcpy(string, "We've got a major problem here! WEAPCO has decided to stop our resistance by destroying Spirit! The explosion will incinerate everything in the system! You've got to destroy that frigate before it gets in range!");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "Damn! I'll get right on it, then!");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "We're all counting on you, Chris! But just remember - They didn't call that thing \"Star Killer\" just because it sounded nice!");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_RESCUESLAVES:
			strcpy(name, "Sid Wilson");

			strcpy(string, "As you know, WEAPCO has many slaves in this system. If we free a large number of them, it might help to spark a rebellion. I estimate that we will need to rescue around 250 to make a difference.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "Most of the slaves are working in ore mines, aren't they?");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "Yes, but attacking the ore mines directly would be dangerous. You'd be better off intercepting slave transports. What you'll have to do is fly around and see if you can intercept a WEAPCO patrol. Of course, they might not be escorting any slave units, so be careful!");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_NEROD:
			strcpy(name, "Phoebe Lexx");

			strcpy(string, "Help! This is an SOS! Can anyone hear me?!");
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			strcpy(string, "I'm hearing you loud and clear! What's up?");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "Oh, thank God! I was intercepted by a large WEAPCO force near Nerod! I'm in need of assistance!");
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			strcpy(string, "I'm on my way!");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_ALLEZ:
			strcpy(name, "Sid Wilson");

			strcpy(string, "I've just received another SOS. This one is coming from a supply craft carrying essential medical supplies.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "Alright, Tell 'em I'm on my way.");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_URUSOR:
			strcpy(name, "Sid Wilson");

			strcpy(string, "I need some resources before we leave, it'll make life a lot easier in Mordor. Problem is that WEAPCO hoards these parts.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "Where can we get them, then?");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "There's a big shipment of them nearby. I can disable the supply craft carrying them; I just need you to give me some cover while I do it.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "You got it!");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_DORIM:
			strcpy(name, "Sid Wilson");

			strcpy(string, "A WEAPCO scientist just ran off in an escape pod and hid in the asteroid belt. If we capture him, we may be able to get some information about Mordor.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "I'm on it.");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_ELAMALE:
			strcpy(name, "Phoebe Lexx");

			strcpy(string, "I've received word that the slaves we rescued have started a rebellion. Looks like the plan worked.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "WEAPCO has an automated mining ship in orbit around Elamale. How about we take it out and cause some confusion?");
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			strcpy(string, "I like that idea!");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "It'll work, but be careful.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_CLOAKFIGHTER:
			strcpy(name, "Sid Wilson");

			strcpy(string, "What have you found out about that experimental fighter?");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "It's got some kind of cloaking device that makes it invisible to radar. Could prove hard to track down.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "I'll just have to run around the system until I run into it.");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "It's likely to run away if you engage it in battle, so try and do as much damage to it as possible.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_ODEON:
			strcpy(name, "Phoebe Lexx");

			strcpy(string, "I've located my sister's ship currently in orbit around Odeon. She's ignoring my hails though.");
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			strcpy(string, "Something's off here. She seems to be travelling freely with a WEAPCO group.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "Do you think she's turned traitor?");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "No way. She hates WEAPCO with a passion.");
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			strcpy(string, "She must be under some kind of mind control. I've heard of WEAPCO developing a new \"AI training program\" recently. We'd better rescue her!");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_FELLON:
			strcpy(name, "Sid Wilson");

			strcpy(string, "A rebel group has organized a counter strike. If we can help them secure a victory it will be a real boost to morale.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "Awesome! I'm on it!");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_SIVEDI:
			strcpy(name, "Sid Wilson");

			strcpy(string, "Seems like taking out that WEAPCO mining ship wasn't such a good idea. The Ore it collected from those asteroids is needed in weapon production.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "Damn! I guess that means I'll have to mine some myself, then, huh?");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "Yes. Be careful, Chris. Your weapons weren't designed for that sort of work, after all.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_ALMARTHA:
			strcpy(name, "Krass Tyler");

			strcpy(string, "Hey, Krass! I need you to help us out with something. Phoebe and Ursula are taking out key WEAPCO plants. Can you help me create a diversion by wreaking havoc a little bit away from that?");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "Sure, I can help you out, boy. But I'll be needing my fee...");
			y = intermission_renderDialog(comms, y, FS_KRASS, string);

			break;

		case MISN_POSWIC:
			strcpy(name, "Ursula Lexx");

			strcpy(string, "I've remembered something. WEAPCO is transporting some high level personnel to Poswic.");
			y = intermission_renderDialog(comms, y, FS_URSULA, string);

			strcpy(string, "This could be really important to our success in Sol. I'll need some cover so I can disable that ship.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "You got it!");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_ELLESH:
			strcpy(name, "Phoebe Lexx");

			strcpy(string, "Phoebe, I need you to keep an eye on things here. I'm going after that ship!");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "Are you sure you can catch up to it?");
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			strcpy(string, "Is that a challenge? Heh heh, don't worry about it. One thing that's really nice about the Firefly is its speed. I'll see you in a bit.");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_PLUTO:
			strcpy(name, "Sid Wilson");

			strcpy(string, "We've got to start from the outside and work our way in. That will give us less chance of being flanked during the final operation.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "Gotcha.");
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			strcpy(string, "Okay.");
			y = intermission_renderDialog(comms, y, FS_URSULA, string);

			strcpy(string, "Alright.");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_NEPTUNE:
			strcpy(name, "Sid Wilson");

			strcpy(string, "We've got to start from the outside and work our way in. That will give us less chance of being flanked during the final operation.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "Gotcha.");
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			strcpy(string, "Okay.");
			y = intermission_renderDialog(comms, y, FS_URSULA, string);

			strcpy(string, "Alright.");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_URANUS:
			strcpy(name, "Sid Wilson");

			strcpy(string, "We've got to start from the outside and work our way in. That will give us less chance of being flanked during the final operation.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "Gotcha.");
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			strcpy(string, "Okay.");
			y = intermission_renderDialog(comms, y, FS_URSULA, string);

			strcpy(string, "Alright.");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_SATURN:
			strcpy(name, "Sid Wilson");

			strcpy(string, "WEAPCO has set up a highly dangerous defense line between Saturn and Uranus. We'll need to take it out.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "What kind of defense system?");
			y = intermission_renderDialog(comms, y, FS_URSULA, string);

			strcpy(string, "Several mobile Energy Ray cannons, not unlike the weapon used by the Star Killer back in Spirit.");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "Best check my ejection system, then!");
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			break;

		case MISN_JUPITER:
			strcpy(name, "Sid Wilson");

			strcpy(string, "While you were gone I picked up a distress call coming from around Jupiter.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "Who would be sending out a distress call within Sol?");
			y = intermission_renderDialog(comms, y, FS_URSULA, string);

			strcpy(string, "Let's check it out. Even if it's a trap, I think we can handle it.");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_MARS:
			strcpy(name, "Sid Wilson");

			strcpy(string, "Chris, we've got a small problem. WEAPCO has a minefield in the asteroid belt. We'll need you to clear a way through.");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "Alright. I'll radio in once I've cleared a safe path.");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_EARTH:
			strcpy(name, "Everyone");

			strcpy(string, "Okay people, this is the big one. We go in fast and we go in hard. Don't hold back and hit them with everything we've got!");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "We've come too far to turn back now. None of us better die out there!");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			strcpy(string, "Right with you, Chris!");
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			strcpy(string, "WEAPCO'll regret sticking probes into my head!");
			y = intermission_renderDialog(comms, y, FS_URSULA, string);

			break;

		case MISN_VENUS:
			strcpy(name, "Sid Wilson");

			strcpy(string, "Kethlan has run off to Venus. I'm going after him.");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "Be careful, Chris. We've won the war, but it would be a real shame if you died now!");
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		default:
			strcpy(name, "Nobody");
			strcpy(string, "Hey, why am I talking to myself? This shouldn't happen! Clearly, this must be a bug.");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			sprintf(string, "I should go to starfighter.nongnu.org and report this bug there. In that report, I should mention that the mission number is %d.", mission);
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "Wait, what am I still talking into empty space for? It's not like anyone can hear me...");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);
	}

	sprintf(string, "+++ Communication with %s +++", name);
	gfx_renderString(string, -1, 20, FONT_GREEN, 0, comms);

	gfx_drawRect(comms, 5, comms->h - 28, 180, 20, 0x25, 0x00, 0x00);
	gfx_renderString("RETURN TO MESSAGES", 15, comms->h - 25, FONT_WHITE, 1, comms);

	engine.commsSection = 1;
}

static void intermission_doComms(SDL_Surface *comms)
{
	if ((engine.keyState[KEY_FIRE]))
	{
		if (engine.commsSection == 0)
		{
			for (int i = 0 ; i < 4 ; i++)
			{
				if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 170, 180 + (i * 60), 430, 50))
				{
					intermission_createMissionDetailSurface(comms, i);
					engine.keyState[KEY_FIRE] = 0;
				}
			}
		}
		else
		{
			if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 170, 440, 160, 20))
			{
				intermission_createCommsSurface(comms);
				engine.keyState[KEY_FIRE] = 0;
			}
		}
	}
}

static void intermission_createOptions(SDL_Surface *optionsSurface)
{
	SDL_FillRect(optionsSurface, NULL, black);

	gfx_drawRect(optionsSurface, 0, 0, optionsSurface->w - 2, optionsSurface->h - 2, 0x00, 0x00, 0x44);

	gfx_renderString("++ OPTIONS ++", 105, 8, FONT_WHITE, 0, optionsSurface);

	gfx_drawRect(optionsSurface, 190, 45, 50, 22, 0x00, 0x00, 0x00);
	gfx_drawRect(optionsSurface, 250, 45, 50, 22, 0x00, 0x00, 0x00);
	gfx_drawRect(optionsSurface, 20, 45, 150, 22, 0x00, 0x00, 0x00);
	if (engine.useSound)
		gfx_drawRect(optionsSurface, 190, 45, 50, 22, 0xff, 0x00, 0x00);
	else
		gfx_drawRect(optionsSurface, 250, 45, 50, 22, 0xff, 0x00, 0x00);
	gfx_renderString("ON", 207, 50, FONT_WHITE, 0, optionsSurface);
	gfx_renderString("OFF", 263, 50, FONT_WHITE, 0, optionsSurface);
	gfx_renderString("SOUND", 30, 50, FONT_WHITE, 0, optionsSurface);

	gfx_drawRect(optionsSurface, 190, 95, 50, 22, 0x00, 0x00, 0x00);
	gfx_drawRect(optionsSurface, 250, 95, 50, 22, 0x00, 0x00, 0x00);
	gfx_drawRect(optionsSurface, 20, 95, 150, 22, 0x00, 0x00, 0x00);
	if (engine.useMusic)
		gfx_drawRect(optionsSurface, 190, 95, 50, 22, 0xff, 0x00, 0x00);
	else
		gfx_drawRect(optionsSurface, 250, 95, 50, 22, 0xff, 0x00, 0x00);
	gfx_renderString("ON", 207, 100, FONT_WHITE, 0, optionsSurface);
	gfx_renderString("OFF", 263, 100, FONT_WHITE, 0, optionsSurface);
	gfx_renderString("MUSIC", 30, 100, FONT_WHITE, 0, optionsSurface);

 	gfx_drawRect(optionsSurface, 190, 145, 50, 22, 0x00, 0x00, 0x00);
	gfx_drawRect(optionsSurface, 250, 145, 50, 22, 0x00, 0x00, 0x00);
	gfx_drawRect(optionsSurface, 20, 145, 150, 22, 0x00, 0x00, 0x00);
	if (engine.fullScreen)
		gfx_drawRect(optionsSurface, 190, 145, 50, 22, 0xff, 0x00, 0x00);
	else
		gfx_drawRect(optionsSurface, 250, 145, 50, 22, 0xff, 0x00, 0x00);
	gfx_renderString("ON", 207, 150, FONT_WHITE, 0, optionsSurface);
	gfx_renderString("OFF", 263, 150, FONT_WHITE, 0, optionsSurface);
	gfx_renderString("FULLSCREEN", 30, 150, FONT_WHITE, 0, optionsSurface);
}

static void intermission_doOptions(SDL_Surface *optionsSurface)
{
	if ((engine.keyState[KEY_FIRE]))
	{
		if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 417, 172, 45, 22))
			engine.useSound = true;

		if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 478, 172, 45, 22))
			engine.useSound = false;


		if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 417, 222, 45, 22))
		{
			engine.useMusic = true;
			audio_playMusic("music/through_space.ogg", -1);
		}

		if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 478, 222, 45, 22))
		{
			engine.useMusic = false;
			audio_haltMusic();
		}

		if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 417, 272, 45, 22))
		{
			if (!engine.fullScreen)
			{
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
				engine.fullScreen = true;
			}
		}

		if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 478, 272, 45, 22))
		{
			if (engine.fullScreen)
			{
				SDL_SetWindowFullscreen(window, 0);
				engine.fullScreen = false;
			}
		}

		intermission_createOptions(optionsSurface);
	}
}

/*
Controls the entire intermission screen. This simply draws a background,
stars, gridlines and the icons at the bottom of the screen. Will call
(and continue to call) the specified functions when the player has
selected an icon.
*/
int intermission()
{
	int iconInfoY;

	char string[25];

	SDL_Rect r;
	SDL_Rect destRect;
	int distance = 0;
	double interceptionChance;

	int section = 1;

	float sinX = 300;
	float cosY = 300;
	bool movePlanets = true;
	int saveSlot = -1;

	int rtn = 0;

	bool redrawBackground = true;

	gfx_free();

	checkForBossMission(); // double check just to make sure!

	// Tell the game we are not in a mission so
	// do not perform certain keyboard actions
	engine.gameSection = SECTION_INTERMISSION;

	screen_clear(black);
	renderer_update();
	screen_clear(black);

	initSaveSlots();

	loadBackground(systemBackground[game.system]);

	engine.cursor_x = screen->w / 2;
	engine.cursor_y = screen->h / 2;
	gfx_sprites[SP_CURSOR] = gfx_loadImage("gfx/cursor.png");

	// Icons
	gfx_sprites[SP_START_MISSION] = gfx_loadImage("gfx/icon1.png");
	gfx_sprites[SP_MAP] = gfx_loadImage("gfx/icon2.png");
	gfx_sprites[SP_STATUS] = gfx_loadImage("gfx/icon3.png");
	gfx_sprites[SP_SAVE] = gfx_loadImage("gfx/icon4.png");
	gfx_sprites[SP_SHOP] = gfx_loadImage("gfx/icon5.png");
	gfx_sprites[SP_COMM] = gfx_loadImage("gfx/icon6.png");
	gfx_sprites[SP_OPTIONS] = gfx_loadImage("gfx/icon7.png");
	gfx_sprites[SP_EXIT] = gfx_loadImage("gfx/icon8.png");
	gfx_sprites[SP_PLASMA_MAX_OUTPUT] = gfx_loadImage("gfx/icon9.png");
	gfx_sprites[SP_PLASMA_MAX_POWER] = gfx_loadImage("gfx/icon10.png");
	gfx_sprites[SP_PLASMA_MAX_RATE] = gfx_loadImage("gfx/icon11.png");
	gfx_sprites[SP_PLASMA_AMMO] = gfx_loadImage("gfx/icon12.png");
	gfx_sprites[SP_ROCKET_AMMO] = gfx_loadImage("gfx/icon13.png");
	gfx_sprites[SP_PLASMA_MIN_OUTPUT] = gfx_loadImage("gfx/icon14.png");
	gfx_sprites[SP_PLASMA_MIN_POWER] = gfx_loadImage("gfx/icon15.png");
	gfx_sprites[SP_PLASMA_MIN_RATE] = gfx_loadImage("gfx/icon16.png");
	gfx_sprites[SP_PLASMA_MAX_AMMO] = gfx_loadImage("gfx/icon17.png");
	gfx_sprites[SP_ROCKET_MAX_AMMO] = gfx_loadImage("gfx/icon18.png");
	gfx_sprites[SP_DOUBLE_ROCKETS] = gfx_loadImage("gfx/icon19.png");
	gfx_sprites[SP_MICRO_ROCKETS] = gfx_loadImage("gfx/icon20.png");
	gfx_sprites[SP_LASER] = gfx_loadImage("gfx/icon21.png");
	gfx_sprites[SP_HOMING_MISSILE] = gfx_loadImage("gfx/icon22.png");
	gfx_sprites[SP_CHARGER] = gfx_loadImage("gfx/icon23.png");
	gfx_sprites[SP_DOUBLE_HOMING_MISSILES] = gfx_loadImage("gfx/icon24.png");
	gfx_sprites[SP_MICRO_HOMING_MISSILES] = gfx_loadImage("gfx/icon25.png");
	gfx_sprites[SP_GOTO] = gfx_loadImage("gfx/icon26.png");
	gfx_sprites[SP_BUY] = gfx_loadImage("gfx/buyIcon.png");
	gfx_sprites[SP_SELL] = gfx_loadImage("gfx/sellIcon.png");
	gfx_sprites[SP_FIREFLY] = gfx_loadImage("gfx/firefly1.png");

	// Planets 30 - 39
	gfx_sprites[SP_SUN] = gfx_loadImage("gfx/planet_sun.png");
	gfx_sprites[SP_PLANET_GREEN] = gfx_loadImage("gfx/planet_green.png");
	gfx_sprites[SP_PLANET_BLUE] = gfx_loadImage("gfx/planet_blue.png");
	gfx_sprites[SP_PLANET_RED] = gfx_loadImage("gfx/planet_red.png");
	gfx_sprites[SP_PLANET_ORANGE] = gfx_loadImage("gfx/planet_orange.png");

	// Faces
	gfx_faceSprites[FS_CHRIS] = gfx_loadImage("gfx/face_chris.png");
	gfx_faceSprites[FS_SID] = gfx_loadImage("gfx/face_sid.png");
	gfx_faceSprites[FS_KRASS] = gfx_loadImage("gfx/face_krass.png");
	gfx_faceSprites[FS_PHOEBE] = gfx_loadImage("gfx/face_phoebe.png");
	gfx_faceSprites[FS_URSULA] = gfx_loadImage("gfx/face_ursula.png");
	gfx_faceSprites[FS_KLINE] = gfx_loadImage("gfx/face_kline.png");
	gfx_faceSprites[FS_CREW] = gfx_loadImage("gfx/face_crew.png");

	engine.done = 0;
	engine.keyState[KEY_FIRE] = 0;
	engine.ssx = 0;
	engine.ssy = 0;

	intermission_setStatusLines();
	initShop();
	intermission_setSystemPlanets();

	SDL_Surface *statsSurface = gfx_createAlphaRect(600, 330, 0x00, 0x00, 0x99);
	SDL_Surface *savesSurface = gfx_createSurface(350, 300);
	SDL_Surface *optionsSurface = gfx_createSurface(320, 240);
	SDL_Surface *commsSurface = gfx_createSurface(450, 400);

	createSavesSurface(savesSurface, -1);
	intermission_createOptions(optionsSurface);
	intermission_createCommsSurface(commsSurface);

	// Remove the Supercharge, if it is there
	if ((game.difficulty != DIFFICULTY_EASY) &&
		(game.difficulty != DIFFICULTY_ORIGINAL))
	{
		weapon[W_PLAYER_WEAPON].reload[0] = MAX(
			weapon[W_PLAYER_WEAPON].reload[0],
			rate2reload[game.maxPlasmaRate]);
		weapon[W_PLAYER_WEAPON].ammo[0] = MIN(weapon[W_PLAYER_WEAPON].ammo[0],
			game.maxPlasmaOutput);
		weapon[W_PLAYER_WEAPON].damage = MIN(weapon[W_PLAYER_WEAPON].damage,
			game.maxPlasmaDamage);
	}

	switch (game.system)
	{
		case SYSTEM_EYANANTH:
			interceptionChance = 1. / 300.;
			break;
		case SYSTEM_MORDOR:
			interceptionChance = 1. / 150.;
			break;
		case SYSTEM_SOL:
			// There is no chance of being interceptted after the final attack on Earth
			if ((game.system == SYSTEM_SOL) && (systemPlanet[2].missionCompleted))
				interceptionChance = 0;
			else
				interceptionChance = 1. / 100.;
			break;
		default:
			interceptionChance = 0;
	}

	if ((engine.useAudio) && (engine.useMusic))
		audio_playMusic("music/through_space.ogg", -1);

	sprintf(string, "System : %s", systemNames[game.system]);
	gfx_createTextObject(TS_CURRENT_SYSTEM, string, -1, 15, FONT_WHITE);

	iconInfoY = 545;

	gfx_createTextObject(TS_INFO_START_MISSION, "Start Next Mission", -1, iconInfoY, FONT_WHITE);
	gfx_createTextObject(TS_INFO_GOTO, "Go to Destination Planet", -1, iconInfoY, FONT_WHITE);
	gfx_createTextObject(TS_INFO_MAP, "View System Map", -1, iconInfoY, FONT_WHITE);
	gfx_createTextObject(TS_INFO_STATUS, "Current Status", -1, iconInfoY, FONT_WHITE);
	gfx_createTextObject(TS_INFO_SAVE_GAME, "Save Game", -1, iconInfoY, FONT_WHITE);
	gfx_createTextObject(TS_INFO_SHOP, "Upgrade FIREFLY", -1, iconInfoY, FONT_WHITE);
	gfx_createTextObject(TS_INFO_COMMS, "Comms", -1, iconInfoY, FONT_WHITE);
	gfx_createTextObject(TS_INFO_OPTIONS, "Options", -1, iconInfoY, FONT_WHITE);
	gfx_createTextObject(TS_INFO_EXIT, "Exit to Title Screen", -1, iconInfoY, FONT_WHITE);

	sprintf(string, "Stationed At: %s", systemPlanet[game.stationedPlanet].name);
	gfx_createTextObject(TS_CURRENT_PLANET, string, 90, 450, FONT_WHITE);

	if (game.destinationPlanet > -1)
		sprintf(string, "Destination: %s", systemPlanet[game.destinationPlanet].name);
	else
		strcpy(string, "Destination: None");
	gfx_createTextObject(TS_DEST_PLANET, string, 550, 450, FONT_WHITE);

	if (game.distanceCovered > 0)
		section = 0;
	else
		player.shield = player.maxShield;

	flushInput();
	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;
	engine.done = 0;

	while (!engine.done)
	{
		renderer_update();

		if (redrawBackground)
		{
			screen_drawBackground();
			redrawBackground = false;
		}
		else
		{
			screen_unBuffer();
		}

  		game_doStars();

		r.x = 0;
		r.y = 0;
		r.h = screen->h;
		r.w = 1;
		for (int i = 40 ; i < screen->w ; i+= 40)
		{
			r.x = i;
			SDL_FillRect(screen, &r, darkerBlue);
		}

		r.x = 0;
		r.y = 0;
		r.h = 1;
		r.w = screen->w;
		for (int i = 40 ; i < screen->h ; i+= 40)
		{
			r.y = i;
			SDL_FillRect(screen, &r, darkerBlue);
		}


		if (rand() % 1000 < 2)
		{
			engine.ssx = RANDRANGE(-100, 100);
			engine.ssy = RANDRANGE(-100, 100);
			engine.ssx /= 100;
			engine.ssy /= 100;
		}

		screen_blitText(TS_CURRENT_SYSTEM);

		switch(section)
		{
			case 0:
				if (game.stationedPlanet == game.destinationPlanet)
				{
					game.area = systemPlanet[game.stationedPlanet].missionNumber;
					rtn = 2;
					engine.done = 1;
				}
				else
				{
					distance = abs(game.stationedPlanet - game.destinationPlanet);
					distance = (5 / distance);
					if (distance < 1)
						distance = 1;

					gfx_createTextObject(TS_CURRENT_PLANET, systemPlanet[game.stationedPlanet].name,
						135, 480, FONT_WHITE);
					gfx_createTextObject(TS_DEST_PLANET, systemPlanet[game.destinationPlanet].name,
						635, 480, FONT_WHITE);

					section = 8;

					destRect.x = 180;
					destRect.y = 450;
					destRect.w = 1;
					if (game.distanceCovered > 0)
						destRect.w = game.distanceCovered;
					destRect.h = 20;
				}
				break;

			case 1:
				if (engine.keyState[KEY_ALTFIRE])
				{
					movePlanets = !movePlanets;
					engine.keyState[KEY_ALTFIRE] = 0;
				}

				if (movePlanets)
				{
					sinX += 0.01;
					cosY += 0.01;
				}

				if (intermission_showSystem(sinX, cosY, true))
				{
					if (game.system == SYSTEM_SPIRIT)
					{
						sprintf(string, "Stationed At: %s", systemPlanet[game.stationedPlanet].name);
						gfx_createTextObject(TS_CURRENT_PLANET, string, 90, 450, FONT_WHITE);
						intermission_updateCommsSurface(commsSurface);
					}
					else
					{
						sprintf(string, "Destination: %s", systemPlanet[game.destinationPlanet].name);
						gfx_createTextObject(TS_DEST_PLANET, string, 550, 450, FONT_WHITE);
					}
				}

				screen_blitText(TS_CURRENT_PLANET);
				if ((game.system > SYSTEM_SPIRIT) && (game.stationedPlanet != game.destinationPlanet))
					screen_blitText(TS_DEST_PLANET);
				break;

			case 2:
				intermission_showStatus(statsSurface);
				break;

			case 3:
				screen_blit(savesSurface, 200, 100);
				saveSlot = showSaveSlots(savesSurface, saveSlot);
				break;

			case 4:
				showShop();
				break;

			case 5:
				screen_blit(commsSurface, 170, 70);
				intermission_doComms(commsSurface);
				break;

			case 6:
				screen_blit(optionsSurface, 230, 130);
				intermission_doOptions(optionsSurface);
				break;

			case 7:
				rtn = 0;
				engine.done = 1;
				break;

			case 8:
				intermission_showSystem(sinX, cosY, false);

				screen_blit(systemPlanet[game.stationedPlanet].image, 150, 450);
				screen_blitText(TS_CURRENT_PLANET);
				screen_blit(systemPlanet[game.destinationPlanet].image, 650, 450);
				screen_blitText(TS_DEST_PLANET);

				destRect.w += distance;
				SDL_FillRect(screen, &destRect, red);

				if (destRect.w >= 450)
				{
					game.stationedPlanet = game.destinationPlanet;
					game.distanceCovered = 0;
					player.shield = player.maxShield;
					sprintf(string, "Stationed At: %s",
						systemPlanet[game.stationedPlanet].name);
					strcpy(game.stationedName,
						systemPlanet[game.stationedPlanet].name);
					gfx_createTextObject(TS_CURRENT_PLANET, string, 90, 450, FONT_WHITE);
					intermission_updateCommsSurface(commsSurface);
					section = 1;
					redrawBackground = true;
					saveGame(0);
				}
				else if (interceptionChance > 0)
				{
					if (CHANCE(interceptionChance))
					{
						game.area = MISN_INTERCEPTION;
						rtn = 2;
						engine.done = 1;
						game.distanceCovered = destRect.w;
					}
				}

				break;
		}

		screen_addBuffer(300, 545, 200, 15);

		if (section != 8)
		{
			if ((game.stationedPlanet == game.destinationPlanet) &&
					(!systemPlanet[game.stationedPlanet].missionCompleted))
				screen_blit(gfx_sprites[SP_START_MISSION], 80, 500);
			else if (game.stationedPlanet != game.destinationPlanet)
				screen_blit(gfx_sprites[SP_GOTO], 80, 500);

			screen_blit(gfx_sprites[SP_MAP], 170, 500);
			screen_blit(gfx_sprites[SP_STATUS], 260, 500);
			screen_blit(gfx_sprites[SP_SAVE], 350, 500);
			screen_blit(gfx_sprites[SP_SHOP], 440, 500);
			screen_blit(gfx_sprites[SP_COMM], 530, 500);
			screen_blit(gfx_sprites[SP_OPTIONS], 620, 500);
			screen_blit(gfx_sprites[SP_EXIT], 710, 500);

			if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 80, 500, 32, 32) &&
					((game.stationedPlanet != game.destinationPlanet) ||
						(!systemPlanet[game.stationedPlanet].missionCompleted)))
			{
				if (game.stationedPlanet == game.destinationPlanet)
					screen_blitText(TS_INFO_START_MISSION);
				else
					screen_blitText(TS_INFO_GOTO);

				if ((engine.keyState[KEY_FIRE]))
				{
					redrawBackground = true;
					section = 0;
					engine.keyState[KEY_FIRE] = 0;
				}
			}
			else if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 170, 500, 32, 32))
			{
				screen_blitText(TS_INFO_MAP);

				if ((engine.keyState[KEY_FIRE]))
				{
					redrawBackground = true;
					section = 1;
					engine.keyState[KEY_FIRE] = 0;
				}
			}
			else if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 260, 500, 32, 32))
			{
				screen_blitText(TS_INFO_STATUS);

				if ((engine.keyState[KEY_FIRE]))
				{
					redrawBackground = true;
					section = 2;
					engine.keyState[KEY_FIRE] = 0;
				}
			}
			else if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 350, 500, 32, 32))
			{
				screen_blitText(TS_INFO_SAVE_GAME);

				if ((engine.keyState[KEY_FIRE]))
				{
					redrawBackground = true;
					section = 3;
					engine.keyState[KEY_FIRE] = 0;
				}
			}
			else if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 440, 500, 32, 32))
			{
				screen_blitText(TS_INFO_SHOP);

				if ((engine.keyState[KEY_FIRE]))
				{
					redrawBackground = true;
					section = 4;
					engine.keyState[KEY_FIRE] = 0;
				}
			}
			else if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 530, 500, 32, 32))
			{
				screen_blitText(TS_INFO_COMMS);

				if ((engine.keyState[KEY_FIRE]))
				{
					redrawBackground = true;
					section = 5;
					engine.keyState[KEY_FIRE] = 0;
				}
			}
			else if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 620, 500, 32, 32))
			{
				screen_blitText(TS_INFO_OPTIONS);

				if ((engine.keyState[KEY_FIRE]))
				{
					redrawBackground = true;
					section = 6;
					engine.keyState[KEY_FIRE] = 0;
				}
			}
			else if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 710, 500, 32, 32))
			{
				screen_blitText(TS_INFO_EXIT);

				if ((engine.keyState[KEY_FIRE]))
				{
					redrawBackground = true;
					section = 7;
					engine.keyState[KEY_FIRE] = 0;
				}
			}
		}

		engine.keyState[KEY_FIRE] = 0;
		engine.keyState[KEY_ALTFIRE] = 0;
		intermission_doCursor();

		game_delayFrame();
	}

	audio_haltMusic();
	SDL_FreeSurface(statsSurface);
	SDL_FreeSurface(savesSurface);
	SDL_FreeSurface(optionsSurface);
	SDL_FreeSurface(commsSurface);

	if (game.distanceCovered == 0)
		player.shield = player.maxShield;

	return rtn;
}

