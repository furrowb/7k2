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

//Filename    : OU_CARA.H
//Description : Header file of Unit Caravan

#ifndef __OU_CARA_H
#define __OU_CARA_H

#ifndef __ODYNARR_H
#include <odynarr.h>
#endif

#ifndef __OUNIT_H
#include <ounit.h>
#endif

#ifndef __ORAWRES_H
#include <orawres.h>
#endif

#ifndef __F_MARK_H
#include <of_mark.h>
#endif

//------------- Define constant -----------//

#define MAX_STOP_FOR_CARAVAN		2		// Maximum no. of destination stations per train
#define MAX_CARAVAN_CARRY_QTY 	200	// Maximum qty of goods a caravan can carry
#define MAX_CARAVAN_WAIT_TERM		8		// no. of term the caravan is in the market to upload/download cargo
#define POPULATION_PER_CARAVAN   10		// the population size for supporting one caravan

//-------- define pick up type --------//

enum	{	AUTO_PICK_UP = 0,
			FIRST_GOODS_SELECT_BUTTON,
			PICK_UP_PRODUCT_FIRST = FIRST_GOODS_SELECT_BUTTON,
			PICK_UP_PRODUCT_LAST=PICK_UP_PRODUCT_FIRST+MAX_PRODUCT-1,
			PICK_UP_RAW_FIRST,
			PICK_UP_RAW_LAST=PICK_UP_RAW_FIRST+MAX_RAW-1,
			NO_PICK_UP,						// for compatibility
			MANUAL_PICK_UP,						// for compatibility
			MAX_PICK_UP_GOODS = MAX_PRODUCT + MAX_RAW,
			MAX_GOODS_SELECT_BUTTON = MAX_PICK_UP_GOODS,		// no include NO_PICK_UP
		};

enum { NO_STOP_DEFINED = 0,	// used for journey_status
		 ON_WAY_TO_FIRM = 1,
		 SURROUND_FIRM,
		 INSIDE_FIRM,
	  };

#define CARAVAN_UNLOAD_TO_MARKET_QTY	100
#define MIN_FIRM_STOCK_QTY					100

//-------- Define struct TradeStop ----------//

#pragma pack(1)
struct TradeStop
{
public:
	short 		firm_recno;							// firm recno of the station
	short			firm_loc_x1;						//-******* used temporarily
	short 		firm_loc_y1;
	char  		pick_up_type;			// auto, selective, none
	char			pick_up_array[MAX_PICK_UP_GOODS]; // useful for selective mode

public:
	int			num_of_pick_up_goods(char *enableTable);

	//----------- single player version --------------//
	void			pick_up_set_auto();
	void			pick_up_set_manual();
	void			pick_up_set_none();
	void			pick_up_toggle(int pos);

	//----------- multiplayer version --------------//
	void			mp_pick_up_set_auto(char *enableTable);
	void			mp_pick_up_set_none(char *enableTable);
};
#pragma pack()

//-------- Define struct CaravanStop ----------//
#pragma pack(1)
struct CaravanStop : public TradeStop
{
public:
	char	firm_id;

public:
	int	update_pick_up(char *enableFlag=NULL);

	//----------- multiplayer version --------------//
	void			mp_pick_up_toggle(int pos);
};
#pragma pack()

//----------- Define class Caravan -----------//

#pragma pack(1)
class UnitCaravan : public Unit
{
public:
	short			caravan_id;						// id. of the caravan, for name display

	char			journey_status;				// 1 for not unload but can up load, 2 for unload but not up load
	char		 	dest_stop_id;					// destination stop id. the stop which the train currently is moving towards
	char			stop_defined_num;				// num of stop defined
	char			wait_count;						// set to -1 to indicate only one stop is specified
	short			entered_firm_recno;			// the recno of the firm which the unit has entered

	char			default_market_trade_mode;

	short			stop_x_loc;						// the x location the unit entering the stop
	short			stop_y_loc;						// the y location the unit entering the stop

	CaravanStop stop_array[MAX_STOP_FOR_CARAVAN];	// an array of firm_recno telling train stop stations

	int			last_set_stop_date;			// the date when stops were last set.
   int			last_load_goods_date;		// the last date when the caravan load goods from a firm  

	//------ goods that the caravan carries -------//

	short			raw_qty_array[MAX_RAW];
	short			product_raw_qty_array[MAX_PRODUCT];

   int			carrying_qty(int pickupType);

public:
	UnitCaravan();

	void	init_derived();

	void 	disp_info(int refreshFlag);
	void  detect_info();

	void 	set_stop(int stopId, int stopXLoc, int stopYLoc, char remoteAction);
	void	del_stop(int stopId, char remoteAction);
	void	update_stop_list();
	void	update_stop_and_goods_info();
	void 	set_stop_pick_up(int stopId, int newPIckUpType, int remoteAction);
	int 	can_set_stop(int firmRecno);
	int	has_pick_up_type(int stopId, int pickUpType);
	char	can_duplicate_caravan();
	short duplicate_caravan(char remoteAction);

	void	caravan_in_firm();
	void	caravan_on_way();
	void  pre_process();

	//------- ai functions --------//

	void 	process_ai();
	int	think_resign();
	int  	think_del_stop();
	void	think_set_pick_up_type();
	void	think_set_pick_up_type2(int fromStopId, int toStopId);

	//-------------- multiplayer checking codes ---------------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

	//--------- casting function ----------//

	UnitCaravan*	cast_to_UnitCaravan()	{ return this; }

private:
	void 	disp_stop(int dispY1, int refreshFlag);
	void	detect_stop();
	void	disp_goods(int dispY1, int refreshFlag);
		
	int 	get_next_stop_id(int curStopId=MAX_STOP_FOR_CARAVAN);

	//-------- for mine ----------//
	void	mine_load_goods(char pickUpType);

	//-------- for factory ---------//
	void	factory_unload_goods();
	void	factory_load_goods(char pickupType);

	//-------- for market ---------//
	void 	market_unload_goods();
	int	market_unload_goods_in_empty_slot(FirmMarket *curMarket, int position);
	void 	market_load_goods();
	void 	market_auto_load_goods();
	void 	market_load_goods_now(MarketGoods* marketGoods, float loadQty);

	int	appear_in_firm_surround(int& xLoc, int& yLoc, Firm* firmPtr);	// select a suitable location to leave the stop
};
#pragma pack()

//------------------------------------------//

#endif
