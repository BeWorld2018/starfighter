/*
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

void addDebris(int x, int y, int amount)
{
	if ((rand() % 2) == 0)
		playSound(SFX_DEBRIS, x);
	else
		playSound(SFX_DEBRIS2, x);

	object *debris;
	
	amount = rrand(3, rand() % amount);
	limitInt(&amount, 3, 8);

	for (int i = 0 ; i < amount ; i++)
	{
		debris = new object;

		debris->next = NULL;
		debris->x = x;
		debris->y = y;

		debris->thinktime = rrand(60, 180);

		debris->dx = rrand(-500, 500);
		debris->dy = rrand(-500, 500);

		if (debris->dx != 0)
			debris->dx /= 100;

		if (debris->dy != 0)
			debris->dy /= 100;

		engine.debrisTail->next = debris;
		engine.debrisTail = debris;
	}
}

void doDebris()
{
	object *prevDebris = engine.debrisHead;
	object *debris = engine.debrisHead;
	engine.debrisTail = engine.debrisHead;

	while (debris->next != NULL)
	{
		debris = debris->next;

		if (debris->thinktime > 0)
		{
			debris->thinktime--;

			debris->x += engine.ssx + engine.smx;
			debris->y += engine.ssy + engine.smy;
			debris->x += debris->dx;
			debris->y += debris->dy;

			addExplosion(debris->x + rrand(-10, 10), debris->y + rrand(-10, 10), E_BIG_EXPLOSION);
		}

		if (debris->thinktime < 1)
		{
			prevDebris->next = debris->next;
			delete debris;
			debris = prevDebris;
		}
		else
		{
			prevDebris = debris;
			engine.debrisTail = debris;
		}

	}
}
