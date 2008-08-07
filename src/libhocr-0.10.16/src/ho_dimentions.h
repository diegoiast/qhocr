
/***************************************************************************
 *            ho_dimentions.h
 *
 *  Fri Aug 12 20:13:33 2005
 *  Copyright  2005-2007  Yaacov Zamir
 *  <kzamir@walla.co.il>
 ****************************************************************************/

/*  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @file ho_dimentions.h
    @brief libhocr C language header.
    
    libhocr - LIBrary for Hebrew Optical Character Recognition 
*/

#ifndef HO_DIMENTIONS_H
#define HO_DIMENTIONS_H 1

#include <ho_bitmap.h>
#include <ho_objmap.h>

/**
 guess font dimentions, sets: font_height, font_width and nikud  [for a page or block]
 @param m pointer to an ho_bitmap
 @param min_height only objects with this minimal height are used
 @param max_height only objects with this maximal height are used
 @param min_width only objects with this minimal width are used
 @param max_width only objects with this maximal width are used
 @return FALSE
 */
int
ho_dimentions_font_width_height_nikud (ho_bitmap * m, const int min_height,
  const int max_height, const int min_width, const int max_width);

/**
 guess line dimentions, sets line_spacing [for a block]
 @param m pointer to an ho_bitmap
 @return FALSE
 */
int ho_dimentions_line_spacing (ho_bitmap * m);

/**
 guess line dimentions, sets font_spacing [for a line]
 @param m pointer to an ho_bitmap of text
 @param m_line_map pointer to an ho_bitmap of ine
 @return FALSE
 */
int ho_dimentions_font_spacing (ho_bitmap * m, const ho_bitmap * m_line_map);

/**
 get line fill parameters [for a line]
 @param m pointer to an ho_bitmap of text
 @param m_line_map pointer to an ho_bitmap of ine
 @return FALSE
 */
int ho_dimentions_line_fill (ho_bitmap * m, const ho_bitmap * m_line_map);

/**
 guess number of columns in bitmap
 @param m pointer to an ho_bitmap
 @return guessed number of columns
 */
int ho_dimentions_get_columns (const ho_bitmap * m);

/**
 guess number of columns in bitmap
 @param m pointer to an ho_bitmap
 @param column_start_list a newly allocated int list of start of columns x value
 @return guessed number of columns
 */
int
ho_dimentions_get_columns_with_x_start (const ho_bitmap * m,
  int **column_start_list);

/**
 guess angle of page lines
 @param m pointer to an ho_bitmap
 @return guessed angle in degrees
 */
double
ho_dimentions_get_lines_angle (const ho_bitmap * m);

#endif /* HO_DIMENTIONS_H */
