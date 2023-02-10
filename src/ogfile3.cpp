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

//Filename    : OGFILE3.CPP
//Description : Object Game file, save game and restore game, part 3

#include <ounit.h>

#include <obullet.h>
#include <ob_proj.h>
#include <osite.h>
#include <otown.h>
#include <onation.h>
#include <ofirm.h>
#include <of_anim.h>
#include <otornado.h>
#include <orebel.h>
#include <ospy.h>
#include <osnowg.h>
#include <oregion.h>
#include <ostate.h>
#include <oregions.h>
#include <onews.h> 
#include <owaypnt.h>
#include <ogfile.h>
#include <ounit.h>
#include <opfind.h>
#include <oun_grp.h>
#include <file_io_visitor.h>
#include <file_reader.h>

using namespace FileIOVisitor;

//------- declare static functions -------//

static char* create_rebel_func();

static void write_ai_info(File* filePtr, short* aiInfoArray, short aiInfoCount, short aiInfoSize);
static void read_ai_info(File* filePtr, short** aiInfoArrayPtr, short& aiInfoCount, short& aiInfoSize);


//-------- Start of function UnitArray::write_file -------------//
//
int UnitArray::write_file(File* filePtr)
{
   int  i;
   Unit *unitPtr;

	filePtr->file_put_short( size()  );  // no. of units in unit_array
	
	filePtr->file_put_short( selected_recno );
	filePtr->file_put_short( selected_count );

	for( i=1; i<=size() ; i++ )
   {
      unitPtr = (Unit*) get_ptr(i);

      //----- write unitId or 0 if the unit is deleted -----//

      if( !unitPtr )    // the unit is deleted
      {
         filePtr->file_put_short(0);
      }
      else
      {
         //--------- write unit_id -------------//

			filePtr->file_put_short(unitPtr->unit_id);

         //------ write data in the base class ------//

         if( !unitPtr->write_file(filePtr) )
            return 0;

         //------ write data in the derived class ------//

         if( !unitPtr->write_derived_file(filePtr) )
				return 0;
      }
   }

   //------- write empty room array --------//

   write_empty_room(filePtr);

   return 1;
}
//--------- End of function UnitArray::write_file ---------------//


//-------- Start of function UnitArray::read_file -------------//
//
int UnitArray::read_file(File* filePtr)
{
	Unit*   unitPtr;
	int     i, unitId, emptyRoomCount=0;

	int unitCount    = filePtr->file_get_short();  // get no. of units from file

	selected_recno   = filePtr->file_get_short();
	selected_count   = filePtr->file_get_short();

   for( i=1 ; i<=unitCount ; i++ )
   {
      unitId = filePtr->file_get_short();

      if( unitId==0 )  // the unit has been deleted
      {
         add_blank(1);     // it's a DynArrayB function
         emptyRoomCount++;
      }
      else
		{
         //----- create unit object -----------//

			unitPtr = create_unit( unitId );
         unitPtr->unit_id = unitId;

         //---- read data in base class -----//

         if( !unitPtr->read_file( filePtr ) )
            return 0;

         //----- read data in derived class -----//

         if( !unitPtr->read_derived_file( filePtr ) )
            return 0;
      }
   }

	//-------- linkout() those record added by add_blank() ----------//
   //-- So they will be marked deleted in DynArrayB and can be -----//
	//-- undeleted and used when a new record is going to be added --//

   for( i=size() ; i>0 ; i-- )
	{
		DynArrayB::go(i);             // since UnitArray has its own go() which will call GroupArray::go()

      if( get_ptr() == NULL )       // add_blank() record
			linkout();
	}

	//------- read empty room array --------//

	read_empty_room(filePtr);

	//------- verify the empty_room_array loading -----//

#ifdef DEBUG
	err_when( empty_room_count != emptyRoomCount );

	for( i=0 ; i<empty_room_count ; i++ )
	{
		if( !is_deleted( empty_room_array[i].recno ) )
			err_here();
	}
#endif

	return 1;
}
//--------- End of function UnitArray::read_file ---------------//


enum { UNIT_RECORD_SIZE = 233 };

template <typename Visitor>
static void visit_baseobj(Visitor *v, BaseObj *o)
{
	visit<int16_t>(v, &o->base_obj_recno);
	visit<int8_t>(v, &o->obj_type);
	visit<int16_t>(v, &o->obj_extern_recno);

	//------- game vars -----------//

	visit<uint8_t>(v, &o->is_ai);
	visit<uint8_t>(v, &o->already_killed);
	visit<int8_t>(v, &o->nation_recno);
	visit<int8_t>(v, &o->race_id);
	visit<int32_t>(v, &o->last_attack_date);
	visit<int16_t>(v, &o->last_attack_nation_recno);
	visit<int8_t>(v, &o->defense_attribute.sturdiness);
	visit<int8_t>(v, &o->defense_attribute.is_wood);

	//-------- hit points vars ---------//

	visit<float>(v, &o->hit_points);
}

template <typename Visitor>
static void visit_sprite(Visitor *v, Sprite *s)
{
	visit<int16_t>(v, &s->sprite_id);
	visit<int16_t>(v, &s->sprite_recno);
	visit<int8_t>(v, &s->mobile_type);
	visit<uint8_t>(v, &s->cur_action);
	visit<uint8_t>(v, &s->saved_action);
	visit<uint8_t>(v, &s->cur_dir);
	visit<uint8_t>(v, &s->cur_frame);
	visit<uint8_t>(v, &s->cur_attack);
	visit<uint8_t>(v, &s->final_dir);
	visit<int8_t>(v, &s->turn_delay);
	visit<int8_t>(v, &s->guard_count);
	visit<int32_t>(v, &s->freeze_frame_left);
	visit<uint8_t>(v, &s->remain_attack_delay);
	visit<uint8_t>(v, &s->remain_frames_per_step);
	visit<int16_t>(v, &s->cur_x);
	visit<int16_t>(v, &s->cur_y);
	visit<int16_t>(v, &s->go_x);
	visit<int16_t>(v, &s->go_y);
	visit<int16_t>(v, &s->next_x);
	visit<int16_t>(v, &s->next_y);
	visit_pointer(v, &s->sprite_info);
}

template <typename Visitor>
static void visit_unitb(Visitor *v, UnitB *b)
{
	//--------- profile vars ----------//

	visit<int16_t>(v, &b->unit_id);

	//-------- selection vars -----//

	visit<int8_t>(v, &b->selected_flag);

	//----- location info -------//

	visit<int8_t>(v, &b->loc_width);
	visit<int8_t>(v, &b->loc_height);
	visit<int32_t>(v, &b->wait_state);
	visit<int32_t>(v, &b->retry_state);
	visit<int8_t>(v, &b->cur_order.mode);
	visit<int16_t>(v, &b->cur_order.para);
	visit<int16_t>(v, &b->cur_order.loc_x);
	visit<int16_t>(v, &b->cur_order.loc_y);
	visit<int16_t>(v, &b->cur_order.action_nation_recno);
	visit<uint16_t>(v, &b->cur_order.ai_action_id);
	visit<uint16_t>(v, &b->cur_order.name_id);

	//----- movement vars ----//

	visit<int16_t>(v, &b->move_to_loc_x);
	visit<int16_t>(v, &b->move_to_loc_y);
	visit<int16_t>(v, &b->wait_count);

	//------ path var -------//

	visit_pointer(v, &b->cur_path);
	visit<int16_t>(v, &b->cur_path_result_id);
	visit<int32_t>(v, &b->steps_remaining);

	//---- other movement and path seeking vars ----//

	visit<int8_t>(v, &b->seek_path_fail_count);
	visit<int8_t>(v, &b->ignore_power_nation);
	visit<int32_t>(v, &b->number_of_times_being_blocked);
	visit<uint8_t>(v, &b->check_formation);
}

template <typename Visitor>
static void visit_unit(Visitor *v, Unit *u)
{
	v->skip(4);  // virtual table pointer

	visit_baseobj(v, u);
	visit_sprite(v, u);
	visit_unitb(v, u);

	//--------- profile vars ----------//

	visit<int8_t>(v, &u->rank_id);
	visit<uint16_t>(v, &u->name_id);
	visit<int16_t>(v, &u->hero_id);
	visit<int8_t>(v, &u->is_royal);
	visit<int32_t>(v, &u->unique_id);
	visit<int8_t>(v, &u->loyalty);
	visit<int8_t>(v, &u->target_loyalty);
	visit<int16_t>(v, &u->spy_recno);

	//-------- skill vars ----------//

	visit<uint8_t>(v, &u->skill.skill_potential);
	visit<int16_t>(v, &u->skill.max_combat_level);
	visit<int16_t>(v, &u->skill.max_skill_level);
	visit<int16_t>(v, &u->skill.std_hit_points);
	visit<float>(v, &u->skill.combat_level);
	visit<float>(v, &u->skill.skill_level);

	//------ unit mode vars --------//

	visit<int8_t>(v, &u->unit_mode);
	visit<int16_t>(v, &u->unit_mode_para);

	//------- politics vars ---------//

	visit<int16_t>(v, &u->nation_contribution);
	visit<int16_t>(v, &u->total_reward);

	//--------- Team vars --------//

	visit_pointer(v, &u->team_info);
	visit<int16_t>(v, &u->leader_unit_recno);

	//-------- item vars --------//

	visit<int16_t>(v, &u->item.id);
	visit<int32_t>(v, &u->item.para);

	// ------- magic effect vars -------//

	visit<int16_t>(v, &u->invulnerable_day_count);

	//------- order vars -------//

	visit<int8_t>(v, &u->pushed_order.mode);
	visit<int16_t>(v, &u->pushed_order.para);
	visit<int16_t>(v, &u->pushed_order.loc_x);
	visit<int16_t>(v, &u->pushed_order.loc_y);
	visit<int16_t>(v, &u->pushed_order.action_nation_recno);
	visit<uint16_t>(v, &u->pushed_order.ai_action_id);
	visit<uint16_t>(v, &u->pushed_order.name_id);
	visit<int8_t>(v, &u->saved_order.mode);
	visit<int16_t>(v, &u->saved_order.para);
	visit<int16_t>(v, &u->saved_order.loc_x);
	visit<int16_t>(v, &u->saved_order.loc_y);
	visit<int16_t>(v, &u->saved_order.action_nation_recno);
	visit<uint16_t>(v, &u->saved_order.ai_action_id);
	visit<uint16_t>(v, &u->saved_order.name_id);

	//------- attack parameters --------//

	visit_pointer(v, &u->attack_info_array);
	visit<int8_t>(v, &u->attack_count);
	visit<int16_t>(v, &u->range_attack_x_loc);
	visit<int16_t>(v, &u->range_attack_y_loc);
	visit<int16_t>(v, &u->attack_loc_offset_x);
	visit<int16_t>(v, &u->attack_loc_offset_y);
	visit<uint8_t>(v, &u->attack_dir);
	visit<int16_t>(v, &u->cur_power);
	visit<int16_t>(v, &u->max_power);
	visit<int16_t>(v, &u->original_target_x_loc);
	visit<int16_t>(v, &u->original_target_y_loc);
	visit<int32_t>(v, &u->last_ask_attack_date);

	//------- other movement and attack behavior vars ------//

	visit<int8_t>(v, &u->can_guard_flag);
	visit<uint8_t>(v, &u->force_move_flag);
	visit<uint8_t>(v, &u->has_way_point);
	visit<int16_t>(v, &u->home_camp_firm_recno);
	visit<int8_t>(v, &u->behavior_mode);
	visit<int32_t>(v, &u->auto_retreat_hit_point);

	//---------- AI vars ---------//

	visit<int16_t>(v, &u->ai_original_target_x_loc);
	visit<int16_t>(v, &u->ai_original_target_y_loc);
	visit<uint8_t>(v, &u->ai_no_suitable_action);

	// -------- special ability ----------//

	visit<int32_t>(v, &u->last_special_ability_start_date);
	visit<int8_t>(v, &u->special_abilty_id);
	visit<int8_t>(v, &u->special_abilty_para);

	// -------- newly added member ----------//

	visit<int8_t>(v, &u->in_ai_attack_mission);
}


//--------- Begin of function Unit::write_file ---------//
//
// Write data in derived class.
//
// If the derived Unit don't have any special data,
// just use Unit::write_file(), otherwise make its own derived copy of write_file()
//
int Unit::write_file(File* filePtr)
{
	if( !write_with_record_size( filePtr, this, &visit_unit<FileWriterVisitor>, UNIT_RECORD_SIZE ) )
		return 0;

	//---------- write PathResult ----------//

	if( cur_path && cur_path_result_id > 0 )
	{
		if( !filePtr->file_write(cur_path, cur_path_result_id * sizeof(PathResult)) )
			return 0;
	}

	//------------ write TeamInfo -----------//

	if( team_info )
	{
		if( !filePtr->file_write( team_info, sizeof(TeamInfo) ) )
			return 0;
	}

	return 1;
}
//----------- End of function Unit::write_file ---------//


//--------- Begin of function Unit::read_file ---------//
//
int Unit::read_file(File* filePtr)
{
	FileReader r;
	FileReaderVisitor v;

	if( !r.init(filePtr) )
		return 0;

	r.check_record_size(UNIT_RECORD_SIZE);
	v.init(&r);
	visit_unit(&v, this);

	if( !r.good() )
		return 0;

	r.deinit();

	//---------- read path ----------//

	if( cur_path )
	{
		if( cur_path_result_id > 0 )
		{
			cur_path = (PathResult *)mem_add(cur_path_result_id * sizeof(PathResult));

			if( !filePtr->file_read(cur_path, cur_path_result_id * sizeof(PathResult)) )
				return 0;
		}
		else
		{
			cur_path = NULL;
		}
	}

	//------ read team_info --------//

	if( team_info )
	{
		team_info = (TeamInfo*) mem_add( sizeof(TeamInfo) );

		if( !filePtr->file_read( team_info, sizeof(TeamInfo) ) )
			return 0;
	}

	//------- set sprite_info ----------//

	sprite_info = sprite_res[sprite_id];

	//-------- load sprite bitmap ------------//

	sprite_info->load_bitmap_res();

	return 1;
}
//----------- End of function Unit::read_file ---------//


//--------- Begin of function Unit::write_derived_file ---------//
//
int Unit::write_derived_file(File* filePtr)
{
   //--- write data in derived class -----//

	int writeSize = unit_array.unit_class_size(unit_id)-sizeof(Unit);

   if( writeSize > 0 )
   {
      if( !filePtr->file_write( (char*) this + sizeof(Unit), writeSize ) )
         return 0;
   }

   return 1;
}
//----------- End of function Unit::write_derived_file ---------//


//--------- Begin of function Unit::read_derived_file ---------//
//
int Unit::read_derived_file(File* filePtr)
{
	//--- read data in derived class -----//

   int readSize = unit_array.unit_class_size(unit_id) - sizeof(Unit);

   if( readSize > 0 )
	{
		if( !filePtr->file_read( (char*) this + sizeof(Unit), readSize ) )
			return 0;
	}

	init_attack_info();

	return 1;
}
//----------- End of function Unit::read_derived_file ---------//


/*//--------- Begin of function UnitMarine::read_derived_file ---------//
int UnitMarine::read_derived_file(File* filePtr)
{
	//---- backup virtual functions table pointer of splash ----//
	char* splashVfPtr = *((char **)&splash);

	//--------- read file --------//
	if( !Unit::read_derived_file(filePtr) )
		return 0;

	//-------- restore virtual function table pointer -------//
	*((char **)&splash) = splashVfPtr ;

	//------- post-process the data read --------//
	splash.sprite_info = sprite_res[splash.sprite_id];
	splash.sprite_info->load_bitmap_res();

	return 1;
}
//--------- End of function UnitMarine::read_derived_file ---------//
*/

//*****//


//-------- Start of function BulletArray::write_file -------------//
//
int BulletArray::write_file(File* filePtr)
{
	int    i, emptyRoomCount=0;;
	Bullet *bulletPtr;

	filePtr->file_put_short( size() );  // no. of bullets in bullet_array

	for( i=1; i<=size() ; i++ )
	{
		bulletPtr = (Bullet*) get_ptr(i);

		//----- write bulletId or 0 if the bullet is deleted -----//

		if( !bulletPtr )    // the bullet is deleted
		{
			filePtr->file_put_short(0);
			emptyRoomCount++;
		}
		else
		{
			filePtr->file_put_short(bulletPtr->sprite_id);      // there is a bullet in this record

			//------ write data in the base class ------//

			if( !bulletPtr->write_file(filePtr) )
				return 0;

			//------ write data in the derived class -------//

			if( !bulletPtr->write_derived_file(filePtr) )
				return 0;
		}
	}

	//------- write empty room array --------//

	write_empty_room(filePtr);

	//------- verify the empty_room_array loading -----//

#ifdef DEBUG
	err_when( empty_room_count != emptyRoomCount );

   for( i=0 ; i<empty_room_count ; i++ )
   {
		if( !is_deleted( empty_room_array[i].recno ) )
         err_here();
   }
#endif

	return 1;
}
//--------- End of function BulletArray::write_file -------------//


//-------- Start of function BulletArray::read_file -------------//
//
int BulletArray::read_file(File* filePtr)
{
	int     i, bulletRecno, bulletCount, emptyRoomCount=0, spriteId;
	Bullet* bulletPtr;

	bulletCount = filePtr->file_get_short();  // get no. of bullets from file

	for( i=1 ; i<=bulletCount ; i++ )
	{
		spriteId = filePtr->file_get_short();
		if( spriteId == 0 )
		{
			add_blank(1);     // it's a DynArrayB function

			emptyRoomCount++;
		}
		else
		{
			//----- create bullet object -----------//

			bulletRecno = create_bullet(spriteId);
			bulletPtr   = bullet_array[bulletRecno];

         //----- read data in base class --------//

         if( !bulletPtr->read_file( filePtr ) )
            return 0;

			//----- read data in derived class -----//

			if( !bulletPtr->read_derived_file( filePtr ) )
				return 0;
      }
	}

   //-------- linkout() those record added by add_blank() ----------//
	//-- So they will be marked deleted in DynArrayB and can be -----//
	//-- undeleted and used when a new record is going to be added --//

	for( i=1 ; i<=size() ; i++ )
	{
		DynArrayB::go(i);             // since BulletArray has its own go() which will call GroupArray::go()

		if( get_ptr() == NULL )       // add_blank() record
			linkout();
	}

	//------- read empty room array --------//

	read_empty_room(filePtr);

	//------- verify the empty_room_array loading -----//

#ifdef DEBUG
	err_when( empty_room_count != emptyRoomCount );

	for( i=0 ; i<empty_room_count ; i++ )
	{
		if( !is_deleted( empty_room_array[i].recno ) )
			err_here();
	}
#endif

	return 1;
}
//--------- End of function BulletArray::read_file ---------------//


template <typename Visitor>
static void visit_bullet(Visitor *v, Bullet *b)
{
	v->skip(4);  // virtual table pointer

	visit_baseobj(v, b);
	visit_sprite(v, b);

	visit<int16_t>(v, &b->parent_recno);
	visit<int16_t>(v, &b->parent_base_obj_recno);
	visit<int8_t>(v, &b->target_mobile_type);
	visit<float>(v, &b->attack_damage);
	visit<int16_t>(v, &b->damage_radius);
	visit<int8_t>(v, &b->fire_radius);
	visit<int16_t>(v, &b->origin_x);
	visit<int16_t>(v, &b->origin_y);
	visit<int16_t>(v, &b->target_x_loc);
	visit<int16_t>(v, &b->target_y_loc);
	visit<int16_t>(v, &b->cur_step);
	visit<int16_t>(v, &b->total_step);
	visit<int16_t>(v, &b->z_init);
	visit<int16_t>(v, &b->z_dest);
	visit<int8_t>(v, &b->attack_attribute.sturdiness);
	visit<int8_t>(v, &b->attack_attribute.explosiveness);
	visit<int8_t>(v, &b->attack_attribute.heat);
	visit<int8_t>(v, &b->attack_attribute.wood_favour);
}

enum { BULLET_RECORD_SIZE = 92 };


//--------- Begin of function Bullet::write_file ---------//
//
int Bullet::write_file(File* filePtr)
{
	return write_with_record_size(filePtr,
				      this,
				      &visit_bullet<FileWriterVisitor>,
				      BULLET_RECORD_SIZE);
}
//----------- End of function Bullet::write_file ---------//


//--------- Begin of function Bullet::read_file ---------//
//
int Bullet::read_file(File* filePtr)
{
	if( !read_with_record_size(filePtr,
				   this,
				   &visit_bullet<FileReaderVisitor>,
				   BULLET_RECORD_SIZE) )
		return 0;

   //------------ post-process the data read ----------//

	sprite_info = sprite_res[sprite_id];

	sprite_info->load_bitmap_res();

	return 1;
}
//----------- End of function Bullet::read_file ---------//


//----------- Begin of function Bullet::write_derived_file ---------//
int Bullet::write_derived_file(File *filePtr)
{
	//--- write data in derived class -----//

	int writeSize = bullet_array.bullet_class_size(sprite_id)-sizeof(Bullet);

	if( writeSize > 0 )
	{
		if( !filePtr->file_write( (char*) this + sizeof(Bullet), writeSize ) )
			return 0;
	}

	return 1;

}
//----------- End of function Bullet::write_derived_file ---------//


//----------- Begin of function Bullet::read_derived_file ---------//
int Bullet::read_derived_file(File *filePtr)
{
	//--- read data in derived class -----//

	int readSize = bullet_array.bullet_class_size(sprite_id) - sizeof(Bullet);

	if( readSize > 0 )
	{
		if( !filePtr->file_read( (char*) this + sizeof(Bullet), readSize ) )
			return 0;
	}

	return 1;
}
//----------- End of function Bullet::read_derived_file ---------//


template <typename Visitor>
static void visit_projectile(Visitor *v, Projectile *p)
{
	visit<float>(v, &p->z_coff);

	v->skip(4);  // virtual table pointer
	visit_baseobj(v, &p->act_bullet);
	visit_sprite(v, &p->act_bullet);

	v->skip(4);  // virtual table pointer
	visit_baseobj(v, &p->bullet_shadow);
	visit_sprite(v, &p->bullet_shadow);
}

enum { PROJECTILE_RECORD_SIZE = 124 };


//----------- Begin of function Projectile::write_derived_file ---------//

int Projectile::write_derived_file(File *filePtr)
{
	return write_with_record_size( filePtr,
				       this,
				       &visit_projectile<FileWriterVisitor>,
				       PROJECTILE_RECORD_SIZE );
}
//----------- End of function Projectile::write_derived_file ---------//


//----------- Begin of function Projectile::read_derived_file ---------//

int Projectile::read_derived_file(File *filePtr)
{
	if( !read_with_record_size( filePtr,
				    this,
				    &visit_projectile<FileReaderVisitor>,
				    PROJECTILE_RECORD_SIZE ) )
		return 0;

   //----------- post-process the data read ----------//
	act_bullet.sprite_info = sprite_res[act_bullet.sprite_id];
	act_bullet.sprite_info->load_bitmap_res();
	bullet_shadow.sprite_info = sprite_res[bullet_shadow.sprite_id];
	bullet_shadow.sprite_info->load_bitmap_res();

	return 1;
}
//----------- End of function Projectile::read_derived_file ---------//

//*****//

template <typename Visitor>
static void visit_place(Visitor *v, Place *p)
{
	visit<int32_t>(v, &p->setup_date);
	visit<float>(v, &p->place_max_hit_points);
	visit<char>(v, &p->no_neighbor_space);

	//--------- location vars ----------//

	visit<uint8_t>(v, &p->region_id);
	visit<int16_t>(v, &p->loc_x1);
	visit<int16_t>(v, &p->loc_y1);
	visit<int16_t>(v, &p->loc_x2);
	visit<int16_t>(v, &p->loc_y2);
	visit<int16_t>(v, &p->abs_x1);
	visit<int16_t>(v, &p->abs_y1);
	visit<int16_t>(v, &p->abs_x2);
	visit<int16_t>(v, &p->abs_y2);
	visit<int16_t>(v, &p->altitude);
	visit<int16_t>(v, &p->center_x);
	visit<int16_t>(v, &p->center_y);

	//------- animation vars ---------//

	visit<int8_t>(v, &p->cur_frame);
	visit<int8_t>(v, &p->remain_frame_delay);

	//------- construction vars --------//

	visit<int8_t>(v, &p->under_construction);
	visit<int16_t>(v, &p->builder_recno);
	visit<uint8_t>(v, &p->builder_region_id);
	visit<int8_t>(v, &p->repair_flag);
}

template <typename Visitor>
static void visit_firm(Visitor *v, Firm *f)
{
	v->skip(4);  // virtual table pointer

	visit_baseobj(v, f);
	visit_place(v, f);

	visit<int16_t>(v, &f->firm_recno);

	visit<int8_t>(v, &f->firm_id);
	visit<int16_t>(v, &f->firm_build_id);

	visit<int16_t>(v, &f->upgrading_firm_id);
	visit<int16_t>(v, &f->upgrade_completion_percent);

	//-------- firm name vars ---------//

	visit<int16_t>(v, &f->closest_town_name_id);
	visit<int16_t>(v, &f->firm_name_instance_id);

	//-------- firm animation vars ---------//

	visit_array<int8_t>(v, f->firm_cur_frame, MAX_FRAME_COUNTERS);
	visit_array<int8_t>(v, f->firm_remain_frame_delay, MAX_FRAME_COUNTERS);

	//------ inter-relationship -------//

	visit<int8_t>(v, &f->linked_firm_count);
	visit<int8_t>(v, &f->linked_town_count);

	visit_array<int16_t>(v, f->linked_firm_array, MAX_LINKED_FIRM_FIRM);
	visit_array<int16_t>(v, f->linked_town_array, MAX_LINKED_FIRM_TOWN);

	visit_array<int8_t>(v, f->linked_firm_enable_array, MAX_LINKED_FIRM_FIRM);
	visit_array<int8_t>(v, f->linked_town_enable_array, MAX_LINKED_FIRM_TOWN);

	visit<int16_t>(v, &f->active_link_town_recno);

	//--------- financial vars ---------//

	visit<float>(v, &f->last_year_income);
	visit<float>(v, &f->cur_year_income);

	//---------- misc vars ------------//

	visit<int8_t>(v, &f->should_set_power);
	visit<int8_t>(v, &f->reveal_info);

	// --------- rally point vars -------//

	visit<int16_t>(v, &f->rally_enable_flag);
	visit<int16_t>(v, &f->rally_dest_base_obj_recno);
	visit<int16_t>(v, &f->rally_dest_x);
	visit<int16_t>(v, &f->rally_dest_y);

	//----------- AI vars ------------//

	visit<int8_t>(v, &f->ai_processed);
	visit<int8_t>(v, &f->ai_status);
	visit<int8_t>(v, &f->ai_link_checked);
	visit<int8_t>(v, &f->ai_sell_flag);

	visit<int8_t>(v, &f->should_close_flag);
	visit<int8_t>(v, &f->ai_should_build_factory_count);
}

enum { FIRM_RECORD_SIZE = 296 };

static bool read_firm(File *file, Firm *firm)
{
	return read_with_record_size(file, firm, &visit_firm<FileReaderVisitor>, FIRM_RECORD_SIZE);
}

static bool write_firm(File *file, Firm *firm)
{
	return write_with_record_size(file, firm, &visit_firm<FileWriterVisitor>, FIRM_RECORD_SIZE);
}

//-------- Start of function FirmArray::write_file -------------//
//
int FirmArray::write_file(File* filePtr)
{
   int  i;
   Firm *firmPtr;

   filePtr->file_put_short( size()  );  // no. of firms in firm_array
   filePtr->file_put_short( process_recno );
	filePtr->file_put_short( selected_recno );

	// BUGHERE 
//	filePtr->file_put_short( Firm::firm_menu_mode );
//	filePtr->file_put_short( Firm::action_spy_recno );
//	filePtr->file_put_short( Firm::bribe_result );
//	filePtr->file_put_short( Firm::assassinate_result );

	for( i=1; i<=size() ; i++ )
   {
      firmPtr = (Firm*) get_ptr(i);

      //----- write firmId or 0 if the firm is deleted -----//

      if( !firmPtr )    // the firm is deleted
		{
         filePtr->file_put_short(0);
      }
      else
      {
         //--------- write firm_id -------------//

         filePtr->file_put_short(firmPtr->firm_id);

         //------ write data in base class --------//

         if( !write_firm(filePtr, firmPtr) )
	    return 0;

         //--------- write worker_array ---------//

			// BUGHERE firmWorker
//         if( firmPtr->worker_array )
//         {
//            if( !filePtr->file_write( firmPtr->worker_array, MAX_WORKER*sizeof(Worker) ) )
//               return 0;
//         }

         //------ write data in derived class ------//

         if( !firmPtr->write_derived_file(filePtr) )
            return 0;
      }
   }

   //------- write empty room array --------//

	write_empty_room(filePtr);

   return 1;
}
//--------- End of function FirmArray::write_file ---------------//


//-------- Start of function FirmArray::read_file -------------//
//
int FirmArray::read_file(File* filePtr)
{
	Firm*   firmPtr;
	int     i, firmId, firmRecno;

	int firmCount      = filePtr->file_get_short();  // get no. of firms from file
	process_recno      = filePtr->file_get_short();
	selected_recno     = filePtr->file_get_short();

	// BUGHERE
//	Firm::firm_menu_mode  	 = (char) filePtr->file_get_short();
//	Firm::action_spy_recno   = filePtr->file_get_short();
//	Firm::bribe_result    	 = (char) filePtr->file_get_short();
//	Firm::assassinate_result = (char) filePtr->file_get_short();

   for( i=1 ; i<=firmCount ; i++ )
   {
      firmId = filePtr->file_get_short();

      if( firmId==0 )  // the firm has been deleted
      {
         add_blank(1);     // it's a DynArrayB function
      }
      else
      {
         //----- create firm object -----------//

         firmRecno = create_firm( firmId );
         firmPtr   = firm_array[firmRecno];

         //---- read data in base class -----//

	 if( !read_firm(filePtr, firmPtr) )
	    return 0;

         //--------- read worker_array ---------//

//         if( firm_res[firmId]->need_worker )
//         {
//            firmPtr->worker_array = (Worker*) mem_add( MAX_WORKER*sizeof(Worker) );
//
//            if( !filePtr->file_read( firmPtr->worker_array, MAX_WORKER*sizeof(Worker) ) )
//               return 0;
//         }

         //----- read data in derived class -----//

         if( !firmPtr->read_derived_file( filePtr ) )
            return 0;
			
			FirmBuild* firmBuild = firm_res.get_build(firmPtr->firm_build_id);
			firmBuild->load_bitmap_res();

      }
   }

   //-------- linkout() those record added by add_blank() ----------//
   //-- So they will be marked deleted in DynArrayB and can be -----//
   //-- undeleted and used when a new record is going to be added --//

   for( i=size() ; i>0 ; i-- )
   {
      DynArrayB::go(i);             // since FirmArray has its own go() which will call GroupArray::go()

      if( get_ptr() == NULL )       // add_blank() record
         linkout();
   }

   //------- read empty room array --------//

   read_empty_room(filePtr);

   return 1;
}
//--------- End of function FirmArray::read_file ---------------//


//--------- Begin of function Firm::write_derived_file ---------//
//
// Write data in derived class.
//
// If the derived Firm don't have any special data,
// just use Firm::write_file(), otherwise make its own derived copy of write_file()
//
int Firm::write_derived_file(File* filePtr)
{
   //--- write data in derived class -----//

   int writeSize = firm_array.firm_class_size(firm_id)-sizeof(Firm);

   if( writeSize > 0 )
   {
      if( !filePtr->file_write( (char*) this + sizeof(Firm), writeSize ) )
         return 0;
   }

   return 1;
}
//----------- End of function Firm::write_derived_file ---------//


//--------- Begin of function Firm::read_derived_file ---------//
//
// Read data in derived class.
//
// If the derived Firm don't have any special data,
// just use Firm::read_file(), otherwise make its own derived copy of read_file()
//
int Firm::read_derived_file(File* filePtr)
{
   //--- read data in derived class -----//

   int readSize = firm_array.firm_class_size(firm_id)-sizeof(Firm);

   if( readSize > 0 )
   {
      if( !filePtr->file_read( (char*) this + sizeof(Firm), readSize ) )
         return 0;
   }

   return 1;
}
//----------- End of function Firm::read_derived_file ---------//


//--------- Begin of function Firm::read_derived_file ---------//
//
int FirmAnimal::read_derived_file(File* filePtr)
{
	if( !Firm::read_derived_file(filePtr) )
		return 0;

	// -------- load bitmap ----------//

	// load bitmap of all three animals unit

	for( int i = 0; i < ANIMAL_TYPE_COUNT; ++i )
	{
		int spriteId = unit_res[animal_unit_id[i]]->sprite_id;
		sprite_res[spriteId]->load_bitmap_res();
	}

	return 1;
}
//--------- End of function Firm::read_derived_file ---------//

//*****//


//-------- Start of function SiteArray::write_file -------------//
//
int SiteArray::write_file(File* filePtr)
{
	filePtr->file_put_short(selected_recno);
	filePtr->file_put_short(untapped_raw_count);
	filePtr->file_put_short(scroll_count);
	filePtr->file_put_short(gold_coin_count);
	// ###### begin Gilbert 25/9 #######//
	filePtr->file_put_short(item_count);
	// ###### end Gilbert 25/9 #######//
	filePtr->file_put_short(std_raw_site_count);

	return DynArrayB::write_file( filePtr );
}
//--------- End of function SiteArray::write_file ---------------//


//-------- Start of function SiteArray::read_file -------------//
//
int SiteArray::read_file(File* filePtr)
{
	selected_recno		 = filePtr->file_get_short();
	untapped_raw_count =	filePtr->file_get_short();
	scroll_count		 = filePtr->file_get_short();
	gold_coin_count	 =	filePtr->file_get_short();
	// ###### begin Gilbert 25/9 #######//
	item_count         = filePtr->file_get_short();
	// ###### end Gilbert 25/9 #######//
	std_raw_site_count =	filePtr->file_get_short();

	return DynArrayB::read_file( filePtr );
}
//--------- End of function SiteArray::read_file ---------------//


//*****//


template <typename Visitor>
static void visit_town(Visitor *v, Town *t)
{
	v->skip(4);  // virtual table pointer

	visit_baseobj(v, t);
	visit_place(v, t);

	//------- basic vars --------//

	visit<int16_t>(v, &t->town_recno);
	visit<int16_t>(v, &t->town_name_id);
	visit<int8_t>(v, &t->is_base_town);
	visit<int8_t>(v, &t->construction_completion_percent);

	//------ town layout vars -------//

	visit<int16_t>(v, &t->layout_id);
	visit<int16_t>(v, &t->first_slot_id);
	visit_array<int16_t>(v, t->slot_object_id_array, MAX_TOWN_LAYOUT_SLOT);
	visit<int16_t>(v, &t->max_layout_pop);

	//------ primary town game vars ------//

	visit<int16_t>(v, &t->population);
	visit<int16_t>(v, &t->jobless_population);
	visit<int16_t>(v, &t->population_growth);
	visit<float>(v, &t->loyalty);
	visit<int16_t>(v, &t->target_loyalty);
	visit<int16_t>(v, &t->bonus_loyalty);
	visit_array<float>(v, t->resistance_array, MAX_NATION);
	visit_array<int16_t>(v, t->target_resistance_array, MAX_NATION);
	visit<int16_t>(v, &t->quality_of_life);

	//------- secondary game vars -------//

	visit<int16_t>(v, &t->spy_count);
	visit<int16_t>(v, &t->town_defender_count);
	visit<float>(v, &t->received_hit_count);

	// ------ wall ---------- //

	visit<int8_t>(v, &t->current_wall_level);
	visit<int8_t>(v, &t->target_wall_level);
	visit<int8_t>(v, &t->target_wall_man_power);
	visit<int16_t>(v, &t->upgrade_wall_progress);

	// ------- archer vars -------//

	visit<int16_t>(v, &t->archers_energy);
	visit<float>(v, &t->arrow_damage);
	visit<int16_t>(v, &t->target_count);
	visit_array<int16_t>(v, t->target_base_obj_recno, MAX_TOWN_TARGETS);

	//-------- rebel vars -------//

	visit<int16_t>(v, &t->rebel_recno);
	visit<int32_t>(v, &t->last_rebel_date);

	//-------- penalty vars ----------//

	visit<int32_t>(v, &t->accumulated_collect_tax_penalty);
	visit<int32_t>(v, &t->accumulated_reward_penalty);
	visit<int32_t>(v, &t->accumulated_recruit_penalty);
	visit<int32_t>(v, &t->accumulated_enemy_grant_penalty);

	//----- independent town/unit vars -----//

	visit<int8_t>(v, &t->independ_town_nation_relation);
	visit<int16_t>(v, &t->independent_unit_join_nation_min_rating);

	//------- auto policy -------------//

	visit<int16_t>(v, &t->auto_collect_tax_loyalty);
	visit<int16_t>(v, &t->auto_grant_loyalty);

	//------ inter-relationship -------//

	visit<int16_t>(v, &t->linked_firm_count);
	visit<int16_t>(v, &t->linked_town_count);
	visit_array<int16_t>(v, t->linked_firm_array, MAX_LINKED_FIRM_TOWN);
	visit_array<int16_t>(v, t->linked_town_array, MAX_LINKED_TOWN_TOWN);
	visit_array<int8_t>(v, t->linked_firm_enable_array, MAX_LINKED_FIRM_TOWN);
	visit_array<int8_t>(v, t->linked_town_enable_array, MAX_LINKED_TOWN_TOWN);
	visit<int8_t>(v, &t->has_linked_own_camp);
	visit<int8_t>(v, &t->has_linked_enemy_camp);

	//----------- AI vars ------------//

	visit<int8_t>(v, &t->ai_town);
	visit<int8_t>(v, &t->ai_link_checked);
	visit<int8_t>(v, &t->town_combat_level);
	visit_array<int8_t>(v, t->has_product_supply, MAX_PRODUCT);
}


enum { TOWN_RECORD_SIZE = 402 };


static bool read_town(File *file, Town *town)
{
	return read_with_record_size(file, town, &visit_town<FileReaderVisitor>, TOWN_RECORD_SIZE);
}


static bool write_town(File *file, Town *town)
{
	return write_with_record_size(file, town, &visit_town<FileWriterVisitor>, TOWN_RECORD_SIZE);
}


//-------- Start of function TownArray::write_file -------------//
//
int TownArray::write_file(File* filePtr)
{
   int  i;
   Town *townPtr;

	filePtr->file_put_short( size()  );  // no. of towns in town_array
	filePtr->file_put_short( selected_recno );
	filePtr->file_write( race_wander_pop_array, sizeof(race_wander_pop_array) );

	filePtr->file_put_short( Town::if_town_recno );

	//-----------------------------------------//

	for( i=1; i<=size() ; i++ )
	{
		townPtr = (Town*) get_ptr(i);

      //----- write townId or 0 if the town is deleted -----//

      if( !townPtr )    // the town is deleted
      {
         filePtr->file_put_short(0);
      }
      else
		{
//			#ifdef DEBUG
//				townPtr->verify_slot_object_id_array();		// for debugging only
//			#endif

			filePtr->file_put_short(1);      // the town exists

         if( !write_town(filePtr, townPtr) )
            return 0;
      }
   }

   //------- write empty room array --------//

   write_empty_room(filePtr);

   return 1;
}
//--------- End of function TownArray::write_file ---------------//


//-------- Start of function TownArray::read_file -------------//
//
int TownArray::read_file(File* filePtr)
{
   Town*   townPtr;
   int     i;

	int townCount = filePtr->file_get_short();  // get no. of towns from file
	selected_recno = filePtr->file_get_short();

	filePtr->file_read( race_wander_pop_array, sizeof(race_wander_pop_array) );

	Town::if_town_recno = filePtr->file_get_short();

	//------------------------------------------//

	for( i=1 ; i<=townCount ; i++ )
	{
		if( filePtr->file_get_short()==0 )  // the town has been deleted
		{
			add_blank(1);     // it's a DynArrayB function
		}
		else
		{
			townPtr = town_array.create_town();

			if( !read_town(filePtr, townPtr) )
				return 0;


			// ###### end Gilbert 25/9 #######//
//			#ifdef DEBUG
//				townPtr->verify_slot_object_id_array();		// for debugging only
//			#endif
		}
	}

	//-------- linkout() those record added by add_blank() ----------//
	//-- So they will be marked deleted in DynArrayB and can be -----//
	//-- undeleted and used when a new record is going to be added --//

   for( i=size() ; i>0 ; i-- )
   {
      DynArrayB::go(i);             // since TownArray has its own go() which will call GroupArray::go()

      if( get_ptr() == NULL )       // add_blank() record
         linkout();
   }

   //------- read empty room array --------//

   read_empty_room(filePtr);

   return 1;
}
//--------- End of function TownArray::read_file ---------------//


//*****//


template <typename Visitor>
static void visit_nation_array(Visitor *v, NationArray *na)
{
   /* DynArray and DynArrayB skipped */

   visit<int16_t>(v, &na->nation_count);
   visit<int16_t>(v, &na->ai_nation_count);
   visit<int32_t>(v, &na->last_del_nation_date);
   visit<int32_t>(v, &na->last_new_nation_date);
   visit<int32_t>(v, &na->max_nation_population);
   visit<int32_t>(v, &na->all_nation_population);
   visit<int16_t>(v, &na->independent_town_count);
   visit_array<int16_t>(v, na->independent_town_count_race_array, MAX_RACE);
   visit<int32_t>(v, &na->max_nation_units);
   visit<int32_t>(v, &na->max_nation_humans);
   visit<int32_t>(v, &na->max_nation_generals);
   visit<int32_t>(v, &na->max_nation_weapons);
   visit<int32_t>(v, &na->max_nation_ships);
   visit<int32_t>(v, &na->max_nation_spies);
   visit<int32_t>(v, &na->max_nation_firms);
   visit<int32_t>(v, &na->max_nation_tech_level);
   visit<int32_t>(v, &na->max_population_rating);
   visit<int32_t>(v, &na->max_military_rating);
   visit<int32_t>(v, &na->max_economic_rating);
   visit<int32_t>(v, &na->max_reputation);
   visit<int32_t>(v, &na->max_kill_monster_score);
   visit<int32_t>(v, &na->max_overall_rating);
   visit<int16_t>(v, &na->max_population_nation_recno);
   visit<int16_t>(v, &na->max_military_nation_recno);
   visit<int16_t>(v, &na->max_economic_nation_recno);
   visit<int16_t>(v, &na->max_reputation_nation_recno);
   visit<int16_t>(v, &na->max_kill_monster_nation_recno);
   visit<int16_t>(v, &na->max_overall_nation_recno);
   visit<int32_t>(v, &na->last_alliance_id);
   visit<int32_t>(v, &na->nation_peace_days);
   visit<int16_t>(v, &na->player_recno);
   visit_pointer(v, &na->player_ptr);
   visit_array<int8_t>(v, na->nation_color_array, MAX_NATION+1);
   visit_array<int8_t>(v, na->nation_power_color_array, MAX_NATION+2);

   for( int n = 0; n < MAX_NATION; n++ )
   {
      visit_array<int8_t>(v, na->custom_nation_name_array[n], NationArray::CUSTOM_NAME_LEN+1);
      visit_array<int8_t>(v, na->custom_king_name_array[n], NationArray::CUSTOM_NAME_LEN+1);
   }
}

enum { NATION_ARRAY_RECORD_SIZE = 859 };


//-------- Start of function NationArray::write_file -------------//
//
int NationArray::write_file(File* filePtr)
{
	//------ write info in NationArray ------//

   if( !write_with_record_size( filePtr,
                                this,
                                &visit_nation_array<FileWriterVisitor>,
                                NATION_ARRAY_RECORD_SIZE ) )
      return 0;

   //---------- write Nations --------------//

   int    i;
   Nation *nationPtr;

   filePtr->file_put_short( size() );  // no. of nations in nation_array

   for( i=1; i<=size() ; i++ )
   {
      nationPtr = (Nation*) get_ptr(i);

      //----- write nationId or 0 if the nation is deleted -----//

      if( !nationPtr )    // the nation is deleted
      {
         filePtr->file_put_short(0);
      }
      else
      {
         filePtr->file_put_short(nationPtr->nation_class_id);      // there is a nation in this record

         //------ write data in the base class ------//

         if( !nationPtr->write_file(filePtr) )
            return 0;
      }
   }

   //------- write empty room array --------//

   write_empty_room(filePtr);

   return 1;
}
//--------- End of function NationArray::write_file -------------//


//-------- Start of function NationArray::read_file -------------//
//
int NationArray::read_file(File* filePtr)
{
   //------ read info in NationArray ------//

   if( !read_with_record_size( filePtr,
                               this,
                               &visit_nation_array<FileReaderVisitor>,
                               NATION_ARRAY_RECORD_SIZE ) )
      return 0;

   //---------- read Nations --------------//

   int     i, nationRecno, nationCount;
   Nation* nationPtr;

   nationCount = filePtr->file_get_short();  // get no. of nations from file

   for( i=1 ; i<=nationCount ; i++ )
   {
		int nationClassId = filePtr->file_get_short();

      if( !nationClassId )
      {
         add_blank(1);     // it's a DynArrayB function
      }
      else
      {
         //----- create nation object -----------//

         nationRecno = create_nation(nationClassId);
         nationPtr   = nation_array[nationRecno];

         //----- read data in base class --------//

         if( !nationPtr->read_file( filePtr ) )
            return 0;
      }
   }

   //-------- linkout() those record added by add_blank() ----------//
   //-- So they will be marked deleted in DynArrayB and can be -----//
   //-- undeleted and used when a new record is going to be added --//

   for( i=size() ; i>0 ; i-- )
   {
      DynArrayB::go(i);             // since NationArray has its own go() which will call GroupArray::go()

      if( get_ptr() == NULL )       // add_blank() record
         linkout();
   }

	//-------- set NationArray::player_ptr -----------//

   player_ptr = nation_array[player_recno];

	//------- read empty room array --------//

	read_empty_room(filePtr);

	return 1;
}
//--------- End of function NationArray::read_file ---------------//


template <typename Visitor>
static void visit_nation_relation(Visitor *v, NationRelation *nr)
{
	visit<int8_t>(v, &nr->has_contact);
	visit<int8_t>(v, &nr->should_attack);
	visit<int8_t>(v, &nr->trade_treaty);
	visit<int8_t>(v, &nr->status);
	visit<int32_t>(v, &nr->last_change_status_date);
	visit<int8_t>(v, &nr->ai_relation_level);
	visit<int8_t>(v, &nr->ai_secret_attack);
	visit<int8_t>(v, &nr->ai_demand_trade_treaty);
	visit<float>(v, &nr->good_relation_duration_rating);
	visit<int16_t>(v, &nr->started_war_on_us_count);
	visit_array<float>(v, nr->cur_year_import, IMPORT_TYPE_COUNT);
	visit_array<float>(v, nr->last_year_import, IMPORT_TYPE_COUNT);
	visit_array<float>(v, nr->lifetime_import, IMPORT_TYPE_COUNT);
	visit_array<int32_t>(v, nr->last_talk_reject_date_array, MAX_TALK_TYPE);
	visit_array<int32_t>(v, nr->never_accept_until_date_array, MAX_TALK_TYPE);
	visit<int32_t>(v, &nr->last_military_aid_date);
	visit_array<int8_t>(v, nr->ai_never_consider, MAX_TALK_TYPE);
	visit<int32_t>(v, &nr->last_give_gift_date);
	visit<int16_t>(v, &nr->total_given_gift_amount);
	visit<int8_t>(v, &nr->contact_msg_flag);
}


template <typename Visitor>
static void visit_ai_region(Visitor *v, AIRegion *reg)
{
	visit<uint8_t>(v, &reg->region_id);
	visit<int8_t>(v, &reg->town_count);
	visit<int8_t>(v, &reg->base_town_count);
}


template <typename Visitor>
static void visit_attack_camp(Visitor *v, AttackCamp *ac)
{
	visit<int16_t>(v, &ac->firm_recno);
	visit<int16_t>(v, &ac->combat_level);
	visit<int16_t>(v, &ac->distance);
	visit<int32_t>(v, &ac->patrol_date);
}


template <typename Visitor>
static void visit_nation(Visitor *v, Nation *nat)
{
	v->skip(4);  // virtual table pointer

	/* NationBase */
	visit<int16_t>(v, &nat->nation_recno);
	visit<int8_t>(v, &nat->nation_type);
	visit<int16_t>(v, &nat->campaign_nation_recno);
	visit<int8_t>(v, &nat->race_id);
	visit<int8_t>(v, &nat->color_scheme_id);
	visit<int8_t>(v, &nat->nation_color);
	visit<int16_t>(v, &nat->king_unit_recno);
	visit<int16_t>(v, &nat->king_leadership);
	visit_array<int8_t>(v, nat->nation_name_str, Nation::NATION_NAME_LEN+1);
	visit<uint32_t>(v, &nat->player_id);
	visit<int8_t>(v, &nat->next_frame_ready);
	visit<int16_t>(v, &nat->last_caravan_id);
	visit<int16_t>(v, &nat->nation_firm_count);
	visit<int32_t>(v, &nat->last_build_firm_date);
	visit_array<int8_t>(v, nat->know_base_array, MAX_RACE);
	visit_array<int8_t>(v, nat->base_count_array, MAX_RACE);
	visit<int8_t>(v, &nat->is_at_war_today);
	visit<int8_t>(v, &nat->is_at_war_yesterday);
	visit<int32_t>(v, &nat->last_war_date);
	visit<int16_t>(v, &nat->last_attacker_obj_recno);
	visit<int32_t>(v, &nat->last_independent_unit_join_date);
	visit<int8_t>(v, &nat->cheat_enabled_flag);
	visit<float>(v, &nat->cash);
	visit<float>(v, &nat->food);
	visit<float>(v, &nat->increased_cash);
	visit<float>(v, &nat->reputation);
	visit<float>(v, &nat->live_points);
	visit<float>(v, &nat->kill_monster_score);
	visit<int16_t>(v, &nat->auto_retreat_hit_point);
	visit<int16_t>(v, &nat->auto_collect_tax_loyalty);
	visit<int16_t>(v, &nat->auto_grant_loyalty);
	visit<int16_t>(v, &nat->research_tech_id);
	visit<float>(v, &nat->fraction_research_progress);
	visit<float>(v, &nat->cur_year_profit);
	visit<float>(v, &nat->last_year_profit);
	visit<float>(v, &nat->cur_year_fixed_income);
	visit<float>(v, &nat->last_year_fixed_income);
	visit<float>(v, &nat->cur_year_fixed_expense);
	visit<float>(v, &nat->last_year_fixed_expense);
	visit_array<float>(v, nat->cur_year_income_array, INCOME_TYPE_COUNT);
	visit_array<float>(v, nat->last_year_income_array, INCOME_TYPE_COUNT);
	visit<float>(v, &nat->cur_year_income);
	visit<float>(v, &nat->last_year_income);
	visit_array<float>(v, nat->cur_year_expense_array, EXPENSE_TYPE_COUNT);
	visit_array<float>(v, nat->last_year_expense_array, EXPENSE_TYPE_COUNT);
	visit<float>(v, &nat->cur_year_expense);
	visit<float>(v, &nat->last_year_expense);
	visit<float>(v, &nat->cur_year_cheat);
	visit<float>(v, &nat->last_year_cheat);
	visit<float>(v, &nat->cur_year_food_in);
	visit<float>(v, &nat->last_year_food_in);
	visit<float>(v, &nat->cur_year_food_out);
	visit<float>(v, &nat->last_year_food_out);
	visit<float>(v, &nat->cur_year_food_change);
	visit<float>(v, &nat->last_year_food_change);
	visit<float>(v, &nat->cur_year_reputation_change);
	visit<float>(v, &nat->last_year_reputation_change);
	visit<float>(v, &nat->cur_year_live_points_change);
	visit<float>(v, &nat->last_year_live_points_change);

	for( int n = 0; n < MAX_NATION; n++ )
		visit_nation_relation(v, &nat->relation_array[n]);

	visit_array<int8_t>(v, nat->relation_status_array, MAX_NATION);
	visit_array<int8_t>(v, nat->relation_passable_array, MAX_NATION);
	visit_array<int8_t>(v, nat->relation_should_attack_array, MAX_NATION);
	visit<int8_t>(v, &nat->is_allied_with_player);
	visit<int32_t>(v, &nat->total_population);
	visit<int32_t>(v, &nat->total_jobless_population);
	visit<int32_t>(v, &nat->total_unit_count);
	visit<int32_t>(v, &nat->total_human_count);
	visit<int32_t>(v, &nat->total_monster_count);
	visit<int32_t>(v, &nat->total_general_count);
	visit<int32_t>(v, &nat->total_weapon_count);
	visit<int32_t>(v, &nat->total_ship_count);
	visit<int32_t>(v, &nat->total_firm_count);
	visit<int32_t>(v, &nat->total_spy_count);
	visit<int32_t>(v, &nat->total_ship_combat_level);
	visit<int16_t>(v, &nat->largest_town_recno);
	visit<int16_t>(v, &nat->largest_town_pop);
	visit_array<int16_t>(v, nat->raw_count_array, MAX_RAW);
	visit_array<int16_t>(v, nat->last_unit_name_id_array, MAX_UNIT_TYPE);
	visit<int32_t>(v, &nat->population_rating);
	visit<int32_t>(v, &nat->military_rating);
	visit<int32_t>(v, &nat->economic_rating);
	visit<int32_t>(v, &nat->overall_rating);
	visit<int32_t>(v, &nat->enemy_soldier_killed);
	visit<int32_t>(v, &nat->own_soldier_killed);
	visit<int32_t>(v, &nat->enemy_frythan_killed);
	visit<int32_t>(v, &nat->own_frythan_killed);
	visit<int32_t>(v, &nat->enemy_civilian_killed);
	visit<int32_t>(v, &nat->own_civilian_killed);
	visit<int32_t>(v, &nat->enemy_weapon_destroyed);
	visit<int32_t>(v, &nat->own_weapon_destroyed);
	visit<int32_t>(v, &nat->enemy_ship_destroyed);
	visit<int32_t>(v, &nat->own_ship_destroyed);
	visit<int32_t>(v, &nat->enemy_firm_destroyed);
	visit<int32_t>(v, &nat->own_firm_destroyed);

	/* Nation */
	visit<int16_t>(v, &nat->nation_class_id);
	v->skip(40); /* action_array */
	visit<uint16_t>(v, &nat->last_action_id);

	//------------------------------------------------------//
	// array used to store the info. of the firms
	//------------------------------------------------------//

	visit_pointer(v, &nat->ai_town_array);
	visit_pointer(v, &nat->ai_base_array);
	visit_pointer(v, &nat->ai_camp_array);
	visit_pointer(v, &nat->ai_mine_array);
	visit_pointer(v, &nat->ai_factory_array);
	visit_pointer(v, &nat->ai_market_array);
	visit_pointer(v, &nat->ai_research_array);
	visit_pointer(v, &nat->ai_war_array);
	visit_pointer(v, &nat->ai_inn_array);
	visit_pointer(v, &nat->ai_spy_array);
	visit_pointer(v, &nat->ai_special_array);
	visit_pointer(v, &nat->ai_offend_array);
	visit_pointer(v, &nat->ai_lishorr_array);
	visit_pointer(v, &nat->ai_mons_train_array);
	visit_pointer(v, &nat->ai_mons_alchemy_array);
	visit_pointer(v, &nat->ai_mons_fort_array);
	visit_pointer(v, &nat->ai_mons_animal_array);
	visit_pointer(v, &nat->ai_incubator_array);
	visit_pointer(v, &nat->ai_mons_magic_array);
	visit_pointer(v, &nat->ai_monster_array);
	visit_pointer(v, &nat->ai_general_array);
	visit_pointer(v, &nat->ai_caravan_array);
	visit_pointer(v, &nat->ai_ship_array);

	//--------------------------------------------------------//
	// parameters used to make decisions
	//--------------------------------------------------------//

	visit<int16_t>(v, &nat->ai_town_size);
	visit<int16_t>(v, &nat->ai_base_size);
	visit<int16_t>(v, &nat->ai_camp_size);
	visit<int16_t>(v, &nat->ai_mine_size);
	visit<int16_t>(v, &nat->ai_factory_size);
	visit<int16_t>(v, &nat->ai_market_size);
	visit<int16_t>(v, &nat->ai_research_size);
	visit<int16_t>(v, &nat->ai_war_size);
	visit<int16_t>(v, &nat->ai_inn_size);
	visit<int16_t>(v, &nat->ai_spy_size);
	visit<int16_t>(v, &nat->ai_special_size);
	visit<int16_t>(v, &nat->ai_offend_size);
	visit<int16_t>(v, &nat->ai_lishorr_size);
	visit<int16_t>(v, &nat->ai_mons_train_size);
	visit<int16_t>(v, &nat->ai_mons_alchemy_size);
	visit<int16_t>(v, &nat->ai_mons_fort_size);
	visit<int16_t>(v, &nat->ai_mons_animal_size);
	visit<int16_t>(v, &nat->ai_incubator_size);
	visit<int16_t>(v, &nat->ai_mons_magic_size);
	visit<int16_t>(v, &nat->ai_monster_size);
	visit<int16_t>(v, &nat->ai_general_size);
	visit<int16_t>(v, &nat->ai_caravan_size);
	visit<int16_t>(v, &nat->ai_ship_size);
	visit<int16_t>(v, &nat->ai_town_count);
	visit<int16_t>(v, &nat->ai_base_count);
	visit<int16_t>(v, &nat->ai_camp_count);
	visit<int16_t>(v, &nat->ai_mine_count);
	visit<int16_t>(v, &nat->ai_factory_count);
	visit<int16_t>(v, &nat->ai_market_count);
	visit<int16_t>(v, &nat->ai_research_count);
	visit<int16_t>(v, &nat->ai_war_count);
	visit<int16_t>(v, &nat->ai_inn_count);
	visit<int16_t>(v, &nat->ai_spy_count);
	visit<int16_t>(v, &nat->ai_special_count);
	visit<int16_t>(v, &nat->ai_offend_count);
	visit<int16_t>(v, &nat->ai_lishorr_count);
	visit<int16_t>(v, &nat->ai_mons_train_count);
	visit<int16_t>(v, &nat->ai_mons_alchemy_count);
	visit<int16_t>(v, &nat->ai_mons_fort_count);
	visit<int16_t>(v, &nat->ai_mons_animal_count);
	visit<int16_t>(v, &nat->ai_incubator_count);
	visit<int16_t>(v, &nat->ai_mons_magic_count);
	visit<int16_t>(v, &nat->ai_monster_count);
	visit<int16_t>(v, &nat->ai_general_count);
	visit<int16_t>(v, &nat->ai_caravan_count);
	visit<int16_t>(v, &nat->ai_ship_count);
	visit<int16_t>(v, &nat->ai_base_town_count);
	visit_array<int16_t>(v, nat->firm_should_close_array, MAX_FIRM_TYPE);

	//------------------------------------------------------//
	// parameters about the nation itself
	//------------------------------------------------------//

	for( int n = 0; n < MAX_AI_REGION; n++ )
		visit_ai_region(v, &nat->ai_region_array[n]);
	visit<int8_t>(v, &nat->ai_region_count);

	//------------------------------------------------------//
	// AI unitalties
	//------------------------------------------------------//

	visit<int8_t>(v, &nat->pref_force_projection);
	visit<int8_t>(v, &nat->pref_military_development);
	visit<int8_t>(v, &nat->pref_economic_development);
	visit<int8_t>(v, &nat->pref_inc_pop_by_capture);
	visit<int8_t>(v, &nat->pref_inc_pop_by_growth);
	visit<int8_t>(v, &nat->pref_peacefulness);
	visit<int8_t>(v, &nat->pref_military_courage);
	visit<int8_t>(v, &nat->pref_territorial_cohesiveness);
	visit<int8_t>(v, &nat->pref_trading_tendency);
	visit<int8_t>(v, &nat->pref_allying_tendency);
	visit<int8_t>(v, &nat->pref_honesty);
	visit<int8_t>(v, &nat->pref_town_defense);
	visit<int8_t>(v, &nat->pref_loyalty_concern);
	visit<int8_t>(v, &nat->pref_forgiveness);
	visit<int8_t>(v, &nat->pref_collect_tax);
	visit<int8_t>(v, &nat->pref_hire_unit);
	visit<int8_t>(v, &nat->pref_use_weapon);
	visit<int8_t>(v, &nat->pref_keep_general);
	visit<int8_t>(v, &nat->pref_keep_skilled_unit);
	visit<int8_t>(v, &nat->pref_diplomacy_retry);
	visit<int8_t>(v, &nat->pref_attack_monster);
	visit<int8_t>(v, &nat->pref_spy);
	visit<int8_t>(v, &nat->pref_counter_spy);
	visit<int8_t>(v, &nat->pref_food_reserve);
	visit<int8_t>(v, &nat->pref_cash_reserve);
	visit<int8_t>(v, &nat->pref_use_marine);
	visit<int8_t>(v, &nat->pref_unit_chase_distance);
	visit<int8_t>(v, &nat->pref_repair_concern);
	visit<int8_t>(v, &nat->pref_scout);
	visit<int8_t>(v, &nat->pref_research);
	visit<int8_t>(v, &nat->pref_seat_of_power);
	visit<int8_t>(v, &nat->pref_monster_aggressiveness);
	visit<int8_t>(v, &nat->pref_monster_special);
	visit<int8_t>(v, &nat->pref_live_points_reserve);
	visit<int8_t>(v, &nat->pref_surrender);

	//------- AI action vars --------//

	visit<int16_t>(v, &nat->ai_capture_enemy_town_recno);
	visit<int32_t>(v, &nat->ai_capture_enemy_town_plan_date);
	visit<int32_t>(v, &nat->ai_capture_enemy_town_start_attack_date);
	visit<int8_t>(v, &nat->ai_capture_enemy_town_use_all_camp);
	visit<int32_t>(v, &nat->ai_last_defend_action_date);
	visit_array<int16_t>(v, nat->ai_attack_target_x_loc, MAX_AI_ATTACK_TARGET);
	visit_array<int16_t>(v, nat->ai_attack_target_y_loc, MAX_AI_ATTACK_TARGET);
	visit<int16_t>(v, &nat->ai_attack_target_count);
	visit<int16_t>(v, &nat->ai_attack_target_nation_recno);

	for( int n = 0; n < MAX_SUITABLE_ATTACK_CAMP; n++ )
		visit_attack_camp(v, &nat->attack_camp_array[n]);

	visit<int16_t>(v, &nat->attack_camp_count);
	visit<int16_t>(v, &nat->lead_attack_camp_recno);
	visit<int32_t>(v, &nat->ai_last_build_lishorr_net_power);
}


enum { NATION_RECORD_SIZE = 3180 };


//--------- Begin of function Nation::write_file ---------//
//
int Nation::write_file(File* filePtr)
{
	if( !write_with_record_size( filePtr,
				     this,
				     &visit_nation<FileWriterVisitor>,
				     NATION_RECORD_SIZE ) )
		return 0;

	//----------- write AI Action Array ------------//

	action_array.write_file(filePtr);

	//------ write AI info array ---------//

	write_ai_info(filePtr, ai_town_array, ai_town_count, ai_town_size);

	write_ai_info(filePtr, ai_base_array, ai_base_count, ai_base_size);
	write_ai_info(filePtr, ai_camp_array, ai_camp_count, ai_camp_size);
	write_ai_info(filePtr, ai_mine_array, ai_mine_count, ai_mine_size);
	write_ai_info(filePtr, ai_factory_array, ai_factory_count, ai_factory_size);
	write_ai_info(filePtr, ai_market_array, ai_market_count, ai_market_size);
	write_ai_info(filePtr, ai_research_array, ai_research_count, ai_research_size);
	write_ai_info(filePtr, ai_war_array, ai_war_count, ai_war_size);
	write_ai_info(filePtr, ai_inn_array, ai_inn_count, ai_inn_size);
	write_ai_info(filePtr, ai_spy_array, ai_spy_count, ai_spy_size);
	write_ai_info(filePtr, ai_special_array, ai_special_count, ai_special_size);
	// ######## begin Gilbert 4/5 ##########//
	write_ai_info(filePtr, ai_offend_array, ai_offend_count, ai_offend_size);
	// ######## end Gilbert 4/5 ##########//
	// ######## begin Gilbert 10/3 ##########//
	write_ai_info(filePtr, ai_lishorr_array, ai_lishorr_count, ai_lishorr_size);
	// ######## end Gilbert 10/3 ##########//
	write_ai_info(filePtr, ai_mons_train_array, ai_mons_train_count, ai_mons_train_size);
	write_ai_info(filePtr, ai_mons_alchemy_array, ai_mons_alchemy_count, ai_mons_alchemy_size);
	write_ai_info(filePtr, ai_mons_fort_array, ai_mons_fort_count, ai_mons_fort_size);
	write_ai_info(filePtr, ai_mons_animal_array, ai_mons_animal_count, ai_mons_animal_size);
	write_ai_info(filePtr, ai_incubator_array, ai_incubator_count, ai_incubator_size);
	write_ai_info(filePtr, ai_mons_magic_array, ai_mons_magic_count, ai_mons_magic_size);
	write_ai_info(filePtr, ai_monster_array, ai_monster_count, ai_monster_size);
	write_ai_info(filePtr, ai_caravan_array, ai_caravan_count, ai_caravan_size);
	write_ai_info(filePtr, ai_general_array, ai_general_count, ai_general_size);
	write_ai_info(filePtr, ai_ship_array, ai_ship_count, ai_ship_size);

	return 1;
}
//----------- End of function Nation::write_file ---------//


//--------- Begin of static function write_ai_info ---------//
//
static void write_ai_info(File* filePtr, short* aiInfoArray, short aiInfoCount, short aiInfoSize)
{
	filePtr->file_put_short( aiInfoCount );
	filePtr->file_put_short( aiInfoSize  );
	filePtr->file_write( aiInfoArray, sizeof(short) * aiInfoCount );
}
//----------- End of static function write_ai_info ---------//


//--------- Begin of function Nation::read_file ---------//
//
int Nation::read_file(File* filePtr)
{
	//---- save the action_array first before loading in the whole Nation class ----//

	char* saveActionArray = (char*) mem_add( sizeof(DynArrayB) );

	memcpy( saveActionArray, &action_array, sizeof(DynArrayB) );

	//--------------------------------------------------//

	if( !read_with_record_size( filePtr,
				    this,
				    &visit_nation<FileReaderVisitor>,
				    NATION_RECORD_SIZE ) )
		return 0;

	//---------- restore action_array  ------------//

	memcpy( &action_array, saveActionArray, sizeof(DynArrayB) );

	mem_del( saveActionArray );

	//-------------- read AI Action Array --------------//

	action_array.read_file(filePtr);

	//------ write AI info array ---------//

	read_ai_info(filePtr, &ai_town_array, ai_town_count, ai_town_size);

	read_ai_info(filePtr, &ai_base_array, ai_base_count, ai_base_size);
	read_ai_info(filePtr, &ai_camp_array, ai_camp_count, ai_camp_size);
	read_ai_info(filePtr, &ai_mine_array, ai_mine_count, ai_mine_size);
	read_ai_info(filePtr, &ai_factory_array, ai_factory_count, ai_factory_size);
	read_ai_info(filePtr, &ai_market_array, ai_market_count, ai_market_size);
	read_ai_info(filePtr, &ai_research_array, ai_research_count, ai_research_size);
	read_ai_info(filePtr, &ai_war_array, ai_war_count, ai_war_size);
	read_ai_info(filePtr, &ai_inn_array, ai_inn_count, ai_inn_size);
	read_ai_info(filePtr, &ai_spy_array, ai_spy_count, ai_spy_size);
	read_ai_info(filePtr, &ai_special_array, ai_special_count, ai_special_size);
	// ######## begin Gilbert 4/5 ##########//
	read_ai_info(filePtr, &ai_offend_array, ai_offend_count, ai_offend_size);
	// ######## end Gilbert 4/5 ##########//
	// ######## begin Gilbert 10/3 ########//
	read_ai_info(filePtr, &ai_lishorr_array, ai_lishorr_count, ai_lishorr_size);
	// ######## end Gilbert 10/3 ########//
	read_ai_info(filePtr, &ai_mons_train_array, ai_mons_train_count, ai_mons_train_size);
	read_ai_info(filePtr, &ai_mons_alchemy_array, ai_mons_alchemy_count, ai_mons_alchemy_size);
	read_ai_info(filePtr, &ai_mons_fort_array, ai_mons_fort_count, ai_mons_fort_size);
	read_ai_info(filePtr, &ai_mons_animal_array, ai_mons_animal_count, ai_mons_animal_size);
	read_ai_info(filePtr, &ai_incubator_array, ai_incubator_count, ai_incubator_size);
	read_ai_info(filePtr, &ai_mons_magic_array, ai_mons_magic_count, ai_mons_magic_size);
	read_ai_info(filePtr, &ai_monster_array, ai_monster_count, ai_monster_size);
	read_ai_info(filePtr, &ai_caravan_array, ai_caravan_count, ai_caravan_size);
	read_ai_info(filePtr, &ai_general_array, ai_general_count, ai_general_size);
	read_ai_info(filePtr, &ai_ship_array, ai_ship_count, ai_ship_size);

	return 1;
}
//----------- End of function Nation::read_file ---------//


//--------- Begin of static function read_ai_info ---------//
//
static void read_ai_info(File* filePtr, short** aiInfoArrayPtr, short& aiInfoCount, short& aiInfoSize)
{
	aiInfoCount = filePtr->file_get_short();
	aiInfoSize  = filePtr->file_get_short();

	*aiInfoArrayPtr = (short*) mem_add( aiInfoSize * sizeof(short) );

	filePtr->file_read( *aiInfoArrayPtr, sizeof(short) * aiInfoCount );
}
//----------- End of static function read_ai_info ---------//

//*****//

//-------- Start of function TornadoArray::write_file -------------//
//
int TornadoArray::write_file(File* filePtr)
{
	int    i;
   Tornado *tornadoPtr;

   filePtr->file_put_short( size() );  // no. of tornados in tornado_array

   for( i=1; i<=size() ; i++ )
   {
      tornadoPtr = (Tornado*) get_ptr(i);

      //----- write tornadoId or 0 if the tornado is deleted -----//

      if( !tornadoPtr )    // the tornado is deleted
      {
         filePtr->file_put_short(0);
      }
      else
      {
         filePtr->file_put_short(1);      // there is a tornado in this record

         //------ write data in the base class ------//

         if( !tornadoPtr->write_file(filePtr) )
            return 0;
      }
   }

   //------- write empty room array --------//

   write_empty_room(filePtr);

   return 1;
}
//--------- End of function TornadoArray::write_file -------------//


//-------- Start of function TornadoArray::read_file -------------//
//
int TornadoArray::read_file(File* filePtr)
{
	int     i, tornadoRecno, tornadoCount;
   Tornado* tornadoPtr;

   tornadoCount = filePtr->file_get_short();  // get no. of tornados from file

   for( i=1 ; i<=tornadoCount ; i++ )
   {
      if( filePtr->file_get_short() == 0 )
      {
			add_blank(1);     // it's a DynArrayB function
      }
      else
      {
         //----- create tornado object -----------//

         tornadoRecno = tornado_array.create_tornado();
         tornadoPtr   = tornado_array[tornadoRecno];

         //----- read data in base class --------//

         if( !tornadoPtr->read_file( filePtr ) )
            return 0;
      }
   }

   //-------- linkout() those record added by add_blank() ----------//
	//-- So they will be marked deleted in DynArrayB and can be -----//
   //-- undeleted and used when a new record is going to be added --//

   for( i=size() ; i>0 ; i-- )
   {
      DynArrayB::go(i);             // since TornadoArray has its own go() which will call GroupArray::go()

      if( get_ptr() == NULL )       // add_blank() record
         linkout();
   }

   //------- read empty room array --------//

   read_empty_room(filePtr);

   return 1;
}
//--------- End of function TornadoArray::read_file ---------------//


template <typename Visitor>
static void visit_tornado(Visitor *v, Tornado *t)
{
   v->skip(4);  // virtual table pointer

   visit_baseobj(v, t);
   visit_sprite(v, t);

   visit<float>(v, &t->attack_damage);
   visit<int16_t>(v, &t->life_time);
   visit<int16_t>(v, &t->dmg_offset_x);
   visit<int16_t>(v, &t->dmg_offset_y);
   visit<int16_t>(v, &t->damage_player_flag);
}

enum { TORNADO_RECORD_SIZE = 72 };


//--------- Begin of function Tornado::write_file ---------//
//
int Tornado::write_file(File* filePtr)
{
   return write_with_record_size( filePtr,
                                  this,
                                  &visit_tornado<FileWriterVisitor>,
                                  TORNADO_RECORD_SIZE );
}
//----------- End of function Tornado::write_file ---------//


//--------- Begin of function Tornado::read_file ---------//
//
int Tornado::read_file(File* filePtr)
{
   if( !read_with_record_size( filePtr,
                               this,
                               &visit_tornado<FileReaderVisitor>,
                               TORNADO_RECORD_SIZE ) )
      return 0;

   //------------ post-process the data read ----------//

   sprite_info = sprite_res[sprite_id];

	sprite_info->load_bitmap_res();

	return 1;
}
//----------- End of function Tornado::read_file ---------//


//*****//


//-------- Start of function RebelArray::write_file -------------//
//
int RebelArray::write_file(File* filePtr)
{
	return write_ptr_array(filePtr, sizeof(Rebel));
}
//--------- End of function RebelArray::write_file ---------------//


//-------- Start of function RebelArray::read_file -------------//
//
int RebelArray::read_file(File* filePtr)
{
	return read_ptr_array(filePtr, sizeof(Rebel), create_rebel_func);
}
//--------- End of function RebelArray::read_file ---------------//


//-------- Start of static function create_rebel_func ---------//
//
static char* create_rebel_func()
{
	Rebel *rebelPtr = new Rebel;

	rebel_array.linkin(&rebelPtr);

	return (char*) rebelPtr;
}
//--------- End of static function create_rebel_func ----------//


//*****//


//-------- Start of function SpyArray::write_file -------------//
//
int SpyArray::write_file(File* filePtr)
{
	return DynArrayB::write_file( filePtr );
}
//--------- End of function SpyArray::write_file ---------------//


//-------- Start of function SpyArray::read_file -------------//
//
int SpyArray::read_file(File* filePtr)
{
	return DynArrayB::read_file( filePtr );
}
//--------- End of function SpyArray::read_file ---------------//


//*****//


//-------- Start of function SnowGroundArray::write_file -------------//
//
int SnowGroundArray::write_file(File* filePtr)
{
   if( !filePtr->file_write( this, sizeof(SnowGroundArray)) )
      return 0;

   return 1;
}
//--------- End of function SnowGroundArray::write_file ---------------//


//-------- Start of function SnowGroundArray::read_file -------------//
//
int SnowGroundArray::read_file(File* filePtr)
{
   if( !filePtr->file_read( this, sizeof(SnowGroundArray)) )
      return 0;

   return 1;
}
//--------- End of function SnowGroundArray::read_file ---------------//

//*****//

template <typename Visitor>
static void visit_region_array(Visitor *v, RegionArray *ra)
{
	visit<int32_t>(v, &ra->init_flag);
	visit_pointer(v, &ra->region_info_array);
	visit<int32_t>(v, &ra->region_info_count);
	visit_pointer(v, &ra->region_stat_array);
	visit<int32_t>(v, &ra->region_stat_count);
	visit_pointer(v, &ra->connect_bits);
	visit_array<uint8_t>(v, ra->region_sorted_array, MAX_REGION);
}

enum { REGION_ARRAY_RECORD_SIZE = 279 };

//-------- Start of function RegionArray::write_file -------------//
//
int RegionArray::write_file(File* filePtr)
{
	if( !write_with_record_size( filePtr,
				     this,
				     &visit_region_array<FileWriterVisitor>,
				     REGION_ARRAY_RECORD_SIZE ) )
		return 0;

	if( !filePtr->file_write( region_info_array, sizeof(RegionInfo)*region_info_count ) )
		return 0;

	//-------- write RegionStat ----------//

	filePtr->file_put_short( region_stat_count );

	if( !filePtr->file_write( region_stat_array, sizeof(RegionStat)*region_stat_count ) )
		return 0;

	//--------- write connection bits ----------//

	int connectBit = (region_info_count -1) * (region_info_count) /2;
	int connectByte = (connectBit +7) /8;

	if( connectByte > 0)
	{
		if( !filePtr->file_write(connect_bits, connectByte) )
			return 0;
	}

	return 1;
}
//--------- End of function RegionArray::write_file ---------------//


//-------- Start of function RegionArray::read_file -------------//
//
int RegionArray::read_file(File* filePtr)
{
	deinit();		// to free memory of region_info_array, region_stat_count, and connect_bits
   
	if( !read_with_record_size( filePtr,
				    this,
				    &visit_region_array<FileReaderVisitor>,
				    REGION_ARRAY_RECORD_SIZE ) )
		return 0;

   if( region_info_count > 0 )
      region_info_array = (RegionInfo *) mem_add(sizeof(RegionInfo)*region_info_count);
   else
      region_info_array = NULL;

   if( !filePtr->file_read( region_info_array, sizeof(RegionInfo)*region_info_count))
      return 0;

	//-------- read RegionStat ----------//

	region_stat_count = filePtr->file_get_short();

	region_stat_array = (RegionStat*) mem_add( region_stat_count * sizeof(RegionStat) );

	if( !filePtr->file_read( region_stat_array, sizeof(RegionStat)*region_stat_count ) )
		return 0;

	//--------- read connection bits ----------//

	int connectBit = (region_info_count -1) * (region_info_count) /2;
	int connectByte = (connectBit +7) /8;

	if( connectByte > 0)
	{
		connect_bits = (unsigned char *)mem_add(connectByte);
		if( !filePtr->file_read(connect_bits, connectByte) )
			return 0;
	}
	else
	{
		connect_bits = NULL;
	}

	return 1;
}
//--------- End of function RegionArray::read_file ---------------//

//*****//

//-------- Start of function NewsArray::write_file -------------//
//
int NewsArray::write_file(File* filePtr)
{
	//----- save news_array parameters -----//

	filePtr->file_write( news_type_option, sizeof(news_type_option) );

   filePtr->file_put_short(news_who_option);
   filePtr->file_put_long (last_clear_recno);

   //---------- save news data -----------//

   return DynArray::write_file(filePtr);
}
//--------- End of function NewsArray::write_file ---------------//


//-------- Start of function NewsArray::read_file -------------//
//
int NewsArray::read_file(File* filePtr)
{
   //----- read news_array parameters -----//

   filePtr->file_read( news_type_option, sizeof(news_type_option) );

   news_who_option   = (char) filePtr->file_get_short();
   last_clear_recno  = filePtr->file_get_long();

   //---------- read news data -----------//

   return DynArray::read_file(filePtr);
}
//--------- End of function NewsArray::read_file ---------------//

//*****//

//-------- Start of function StateArray::write_file -------------//
//
int StateArray::write_file(File* filePtr)
{
	//------ write the matrix -----//

	filePtr->file_put_short(init_flag);

	if( init_flag )
	{
		filePtr->file_put_short(max_x_loc);
		filePtr->file_put_short(max_y_loc);

		if( !filePtr->file_write(loc_matrix, max_x_loc*max_y_loc) )
			return 0;

		//------- write the array -----//

		filePtr->file_put_short(state_count);

		if( !filePtr->file_write(state_info_array, sizeof(StateInfo)*state_count) )
			return 0;
	}

	return 1;
}
//--------- End of function StateArray::write_file ---------------//


//-------- Start of function StateArray::read_file -------------//
//
int StateArray::read_file(File* filePtr)
{
	//------ read the matrix -----//

	deinit();

	init_flag = filePtr->file_get_short();

	if( init_flag )
	{
		max_x_loc = filePtr->file_get_short();
		max_y_loc = filePtr->file_get_short();

		loc_matrix = (StateLocation*) mem_resize( loc_matrix, max_x_loc*max_y_loc );

		if( !filePtr->file_read(loc_matrix, max_x_loc*max_y_loc) )
			return 0;

		//------- read the array -----//

		state_count = filePtr->file_get_short();

		state_info_array = (StateInfo*) mem_resize(state_info_array, sizeof(StateInfo)*state_count);

		if( !filePtr->file_read(state_info_array, sizeof(StateInfo)*state_count) )
			return 0;
	}

	return 1;
}
//--------- End of function StateArray::read_file ---------------//

//****//

//--------- Begin of function WayPoint::write_file ----------//
//
int WayPoint::write_file(File *filePtr)
{
	if( !DynArray::write_file(filePtr) )
		return 0;

	if( !cur_unit_group.write_file(filePtr) )
		return 0;

	return 1;
}
//--------- End of function WayPoint::write_file ----------//

//--------- Begin of function WayPoint::read_file ----------//
//
int WayPoint::read_file(File *filePtr)
{
	if( !DynArray::read_file(filePtr) )
		return 0;

	if( !cur_unit_group.read_file(filePtr) )
		return 0;

	return 1;
}
//--------- End of function WayPoint::read_file ----------//

//--------- Begin of function WayPointArray::write_file ----------//
//
int WayPointArray::write_file(File *filePtr)
{
	// write number of element

	if( !filePtr->file_put_long(last_ele) )
		return 0;

	for( int i = 1; i <= size(); ++i )
	{
		if( !get(i)->write_file(filePtr))
			return 0;
	}

	return 1;
}
//--------- End of function WayPointArray::write_file ----------//

//--------- Begin of function WayPointArray::read_file ----------//
//
int WayPointArray::read_file(File *filePtr)
{
	zap();

	int newSize = filePtr->file_get_long();
	resize(newSize);

	for( int i = 1; i <= newSize; ++i )
	{
		if( i != add() )
			return 0;

		if( !get(i)->read_file(filePtr) )
			return 0;
	}

	return 1;
}
//--------- End of function WayPointArray::read_file ----------//

//****//

//-------- Begin of function BaseObjArray::write_file --------//
//
int BaseObjArray::write_file(File *filePtr)
{
	int   i;
	BaseObj* elePtr;

	filePtr->file_put_short( size() );

	for( i=1; i<=size() ; i++ )
	{
		elePtr = (BaseObj*) get_ptr(i);

		if( !elePtr )
		{
			//----- write 0 if the obj is deleted -----//

			filePtr->file_put_short(0);
		}
		else
		{
			//----- object not deleted -------//

			filePtr->file_put_short(elePtr->obj_type);
			filePtr->file_put_short(elePtr->obj_extern_recno);
		}
	}

	//------- write empty room array --------//

	write_empty_room(filePtr);

	return 1;
}
//-------- End of function BaseObjArray::write_file --------//


//-------- Begin of function BaseObjArray::read_file --------//
//
int BaseObjArray::read_file(File* filePtr)
{
	int   i;

	int eleCount = filePtr->file_get_short();  // get no. of object from file

	for( i=1 ; i<=eleCount ; i++ )
	{
		short objType = filePtr->file_get_short();
		short objExternRecno;

		if( !objType )			// the object has been deleted
		{
			add_blank(1);     // it's a DynArrayB function
		}
		else
		{
			objExternRecno = filePtr->file_get_short();

			switch( objType )
			{
			case OBJ_UNIT:
				if( unit_array.is_truly_deleted(objExternRecno) )
				{
					err_here();		// already deleted
					return 0;
				}
				if( add(unit_array[objExternRecno]) != i )
				{
					err_here();		// check create recno number is as same as that expected in the save game
					return 0;
				}
				break;

			case OBJ_FIRM:
				if( firm_array.is_deleted(objExternRecno) )
				{
					err_here();		// already deleted
					return 0;
				}
				if( add(firm_array[objExternRecno]) != i )
				{
					err_here();		// check create recno number is as same as that expected in the save game
					return 0;
				}
				break;

			case OBJ_TOWN:
				if( town_array.is_deleted(objExternRecno) )
				{
					err_here();		// already deleted
					return 0;
				}
				if( add(town_array[objExternRecno]) != i )
				{
					err_here();		// check create recno number is as same as that expected in the save game
					return 0;
				}
				break;

			default:
				err_here();
			}
		}
	}

	//-------- linkout() those record added by add_blank() ----------//
	//-- So they will be marked deleted in DynArrayB and can be -----//
	//-- undeleted and used when a new record is going to be added --//

	for( i=size() ; i>0 ; i-- )
	{
		DynArrayB::go(i);             // since DynArrayB has its own go() which will call GroupArray::go()

		if( get_ptr() == NULL )       // add_blank() record
			linkout();
	}

	//------- read empty room array --------//

	read_empty_room(filePtr);

	return 1;
}
//-------- End of function BaseObjArray::read_file --------//

// ####### end Gilbert 25/9 #########//
