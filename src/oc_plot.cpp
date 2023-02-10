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

//Filename   : OC_PLOT.CPP
//Description: Plot functions

#include <oc_east.h>

//--------- Define constant ----------//

enum { PLOT_A_COUNT = 7, PLOT_C_COUNT = 4, PLOT_D_COUNT = 5 };

//--------- Define function tables --------//

static CampaignEastWestFP plot_a_create_game_func_array[PLOT_A_COUNT] =
{
	&CampaignEastWest::plot_a1_create_game,
	&CampaignEastWest::plot_a2_create_game,
	&CampaignEastWest::plot_a3_create_game,
	&CampaignEastWest::plot_a4_create_game,
	&CampaignEastWest::plot_a5_create_game,
	&CampaignEastWest::plot_a6_create_game,
	&CampaignEastWest::plot_a7_create_game,
};

static CampaignEastWestFP plot_a_next_day_func_array[PLOT_A_COUNT] =
{
	&CampaignEastWest::plot_a1_next_day,
	&CampaignEastWest::plot_a2_next_day,
	&CampaignEastWest::plot_a3_next_day,
	&CampaignEastWest::plot_a4_next_day,
	&CampaignEastWest::plot_a5_next_day,
	&CampaignEastWest::plot_a6_next_day,
	&CampaignEastWest::plot_a7_next_day,
};


static CampaignEastWestFP plot_c_create_game_func_array[PLOT_C_COUNT] =
{
	&CampaignEastWest::plot_c1_create_game,
	&CampaignEastWest::plot_c2_create_game,
	&CampaignEastWest::plot_c3_create_game,
	&CampaignEastWest::plot_c4_create_game,
};

static CampaignEastWestFP plot_c_next_day_func_array[PLOT_C_COUNT] =
{
	&CampaignEastWest::plot_c1_next_day,
	&CampaignEastWest::plot_c2_next_day,
	&CampaignEastWest::plot_c3_next_day,
	&CampaignEastWest::plot_c4_next_day,
};


static CampaignEastWestFP plot_d_create_game_func_array[PLOT_D_COUNT] =
{
	&CampaignEastWest::plot_d1_create_game,
	&CampaignEastWest::plot_d2_create_game,
	&CampaignEastWest::plot_d3_create_game,
	&CampaignEastWest::plot_d4_create_game,
	&CampaignEastWest::plot_d5_create_game,
};

static CampaignEastWestFP plot_d_next_day_func_array[PLOT_D_COUNT] =
{
	&CampaignEastWest::plot_d1_next_day,
	&CampaignEastWest::plot_d2_next_day,
	&CampaignEastWest::plot_d3_next_day,
	&CampaignEastWest::plot_d4_next_day,
	&CampaignEastWest::plot_d5_next_day,
};

// ######## begin Gilbert 17/3 #########//

// -------- define static variable ----------//

CampaignEastWestFP CampaignEastWest::plot_create_game_FP_array[] =
{
	&CampaignEastWest::plot_null,
	&CampaignEastWest::plot_a1_create_game,
	&CampaignEastWest::plot_a2_create_game,
	&CampaignEastWest::plot_a3_create_game,
	&CampaignEastWest::plot_a4_create_game,
	&CampaignEastWest::plot_a5_create_game,
	&CampaignEastWest::plot_a6_create_game,
	&CampaignEastWest::plot_a7_create_game,
	&CampaignEastWest::plot_c1_create_game,
	&CampaignEastWest::plot_c2_create_game,
	&CampaignEastWest::plot_c3_create_game,
	&CampaignEastWest::plot_c4_create_game,
	&CampaignEastWest::plot_d1_create_game,
	&CampaignEastWest::plot_d2_create_game,
	&CampaignEastWest::plot_d3_create_game,
	&CampaignEastWest::plot_d4_create_game,
	&CampaignEastWest::plot_d5_create_game,
	NULL			// keep it last
};

CampaignEastWestFP CampaignEastWest::plot_next_day_FP_array[] =
{
	&CampaignEastWest::plot_null,
	&CampaignEastWest::plot_a1_next_day,
	&CampaignEastWest::plot_a2_next_day,
	&CampaignEastWest::plot_a3_next_day,
	&CampaignEastWest::plot_a4_next_day,
	&CampaignEastWest::plot_a5_next_day,
	&CampaignEastWest::plot_a6_next_day,
	&CampaignEastWest::plot_a7_next_day,
	&CampaignEastWest::plot_c1_next_day,
	&CampaignEastWest::plot_c2_next_day,
	&CampaignEastWest::plot_c3_next_day,
	&CampaignEastWest::plot_c4_next_day,
	&CampaignEastWest::plot_d1_next_day,
	&CampaignEastWest::plot_d2_next_day,
	&CampaignEastWest::plot_d3_next_day,
	&CampaignEastWest::plot_d4_next_day,
	&CampaignEastWest::plot_d5_next_day,
	NULL			// keep it last
};
// ######## end Gilbert 17/3 #########//


//---- Begin of function CampaignEastWest::init_random_plot ----//
//
// Randomly initialize a plot for the current campaign game.
//
// <char> plotCategory - 'A' to 'E' indicating the category of the plot.
//
void CampaignEastWest::init_random_plot(char plotCategory)
{
	if( keep_plot_flag )  	// this is set to 1 for replaying a game
	{
		keep_plot_flag = 0;
		return;
	}

	//---------------------------------------------//

	plot_category = plotCategory;

	CampaignEastWestFP *createGameArray = NULL;
	CampaignEastWestFP *nextDayArray = NULL;
	int funcCount = 0;

	switch(plotCategory)
	{
	case 'a':
	case 'A':
		createGameArray = plot_a_create_game_func_array;
		nextDayArray = plot_a_next_day_func_array;
		funcCount = PLOT_A_COUNT;
		break;

	case 'c':
	case 'C':
		createGameArray = plot_c_create_game_func_array;
		nextDayArray = plot_c_next_day_func_array;
		funcCount = PLOT_C_COUNT;
		break;

	case 'd':
	case 'D':
		createGameArray = plot_d_create_game_func_array;
		nextDayArray = plot_d_next_day_func_array;
		funcCount = PLOT_D_COUNT;
		break;

	default:
		funcCount = 0;
		break;
	}

		// ###### begin Gilbert 22/2 ######//
//			if( misc.random(PLOT_A_COUNT*2)==0 )
//			{
//				plot_create_game_FP = plot_null;		// there is a chance that the plot is null. That means there is no plot for the game.
//				plot_next_day_FP 	  = plot_null;
//
//				plot_id = 0;
//			}
//			else
//			{
//				plot_id = misc.random(PLOT_A_COUNT)+1;
//				plot_create_game_FP = plot_a_create_game_func_array[plot_id-1];
//				plot_next_day_FP 	  = plot_a_next_day_func_array[plot_id-1];
//			}

	// modified a bit to simply debugging

	if( misc.random(7)==0 || funcCount==0 )		// 20% chance that this game does not have a plot
	{
		plot_id = 0;								// set to zero, in case the condition is changed
		plot_create_game_FP = &CampaignEastWest::plot_null;		// there is a chance that the plot is null. That means there is no plot for the game.
		plot_next_day_FP 	  = &CampaignEastWest::plot_null;
	}
	else		// 1 to funcCount
	{
		err_when( funcCount == 0 );
		plot_id = misc.random(funcCount)+1;		// randomize the plot of this game.
		err_when( plot_id <= 0 || plot_id > funcCount );
		plot_create_game_FP = createGameArray[plot_id-1];
		plot_next_day_FP 	  = nextDayArray[plot_id-1];
	}
	// ###### end Gilbert 22/2 ######//
}
//---- End of function CampaignEastWest::init_random_plot ----//
