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

//Filename    : OMATRIX.H
//Description : Object road direction turn

#ifndef __OMATRIX_H
#define __OMATRIX_H

#include <gamedef.h>
#include <oregion.h>
#include <olocate.h>

//------------ Define class Matrix -----------//

class World;

class Matrix
{
friend class World;

public:
   int       max_x_loc, max_y_loc;      // read from map file
   Location  *loc_matrix;
	Corner    *corner_matrix;            // no. of corners = (max_x_loc+1)*(max_y_loc+1)

	int       top_x_loc, top_y_loc;              // the top left location of current zoom window
	int       cur_x_loc, cur_y_loc;
	int       cur_cargo_width, cur_cargo_height; // cur_x2_loc = cur_x_loc + cur_cargo_width

	int       disp_x_loc, disp_y_loc;    // calculated in Matrix()
	// int       loc_width, loc_height;     // passed as para in Matrix()
   int       loc_x_width, loc_x_height;		// multiplied by 2^LOC_XY_MUL_SHIFT
   int       loc_y_width, loc_y_height;
	int       determinant_shift;		// log2(loc_x_width * loc_y_height - loc_x_height * loc_y_width);
	int       center_x, center_y;			 // location different vector from center of window to top left of window

   int       win_x1, win_y1, win_x2, win_y2;
	int       image_x1, image_y1, image_x2, image_y2;
	int		 image_width, image_height;

   char      own_matrix;
	char		 cur_cargo_rect_type;			// cur_cargo_width/height in 0 = rectangle of x/y loc or 1 = rectangle of column/row
	char		 disp_rect_type;				// disp_x/y_loc in 0 = rectangle of x/y loc or 1 = rectangle of column/row

	char      just_drawn_flag;    // whether the matrix has just been drawn by draw()
   short		*save_image_buf;

public:
   virtual ~Matrix();

   void init(int,int,int,int,int,int,float,float,float,float,char,int);
   void assign_map(Matrix*);
   void assign_map(Location*,int,int, Corner*);

   virtual void paint();
   virtual void draw();
   virtual void disp();
   virtual int  detect()      {return 0;}
	virtual void refresh();
   virtual void scroll(int,int);

   Location* get_loc(int xLoc,int yLoc)
			{ return loc_matrix+yLoc * max_x_loc + xLoc; }
	LocationCorners *get_loc_corner(int xLoc, int yLoc, LocationCorners *lc)
	{ 
		int i = yLoc * max_x_loc + xLoc;
		lc->loc_ptr = loc_matrix+ i;
		i += yLoc;	// i = yLoc * (max_x_loc+1) + xLoc;
		lc->top_right = (lc->top_left = corner_matrix + i) + 1;
		i += max_x_loc+1;		// next row
		lc->bottom_right = (lc->bottom_left = corner_matrix + i) + 1;

		return lc;
	}

	Corner *get_corner(int xLoc, int yLoc)
	{ return corner_matrix + yLoc *(max_x_loc+1) + xLoc; }
	
	virtual int get_detect_location(int scrnX, int scrnY, int *locX, int *locY, char *mobileType);
	// if outside the screen, return 0

	virtual int get_close_location(int scrnX, int scrnY, int *locX, int *locY, char *mobileType);

	// only for isometric view
	int	get_row(int xLoc, int yLoc);
	int	get_column(int xLoc, int yLoc);
	int	get_x_loc(int column, int row);
	int	get_y_loc(int row, int column);

	int	calc_loc_pos_x(int xLoc, int yLoc);	// loc x/y to window position (relative to image_x1, image_y1)
	int	calc_loc_pos_y(int xLoc, int yLoc);

protected:
   virtual void post_draw()      {;}
   virtual void draw_loc(int x,int y,int xLoc,int yLoc,Location* locPtr) {;}

   virtual void init_var();
	int  valid_cur_box(int=1);
   void valid_disp_area(int=0);
};

#endif
