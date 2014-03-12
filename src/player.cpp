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

object player;

/*
Initialises the player for a new game.
*/
void initPlayer()
{
	player.active = true;
	player.x = screen->w / 2;
	player.y = screen->h / 2;
	player.speed = 2;
	player.maxShield = (25 * currentGame.shieldUnits);
	player.systemPower = player.maxShield;
	player.face = 0;

	player.image[0] = shipShape[0];
	player.image[1] = shipShape[1];

	player.engineX = player.image[0]->w;
	player.engineY = (player.image[0]->h / 2);

	player.owner = &player;
	player.flags = FL_FRIEND;

	player.weaponType[0] = W_PLAYER_WEAPON;

	if(weapon[0].ammo[0] < currentGame.minPlasmaOutput)
		weapon[0].ammo[0] = currentGame.minPlasmaOutput;
	if(weapon[0].damage < currentGame.minPlasmaDamage)
		weapon[0].damage = currentGame.minPlasmaDamage;
	if(weapon[0].reload[0] > rate2reload[currentGame.minPlasmaRate])
		weapon[0].reload[0] = rate2reload[currentGame.minPlasmaRate];

	player.hit = 0;

	engine.lowShield = (player.maxShield / 3);
	engine.averageShield = engine.lowShield + engine.lowShield;

	if (player.weaponType[1] == W_CHARGER)
		player.ammo[1] = 0;

	if (player.weaponType[1] == W_LASER)
		player.ammo[1] = 0;
}

void exitPlayer()
{
	if (player.weaponType[1] == W_CHARGER)
		player.ammo[1] = 0;

	if (player.weaponType[1] == W_LASER)
		player.ammo[1] = 0;
}

void doPlayer()
{
	// This causes the motion to slow
	engine.ssx *= 0.99;
	engine.ssy *= 0.99;

	int shapeToUse;

	if (player.shield > -100)
	{
		if (player.shield > 0)
		{
			if ((engine.keyState[SDLK_LCTRL]) || (engine.keyState[SDLK_RCTRL]))
				fireBullet(&player, 0);

			if ((engine.keyState[SDLK_SPACE]) && (player.weaponType[1] != W_NONE))
			{
				if ((player.weaponType[1] != W_CHARGER) && (player.weaponType[1] != W_LASER) && (player.ammo[1] > 0))
				{
					fireBullet(&player, 1);
				}

				if (player.weaponType[1] == W_LASER)
				{
					if (player.ammo[1] < 100)
					{
						fireBullet(&player, 1);
						player.ammo[1] += 2;
						if (player.ammo[1] >= 100)
						{
							player.ammo[1] = 200;
							setInfoLine("Laser Overheat!!", FONT_WHITE);
						}
					}
				}
			}

			if (player.weaponType[1] == W_CHARGER)
			{
				if (engine.keyState[SDLK_SPACE] && !(currentGame.gamePlay == GAMEPLAY_ONPON && (engine.keyState[SDLK_LCTRL] || engine.keyState[SDLK_RCTRL])))
				{
					limitCharAdd(&player.ammo[1], 1, 0, 200);
				}
				else
				{
					if (player.ammo[1] > 0)
						fireBullet(&player, 1);
					player.ammo[1] = 0;
				}
			}

			if ((engine.keyState[SDLK_LSHIFT]) || (engine.keyState[SDLK_RSHIFT]))
			{
				if(weapon[0].ammo[0] >= 3 && weapon[0].ammo[0] <= currentGame.maxPlasmaOutput)
				{
					weapon[0].flags ^= WF_SPREAD;

					if(weapon[0].flags & WF_SPREAD)
					{
						setInfoLine("Weapon set to Spread", FONT_WHITE);
					}
					else
					{
						setInfoLine("Weapon set to Concentrate", FONT_WHITE);
					}
				}

				engine.keyState[SDLK_LSHIFT] = engine.keyState[SDLK_RSHIFT] = 0;
			}

			limitCharAdd(&player.reload[0], -1, 0, 999);
			limitCharAdd(&player.reload[1], -1, 0, 999);

			if (engine.keyState[SDLK_UP])
			{
				player.y -= player.speed;
				engine.ssy += 0.1;
			}

			if (engine.keyState[SDLK_DOWN])
			{
				player.y += player.speed;
				engine.ssy -= 0.1;
			}

			if (engine.keyState[SDLK_LEFT])
			{
				player.x -= player.speed;
				engine.ssx += 0.1;
				player.face = 1;
			}

			if (engine.keyState[SDLK_RIGHT])
			{
				player.x += player.speed;
				engine.ssx -= 0.1;
				player.face = 0;
			}

			if (engine.keyState[SDLK_ESCAPE])
			{
				if ((engine.done == 0) && (engine.gameSection == SECTION_GAME) && (currentMission.remainingObjectives1 == 0))
				{
					playSound(SFX_FLY);
					engine.done = 2;
					engine.missionCompleteTimer = (SDL_GetTicks() - 1);
				}
			}

			if (engine.keyState[SDLK_p] || !(SDL_GetAppState() & SDL_APPINPUTFOCUS))
			{
				engine.paused = true;
				engine.keyState[SDLK_p] = 0;
			}

			if ((engine.keyState[SDLK_t]) && (currentGame.area != 10))
			{
				if (engine.targetArrowTimer == -1 && currentGame.difficulty < DIFFICULTY_HARD) {
					engine.targetArrowTimer = -2;
					setInfoLine("Showing all targets", FONT_WHITE);
				} else if (engine.targetArrowTimer < 0) {
					engine.targetArrowTimer = 0;
					setInfoLine("Disabled target arrows", FONT_WHITE);
				} else {
					engine.targetArrowTimer = -1;
					setInfoLine("Showing mission target", FONT_WHITE);
				}

				engine.keyState[SDLK_t] = 0;
			}

			if ((engine.missionCompleteTimer == 0) && (engine.targetArrowTimer == -1))
			{
				if ((enemy[engine.targetIndex].shield < 1) && (currentGame.area != 10))
				{
					engine.targetIndex = rand() % MAX_ALIENS;
					
					if (enemy[engine.targetIndex].flags & FL_FRIEND)
						engine.targetIndex = 0;
					else
						setTarget(engine.targetIndex);
				}
			}

			if (((currentGame.area == 18) && (enemy[WC_BOSS].shield > 0)) || (currentGame.area == 24))
				player.face = 0;

			if (engine.done == 0)
			{
				limitFloat(&player.x, viewBorder, screen->w - viewBorder);
				limitFloat(&player.y, viewBorder, screen->h - viewBorder);
			}

			if (player.shield > engine.lowShield)
				addEngine(&player);

			shapeToUse = player.face;

			if (player.hit)
				shapeToUse += SHIP_HIT_INDEX;

			limitCharAdd(&player.hit, -1, 0, 100);

			blit(shipShape[shapeToUse], (int)player.x, (int)player.y);
			if ((player.shield <= engine.lowShield) && (rand() % 5 < 1))
				addExplosion(player.x + rrand(-10, 10), player.y + rrand(-10, 20), E_SMOKE);
		}
		else
		{
			player.active = false;
			player.shield--;
			if (player.shield == -1)
			{
				if ((currentGame.hasWingMate1) || (enemy[WC_KLINE].active))
					getPlayerDeathMessage();

				// Make it look like the ships are all still moving...
				if (currentGame.area == 18)
				{
					for (int i = 0 ; i < MAX_ALIENS ; i++)
						enemy[i].flags |= FL_LEAVESECTOR;
				}

				playSound(SFX_DEATH, player.x);
				playSound(SFX_EXPLOSION, player.x);
			}

			engine.keyState[SDLK_UP] = engine.keyState[SDLK_DOWN] = engine.keyState[SDLK_LEFT] = engine.keyState[SDLK_RIGHT] = 0;
			if ((rand() % 3) == 0)
				addExplosion(player.x + rrand(-10, 10), player.y + rrand(-10, 10), E_BIG_EXPLOSION);
			if (player.shield == -99)
				addDebris((int)player.x, (int)player.y, player.maxShield);
		}
	}

	limitFloat(&engine.ssx, -3, 3);
	limitFloat(&engine.ssy, -3, 3);

	// Specific for the mission were you have to chase the Executive Transport
	if ((currentGame.area == 18) && (enemy[WC_BOSS].shield > 0) && (player.shield > 0))
	{
		engine.ssx = -6;
		engine.ssy = 0;
	}
	
	if (currentGame.area == 24)
	{
		engine.ssx = -6;
		engine.ssy = 0;
	}

	player.dx = engine.ssx;
	player.dy = engine.ssy;
}

void flushInput()
{
	for (int i = 0 ; i < 350 ; i++)
		engine.keyState[i] = 0;

	while (SDL_PollEvent(&engine.event)){}
}

void getPlayerInput()
{
	while (SDL_PollEvent(&engine.event))
	{
		switch (engine.event.type)
		{
			case SDL_QUIT:
				exit(0);
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (engine.gameSection == SECTION_INTERMISSION)
				{
					if (engine.event.button.button == SDL_BUTTON_LEFT) engine.keyState[SDLK_LCTRL] = 1;
					if (engine.event.button.button == SDL_BUTTON_RIGHT) engine.keyState[SDLK_SPACE] = 1;
				}
				break;

			case SDL_KEYDOWN:
				if (engine.gameSection == SECTION_TITLE)
					addKeyEvent(SDL_GetKeyName(engine.event.key.keysym.sym));

				engine.keyState[engine.event.key.keysym.sym] = 1;

				if (engine.gameSection != SECTION_GAME)
					engine.paused = false;

				break;

			case SDL_KEYUP:
				if (engine.event.key.keysym.sym != SDLK_p)
					engine.keyState[engine.event.key.keysym.sym] = 0;
				break;

			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
				switch (engine.event.jbutton.button)
				{
					case 0:
						engine.keyState[SDLK_LCTRL] = engine.event.jbutton.state;
						break;
					case 1:
						engine.keyState[SDLK_SPACE] = engine.event.jbutton.state;
						break;
					case 2:
						engine.keyState[SDLK_LSHIFT] = engine.event.jbutton.state;
						break;
					case 3:
						engine.keyState[SDLK_t] = engine.event.jbutton.state;
						break;
				}
				break;

			case SDL_JOYHATMOTION:
				engine.keyState[SDLK_UP]    = engine.event.jhat.value & SDL_HAT_UP;
				engine.keyState[SDLK_DOWN]  = engine.event.jhat.value & SDL_HAT_DOWN;
				engine.keyState[SDLK_LEFT]  = engine.event.jhat.value & SDL_HAT_LEFT;
				engine.keyState[SDLK_RIGHT] = engine.event.jhat.value & SDL_HAT_RIGHT;
				break;

			case SDL_JOYAXISMOTION:
				static bool prevjoyup, prevjoydown, prevjoyleft, prevjoyright;
				if (engine.event.jaxis.axis & 1) {
					bool joyup = engine.event.jaxis.value < -16384;
					bool joydown = engine.event.jaxis.value >= 16384;
					if(joyup != prevjoyup)
						engine.keyState[SDLK_UP] = prevjoyup = joyup;
					if(joydown != prevjoydown)
						engine.keyState[SDLK_DOWN] = prevjoydown = joydown;
				} else {
					bool joyleft = engine.event.jaxis.value < -16384;
					bool joyright = engine.event.jaxis.value >= 16384;
					if(joyleft != prevjoyleft)
						engine.keyState[SDLK_LEFT] = prevjoyleft = joyleft;
					if(joyright != prevjoyright)
						engine.keyState[SDLK_RIGHT] = prevjoyright = joyright;
				}
				break;

			default:
				break;
		}

		if ((engine.keyState[SDLK_F11]) || ((engine.keyState[SDLK_RETURN]) && (engine.keyState[SDLK_LALT])))
		{
			SDL_WM_ToggleFullScreen(screen);
			engine.fullScreen = !engine.fullScreen;
			engine.keyState[SDLK_F11] = engine.keyState[SDLK_LALT] = engine.keyState[SDLK_RETURN] = 0;
		}
	}

	if (engine.gameSection == SECTION_INTERMISSION)
	{
		// Get the current mouse position
		static int px = -1, py = -1;
		int x, y;
		SDL_GetMouseState(&x, &y);
		if (px == x && py == y) {
			if(engine.keyState[SDLK_UP] && engine.cursor_y > 0)
				engine.cursor_y -= 4;
			if(engine.keyState[SDLK_DOWN] && engine.cursor_y < screen->h - 4)
				engine.cursor_y += 4;
			if(engine.keyState[SDLK_LEFT] && engine.cursor_x > 0)
				engine.cursor_x -= 4;
			if(engine.keyState[SDLK_RIGHT] && engine.cursor_x < screen->w - 4)
				engine.cursor_x += 4;
		} else {
			engine.cursor_x = px = x;
			engine.cursor_y = py = y;
		}
	}

}

void leaveSector()
{
	engine.keyState[SDLK_UP] = engine.keyState[SDLK_DOWN] = engine.keyState[SDLK_LEFT] = engine.keyState[SDLK_RIGHT] = 0;
	engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = engine.keyState[SDLK_SPACE] = 0;

	if (engine.done == 0)
		engine.done = 3;

	if (engine.done == 3)
	{
		player.face = 0;
		if (player.x > -100)
		{
			player.x += engine.ssx;
			engine.ssx -= 1;
			if (player.y > screen->h)
				player.y--;
			if (player.y < screen->h)
				player.y++;
		}

		if (player.x <= -100)
		{
			engine.done = 2;
			playSound(SFX_FLY);
		}
	}

	if (engine.done == 2)
	{
		player.face = 0;
		player.x += 12;
		engine.ssx -= 0.2;
		if (player.x > (2 * screen->w))
			engine.done = 1;
	}
}

