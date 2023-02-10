/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 1999 Enlight Software Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Filename   : OW_SOUND.CPP
// Description: Ambient Sound Functions

#include <osys.h>
#include <oaudio.h>
#include <ose.h>
#include <ovolume.h>
#include <oweather.h>
#include <oworld.h>
#include <oworldmt.h>

// ------- define constant -------//
#define MAX_BIRD 17

//------- Begin of function World::process_ambient_sound -------//
//
void World::process_ambient_sound()
{
	int temp = weather.temp_c();
	if( weather.rain_scale() == 0 && temp >= 15 && misc.random(temp) >= 12)
	{
		int bird = misc.random(MAX_BIRD) + 1;
		char sndFile[] = "BIRDS00";
		err_when( bird > 99 );
		sndFile[5] = (bird / 10) + '0';
		sndFile[6] = (bird % 10) + '0';

		int xLoc = misc.random(max_x_loc) - (zoom_matrix->top_x_loc + zoom_matrix->center_x);
		int yLoc = misc.random(max_y_loc) - (zoom_matrix->top_y_loc + zoom_matrix->center_y);
		PosVolume p(PosVolume(xLoc, yLoc));
                RelVolume relVolume(p, 200, MAX_MAP_WIDTH);
		if( relVolume.rel_vol < 80)
			relVolume.rel_vol = 80;

		se_ctrl.request(sndFile, relVolume);
	}
}
//-------- End of function World::process_ambient_sound --------//
