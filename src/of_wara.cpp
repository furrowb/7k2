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

//Filename    : OF_WARA.CPP
//Description : Firm War Factory - AI functions

#include <onation.h>
#include <oinfo.h>
#include <otown.h>
#include <ounit.h>
#include <otechres.h>
#include <of_camp.h>
#include <of_war.h>

//--------- Begin of function FirmWar::process_ai ---------//

void FirmWar::process_ai()
{
	//---- think about which technology to research ----//

	if( !build_unit_id )
		think_new_production();

	//----- think about closing down this firm -----//

	if( info.game_date%30==firm_recno%30 )
	{
		if( think_del() )
			return;
	}
}
//----------- End of function FirmWar::process_ai -----------//


//------- Begin of function FirmWar::think_del -----------//
//
// Think about deleting this firm.
//
int FirmWar::think_del()
{
	if( worker_count > 0 )
		return 0;

	//-- check whether the firm is linked to any towns or not --//

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		if( linked_town_enable_array[i] == LINK_EE )
			return 0;
	}

	//------------------------------------------------//

	ai_del_firm();

	return 1;
}
//--------- End of function FirmWar::think_del -----------//


//----- Begin of function FirmWar::think_new_production ------//
//
// Think about which weapon to produce.
//
void FirmWar::think_new_production()
{
	//----- first see if we have enough money to build & support the weapon ----//

	if( !should_build_new_weapon() )
		return;

	//---- calculate the average instance count of all available weapons ---//

	int 		 weaponTypeCount=0, totalWeaponCount=0;
	UnitInfo* unitInfo;

	int unitId;
	for( unitId=1; unitId<=MAX_UNIT_TYPE ; unitId++ )
	{
		unitInfo = unit_res[unitId];

		if( unitInfo->unit_class != UNIT_CLASS_WEAPON ||
			 unitInfo->get_nation_tech_level(nation_recno) == 0 )
		{
			continue;
		}

		if( unitId == UNIT_EXPLOSIVE_CART )		// AI doesn't use Porcupine
			continue;

		weaponTypeCount++;
		totalWeaponCount += unitInfo->nation_unit_count_array[nation_recno-1];
	}

	if( weaponTypeCount==0 )		// none of weapon technologies is available
		return;

	int averageWeaponCount = totalWeaponCount/weaponTypeCount;

	//----- think about which is best to build now ------//

	int curRating, bestRating=0, bestUnitId=0;

	for( unitId=1; unitId<=MAX_UNIT_TYPE ; unitId++ )
	{
		unitInfo = unit_res[unitId];

		if( unitInfo->unit_class != UNIT_CLASS_WEAPON )
			continue;

		int techLevel = unitInfo->get_nation_tech_level(nation_recno);

		if( techLevel==0 )
			continue;

		if( unitId == UNIT_EXPLOSIVE_CART )		//**BUGHERE, don't produce it yet, it needs a different usage than the others.
			continue;

		int unitCount = unitInfo->nation_unit_count_array[nation_recno-1];

		curRating = averageWeaponCount-unitCount + techLevel*3;

		if( curRating > bestRating )
		{
			bestRating = curRating;
			bestUnitId = unitId;
		}
	}

	//------------------------------------//

	if( bestUnitId )
		add_queue( bestUnitId );
}
//------ End of function FirmWar::think_new_production -------//


//----- Begin of function FirmWar::should_build_new_weapon ------//
//
int FirmWar::should_build_new_weapon()
{
	//----- first see if we have enough money to build & support the weapon ----//

	Nation* nationPtr = nation_array[nation_recno];

	if( nationPtr->true_profit_365days() < 0
		 && nationPtr->cash < 15000 * (100+nationPtr->pref_cash_reserve) / 200 )		// don't build new weapons if we are currently losing money
	{
		return 0;
	}

	if( nationPtr->expense_365days(EXPENSE_WEAPON) >
		 nationPtr->income_365days() * (30 + nationPtr->pref_use_weapon/2) / 100 )		// if weapon expenses are larger than 30% to 80% of the total income, don't build new weapons
	{
		return 0;
	}

	//----- see if there is any space on existing camps -----//

	FirmCamp* firmCamp;

	for( int i=0 ; i<nationPtr->ai_camp_count ; i++ )
	{
		firmCamp = firm_array[ nationPtr->ai_camp_array[i] ]->cast_to_FirmCamp();

		if( firmCamp->region_id != region_id )
			continue;

		if( firmCamp->soldier_count < MAX_SOLDIER )		// there is space in this firm
			return 1;
	}

	return 0;
}
//------ End of function FirmWar::should_build_new_weapon -------//


//------- Begin of function FirmWar::think_adjust_workforce -------//
//
// Think about adjusting the workforce by increasing or decreasing
// needed_worker_count.
//
int FirmWar::think_adjust_workforce()
{
	Nation* nationPtr = nation_array[nation_recno];

	int preferredJoblessPop = nationPtr->pref_economic_development/20;
	int availableWorkforce  = available_workforce();

	if( needed_worker_count < 10 )	// we have too few workers, try to get more
	{
		preferredJoblessPop -= 10 - needed_worker_count;

		if( preferredJoblessPop < 0 )
			preferredJoblessPop = 0;
	}

	//-------------------------------------------------------//
	//
	// If the war factory is currently building a war machine
	// and we have available workforce surplus, increase the
	// needed employee count.
	//
	//-------------------------------------------------------//

	if( availableWorkforce > preferredJoblessPop && build_unit_id )
	{
		set_needed_worker_count( needed_worker_count+1, COMMAND_AI );
		return 1;
	}

	//-------------------------------------------------------//
	//
	// If the war factory is currently not building a war machine
	// and the extra available workforce is below the balanced value,
	// decrease the needed employee count.
	//
	//-------------------------------------------------------//

	if( availableWorkforce < preferredJoblessPop &&
		 !build_unit_id &&
		 needed_worker_count > 5 )		// minimum 5 workers
	{
		set_needed_worker_count( needed_worker_count-1, COMMAND_AI );
		return 1;
	}

	return 0;
}
//--------- End of function FirmWar::think_adjust_workforce -------//

