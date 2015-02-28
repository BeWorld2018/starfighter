/*
Copyright (C) 2003 Parallel Realities

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
Checked during the main game loop. When the game is paused
it goes into a constant loop checking this routine. If escape is
pressed, the game automatically ends and goes back to the title screen
*/
bool checkPauseRequest()
{
	getPlayerInput();
		
	if (engine.keyState[KEY_ESCAPE])
	{
		engine.paused = false;
		engine.done = 1;
		player.shield = 0;
		return true;
	}
	
	if (engine.keyState[KEY_PAUSE])
	{
		engine.paused = false;
		engine.keyState[KEY_PAUSE] = 0;
	}

	return false;
}
