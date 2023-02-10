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

//Filename    : OSFRMRES.CPP
//Description : Object Sprite Frame Resource

#include <osfrmres.h>
#include <all.h>
#include <ostr.h>
#include <osys.h>
#include <ogameset.h>
#include <oworldmt.h>
#include <osprtres.h>

//-------- define file name -----------//

#define SPRITE_FRAME_DB    "SFRAME"


//-------- Begin of function SpriteFrameRes::init ---------//

void SpriteFrameRes::init()
{
	deinit();

	//------- load database information --------//

	load_info();

	init_flag=1;
}
//--------- End of function SpriteFrameRes::init ----------//


//-------- Begin of function SpriteFrameRes::deinit ---------//

void SpriteFrameRes::deinit()
{
	if( init_flag )
	{
//		delete[] sprite_frame_array;
		mem_del(sprite_frame_array);

		init_flag=0;
	}
}
//--------- End of function SpriteFrameRes::deinit ----------//


//------- Begin of function SpriteFrameRes::load_info ---------//

void SpriteFrameRes::load_info()
{
	Database   		*dbSpriteFrame = game_set.open_db(SPRITE_FRAME_DB);
	SpriteFrameRec *frameRec;
	SpriteFrame 	*spriteFrame;
	SpriteInfo 	   *spriteInfo;

	int		  		i;

	sprite_frame_count = dbSpriteFrame->rec_count();
//	sprite_frame_array = new SpriteFrame[sprite_frame_count];
	sprite_frame_array = (SpriteFrame *) mem_add(sizeof(SpriteFrame)*sprite_frame_count);

	memset( sprite_frame_array, 0, sizeof(SpriteFrame)*sprite_frame_count );

	char lastSpriteName[frameRec->NAME_LEN+1];

	//--------- read in frame information ---------//

	for( i=0 ; i<dbSpriteFrame->rec_count() ; i++ )
	{
		frameRec  = (SpriteFrameRec*) dbSpriteFrame->read(i+1);
		spriteFrame = sprite_frame_array+i;

		char spriteName[frameRec->NAME_LEN+1];
		
		misc.rtrim_fld( spriteName, frameRec->sprite_name, frameRec->NAME_LEN );

		// check for sprite_code,

		// basically, it try to match sprite_code from sprite_res
		// speed up by compare sprite_code in last record first
		// sprite_code is as same as last record, skip comparing

		if( i == 0 || strcmp(spriteName, lastSpriteName) )
		{
			int j;
			for( j=1 ;
				j<= sprite_res.sprite_info_count && strcmp(spriteName, sprite_res[j]->sprite_code);
				j++ );
			err_when( j > sprite_res.sprite_info_count );		// error when not found
			spriteInfo = sprite_res[j];
		}

		err_when( !spriteInfo || strcmp( spriteInfo->sprite_code, spriteName) );

		spriteFrame->offset_x = misc.atoi(frameRec->offset_x, frameRec->OFFSET_LEN);
		spriteFrame->offset_y = misc.atoi(frameRec->offset_y, frameRec->OFFSET_LEN);

		spriteFrame->width  = misc.atoi(frameRec->width , frameRec->WIDTH_LEN);
		spriteFrame->height = misc.atoi(frameRec->height, frameRec->HEIGHT_LEN);

		memcpy( &spriteFrame->bitmap_offset, frameRec->bitmap_offset, sizeof(uint32_t) );

		// modify offset_x/y for 7k2, assume loc_width = loc_height = 1
		spriteFrame->offset_x += -spriteInfo->loc_width*LOCATE_WIDTH/2 - (-spriteInfo->loc_width*ZOOM_LOC_X_WIDTH/2 + -spriteInfo->loc_height*ZOOM_LOC_Y_WIDTH/2);
		spriteFrame->offset_y += -spriteInfo->loc_height*LOCATE_HEIGHT/2 - (-spriteInfo->loc_width*ZOOM_LOC_X_HEIGHT/2 + -spriteInfo->loc_height*ZOOM_LOC_Y_HEIGHT/2);

	//	spriteFrame->offset_x += -LOCATE_WIDTH/2 - (-ZOOM_LOC_X_WIDTH/2 + -ZOOM_LOC_Y_WIDTH/2);
	//	spriteFrame->offset_y += -LOCATE_HEIGHT/2 - (-ZOOM_LOC_X_HEIGHT/2 + -ZOOM_LOC_Y_HEIGHT/2);

		strcpy( lastSpriteName, spriteName );
	}
}
//-------- End of function SpriteFrameRes::load_info ---------//


#ifdef DEBUG

//-------- Begin of function SpriteFrameRes::operator[] -------//

SpriteFrame* SpriteFrameRes::operator[](int recNo)
{
	if( recNo<1 || recNo>sprite_frame_count )
		err.run( "SpriteFrameRes::operator[]" );

	return sprite_frame_array+recNo-1;
}

//--------- End of function SpriteFrameRes::operator[] --------//

#endif
