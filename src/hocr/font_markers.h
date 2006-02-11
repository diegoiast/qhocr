/***************************************************************************
 *            font_markers.h
 *
 *  Fri Aug 12 20:33:02 2005
 *  Copyright  2005  Yaacov Zamir
 *  <kzamir@walla.co.il>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* 
 font markers
 */

#include "hocr.h"

#ifndef __FONT_MARKERS_H__
#define __FONT_MARKERS_H__


/**
 */

/* font shape markers array*/

typedef int (*has_font_mark_function) (hocr_pixbuf * pix, hocr_box font);

int
init_has_font_mark_functions (has_font_mark_function * has_font_mark);

/* the real functions */

int has_alef_mark (hocr_pixbuf * pix, hocr_box font);
int has_bet_mark (hocr_pixbuf * pix, hocr_box font);
int has_gimel_mark (hocr_pixbuf * pix, hocr_box font);
int has_dalet_mark (hocr_pixbuf * pix, hocr_box font);
int has_he_mark (hocr_pixbuf * pix, hocr_box font);
int has_vav_mark (hocr_pixbuf * pix, hocr_box font);
int has_zain_mark (hocr_pixbuf * pix, hocr_box font);
int has_het_mark (hocr_pixbuf * pix, hocr_box font);
int has_tet_mark (hocr_pixbuf * pix, hocr_box font);
int has_yud_mark (hocr_pixbuf * pix, hocr_box font);
int has_kaf_mark (hocr_pixbuf * pix, hocr_box font);
int has_kaf_sofit_mark (hocr_pixbuf * pix, hocr_box font);
int has_lamed_mark (hocr_pixbuf * pix, hocr_box font);
int has_mem_mark (hocr_pixbuf * pix, hocr_box font);
int has_mem_sofit_mark (hocr_pixbuf * pix, hocr_box font);
int has_nun_mark (hocr_pixbuf * pix, hocr_box font);
int has_nun_sofit_mark (hocr_pixbuf * pix, hocr_box font);
int has_sameh_mark (hocr_pixbuf * pix, hocr_box font);
int has_ayin_mark (hocr_pixbuf * pix, hocr_box font);
int has_pe_mark (hocr_pixbuf * pix, hocr_box font);
int has_pe_sofit_mark (hocr_pixbuf * pix, hocr_box font);
int has_tzadi_mark (hocr_pixbuf * pix, hocr_box font);
int has_tzadi_sofit_mark (hocr_pixbuf * pix, hocr_box font);
int has_kof_mark (hocr_pixbuf * pix, hocr_box font);
int has_resh_mark (hocr_pixbuf * pix, hocr_box font);
int has_shin_mark (hocr_pixbuf * pix, hocr_box font);
int has_tav_mark (hocr_pixbuf * pix, hocr_box font);

int has_quat_mark (hocr_pixbuf * pix, hocr_box font);
int has_double_quat_mark (hocr_pixbuf * pix, hocr_box font);
int has_exlem_mark (hocr_pixbuf * pix, hocr_box font);
int has_question_mark (hocr_pixbuf * pix, hocr_box font);
int has_makaf_mark (hocr_pixbuf * pix, hocr_box font);

int
has_black_right_bottom_mark (hocr_pixbuf * pix, hocr_box font);
/**
 */

#endif
