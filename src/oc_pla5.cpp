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

// Filename    : OC_PLA5.CPP
// Description : sub-plot 5


#include <oc_east.h>
#include <ounitres.h>
#include <oraceres.h>
#include <omonsres.h>
#include <ofirmres.h>
#include <otechres.h>
#include <otech.h>
#include <onationa.h>
#include <onation2.h>
#include <osite.h>
#include <itemid.h>
#include <oitemres.h>

void CampaignEastWest::plot_a5_create_game()
{
	plot_nation_recno2 = 0;

	Nation *monsterNation = nation_array[plot_enemy_nation_recno];

	if( monsterNation->is_monster() )
	{
		// create Fryhtan bane far away from player king

		int itemId = ITEM_MONSTER1_BOOK + monsterNation->monster_id() - 1;
		plot_nation_recno2 = (~nation_array)->king_unit_recno;
		Unit *playerKing = unit_array[(~nation_array)->king_unit_recno];

		int farXLoc;		// distance to place monster bane
		int farYLoc;
		int maxDist = 0;

		int nearXLoc;		// distance to place teleporter
		int nearYLoc;
		int nearDist = 1000;

		for( int trial = 100; trial > 0; --trial )
		{
			int xLoc = misc.random(MAX_WORLD_X_LOC);
			int yLoc = misc.random(MAX_WORLD_Y_LOC);

			if( !world.can_build_site( xLoc, yLoc, 1, 1, 3) )	// some space there
				continue;

			int dist = misc.points_distance( xLoc, yLoc,
				playerKing->next_x_loc(), playerKing->next_y_loc() );

			if( dist > maxDist )
			{
				farXLoc = xLoc;
				farYLoc = yLoc;
				maxDist = dist;
			}
			else if( dist >= 10 && dist < nearDist )
			{
				nearXLoc = xLoc;
				nearYLoc = yLoc;
				nearDist = dist;
			}
		}

		// add monster bane
		if( maxDist )
		{
			site_array.add_site( farXLoc, farYLoc, SITE_ITEM, 
				itemId, item_res.random_para(itemId, misc.rand()) );
		}

		// add teleporter
		if( nearDist < 1000 )
		{
			site_array.add_site( nearXLoc, nearYLoc, SITE_ITEM, 
				ITEM_TELEPORT, item_res.random_para(ITEM_TELEPORT, misc.rand()) );
		}

		// add presistent item (i.e. ring of life) to monster king, so it will not take the bane

		if( !unit_array.is_deleted(monsterNation->king_unit_recno) )
		{
			Unit *monsterKing = unit_array[monsterNation->king_unit_recno];

			if( !monsterKing->item.id )
			{
				monsterKing->item.set( ITEM_REGENERATE, 0 );
			}
		}
	}
}


void CampaignEastWest::plot_a5_next_day()
{
	// make the monster king undead so it must use fryhtan bane

	if( !nation_array.is_deleted(plot_enemy_nation_recno) )
	{
		Nation *monsterNation = nation_array[plot_enemy_nation_recno];

		// only the first fryhtan king has invincibility

		// plot_nation_recno2 is the fryhtan king at create game

		if( monsterNation && monsterNation->king_unit_recno
			&& monsterNation->king_unit_recno == plot_nation_recno2 )
		{
			Unit *monsterKing = unit_array[monsterNation->king_unit_recno];

			if( monsterKing->invulnerable_day_count < 2 )
				monsterKing->invulnerable_day_count = 2;
		}
		else
		{
			plot_nation_recno2 = 0;
		}
	}
}

