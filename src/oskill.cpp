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

//Filename   : OSKILL.CPP
//Description: Class Skill

#include <onation.h>
#include <oskill.h>
#include <ospy.h>
#include <otech.h>
#include <oitem.h>
#include <otechres.h>
#include <itemid.h>


//--------- Begin of function Skill::Skill -----------//

Skill::Skill()
{
	memset( this, 0, sizeof(Skill) );
}
//--------- End of function Skill::Skill -----------//


//--------- Begin of function Skill::init -----------//
//
// <int> unitId			 - unit id.
// [int] initCombatLevel - the startup combat level (default: 0)
// [int] initSkillLevel  - the startup skill level (default: 0)
//
void Skill::init(int unitId, int initCombatLevel, int initSkillLevel)
{
	memset( this, 0, sizeof(Skill) );

	UnitInfo* unitInfo = unit_res[unitId];

	if( unitInfo->race_id )                	// if this is a liviing being, its combat level can be increased through training and battling
		max_combat_level = MAX_COMBAT_BATTLE;
	else
		max_combat_level = 100;

	max_skill_level = 100;

	std_hit_points = unitInfo->hit_points;

	combat_level = (float) initCombatLevel;
   skill_level  = (float) initSkillLevel;

	//-------- initialize potential ---------//

	switch( misc.random(15) )
	{
		case 0:
			skill_potential = 50+misc.random(51);	 // 50 to 100 potential
			break;

		case 1:
			skill_potential = 25+misc.random(26);	 // 25 to 50 potential
			break;

		case 2:
			skill_potential = 10+misc.random(15);	 // 10 to 25 potential
			break;
	}
}
//--------- End of function Skill::init -----------//


//--------- Begin of function Skill::actual_combat_level -----------//

int Skill::actual_combat_level(Item* item)
{
	if( item )
		return (int) combat_level + item->ability(ITEM_ABILITY_COMBAT_LEVEL);
	else
		return (int) combat_level;
}
//--------- End of function Skill::actual_combat_level -----------//


//--------- Begin of function Skill::actual_skill_level -----------//

int Skill::actual_skill_level(Item* item)
{
	if( item) 
		return (int) skill_level + item->ability(ITEM_ABILITY_SKILL_LEVEL);
	else
		return (int) skill_level;
}
//--------- End of function Skill::actual_skill_level -----------//


//--------- Begin of function Skill::actual_max_hit_points -----------//

int Skill::actual_max_hit_points(Item* item)
{
	return (int) std_hit_points * actual_combat_level(item) / 100;
}
//--------- End of function Skill::actual_max_hit_points -----------//


//--------- Begin of function Skill::inc_combat_level --------//

void Skill::inc_combat_level(float changeLevel)
{
	err_when( changeLevel < 0 );

	combat_level += changeLevel;

	if( combat_level > max_combat_level )
		combat_level = max_combat_level;
}
//--------- End of function Skill::inc_combat_level -----------//


//--------- Begin of function Skill::inc_skill_level --------//

void Skill::inc_skill_level(float changeLevel)
{
	err_when( changeLevel < 0 );

	skill_level += changeLevel;

	if( skill_level > max_skill_level )
		skill_level = max_skill_level;
}
//--------- End of function Skill::inc_skill_level -----------//


//--------- Begin of function Skill::set_combat_level -----------//

void Skill::set_combat_level(int newLevel)
{
	err_when( newLevel < 0 );
	err_when( newLevel > max_combat_level );

	combat_level = (float) newLevel;
}
//--------- End of function Skill::set_combat_level -----------//


//--------- Begin of function Skill::set_skill_level -----------//

void Skill::set_skill_level(int newLevel)
{
	err_when( newLevel < 0 || newLevel > max_skill_level );

	skill_level = (float) newLevel;
}
//--------- End of function Skill::set_skill_level -----------//


//--------- Begin of function Skill::set_max_combat_level -----------//

void Skill::set_max_combat_level(int newLevel)
{
	err_when( newLevel < 0 );

	max_combat_level = newLevel;
}
//--------- End of function Skill::set_max_combat_level -----------//


//--------- Begin of function Skill::set_max_skill_level -----------//

void Skill::set_max_skill_level(int newLevel)
{
	err_when( newLevel < 0 );

	max_skill_level = newLevel;
}
//--------- End of function Skill::set_max_skill_level -----------//



// -------- Begin of function Skill::set_unit_id ----------//
//
void Skill::set_unit_id( int unitId )
{
	std_hit_points = unit_res[unitId]->hit_points;
}
// -------- End of function Skill::set_unit_id ----------//
