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

//Filename   : OU_MONS.CPP
//Description: Unit Monster header file

#ifndef __OU_MONS_H
#define __OU_MONS_H

#ifndef __OUNIT_H
#include <ounit.h>
#endif

//----------- Define class Monster -----------//

#pragma pack(1)
class UnitMonster : public Unit
{
public:
	const char* unit_name(int withTitle=1, int firstNameOnly=0);

	//---------- multiplayer checking codes ---------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

	//--------- casting function ----------//

	UnitMonster*	cast_to_UnitMonster()	{ return this; }
};
#pragma pack()

//--------------------------------------------//

#endif

