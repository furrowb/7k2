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

//Filename   : OAI_DEFE.CPP
//Description: AI on defense

#include <all.h>
#include <otalkres.h>
#include <onation.h>

//----- Begin of function Nation::ai_defend -----//
//
// <BaseObj*> attackerObj - BaseObj of the attacker object.
//
int Nation::ai_defend(BaseObj* attackerObj)
{
	// ####### begin Gilbert 25/2 #######//
	if( !attackerObj 
		|| !attackerObj->is_visible() )		// object may be invisible
	// ####### end Gilbert 25/2 #######//
		return 0;

	//--- don't call for defense too frequently, only call once 7 days (since this function will be called every time our king/firm/town is attacked, so this filtering is necessary ---//

	if( info.game_date < ai_last_defend_action_date+7 )
		return 0;

	ai_last_defend_action_date = info.game_date;

	//---------- analyse the situation first -----------//

	err_when( attackerObj->nation_recno == nation_recno );

	int attackerXLoc = attackerObj->obj_loc_x1();
	int attackerYLoc = attackerObj->obj_loc_y1();

	int hasWar;

	int enemyCombatLevel = mobile_defense_combat_level( attackerXLoc, attackerYLoc,
		attackerObj->nation_recno, 0, hasWar );		// 0-don't return immediately even if there is war around this town

	//-- the value returned is enemy strength minus your own strength, so if it's positive, it means that your enemy is stronger than you, otherwise you're stronger than your enemy --//

	int attackCombatLevel = ai_attack_target(attackerXLoc, attackerYLoc, enemyCombatLevel, 1);		// 1-defense mode

	//------ request military aid from allies ----//

	if( attackCombatLevel < enemyCombatLevel && attackerObj->nation_recno )
	{
		ai_request_military_aid();
	}

	return 1;
}
//----- End of function Nation::ai_defend -----//


//----- Begin of function Nation::ai_request_military_aid -----//
//
// Request allied nations to provide immediate military aid.
//
int Nation::ai_request_military_aid()
{
	return 0;		// **BUGHERE, multiplayer sync error, disabled temporarily

	for( int i=nation_array.size() ; i>0 ; i-- )
	{
		if( nation_array.is_deleted(i) )
			continue;

		if( get_relation(i)->status != RELATION_ALLIANCE )
			continue;

		if( should_diplomacy_retry(TALK_REQUEST_MILITARY_AID, i) )
		{
			talk_res.ai_send_talk_msg(i, nation_recno, TALK_REQUEST_MILITARY_AID);
			return 1;
		}
	}

	return 0;
}
//----- End of function Nation::ai_request_military_aid -----//
