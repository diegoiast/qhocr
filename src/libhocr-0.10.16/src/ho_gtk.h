
/***************************************************************************
 *            ho_gtk.h
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

/** @file ho_gtk.h
    @brief libhocr C language header.
    
    libhocr - LIBrary for Hebrew Optical Character Recognition 
*/

#ifndef HO_GTK_H
#define HO_GTK_H 1

#include <gdk-pixbuf/gdk-pixbuf.h>

#include <hocr.h>

#ifdef __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

/**
 new ho_pixbuf from a gdk_pixbuf
 
 @param gdk_pix the pixbuf to copy
 @return newly allocated ho_pixbuf
 */
  ho_pixbuf *ho_gtk_pixbuf_from_gdk (const GdkPixbuf * gdk_pix);

/**
 new ho_pixbuf from a gdk_pixbuf
 
 @param gdk_pix the pixbuf to share data with
 @return newly allocated ho_pixbuf, share data with gtk pixbuf
 */
  ho_pixbuf *ho_gtk_pixbuf_from_gdk_share_data (const GdkPixbuf * gdk_pix);

/**
 new gdk_pixbuf from a ho_pixbuf
 
 @param hocr_pix the pixbuf to copy
 @return newly allocated gdk_pixbuf
 */
  GdkPixbuf *ho_gtk_pixbuf_to_gdk (const ho_pixbuf * hocr_pix);

/**
 read ho_pixbuf from file
 
 @param filename file name
 @return newly allocated ho_pixbuf
 */
  ho_pixbuf *ho_gtk_pixbuf_load (const char *filename);

/**
 writes ho_pixbuf to file
 
 @param pix ho_pixbuf 8 or 24 bpp
 @param filename save as file name
 @param ext file type extention, e.g jpeg, png ...
 @return FALSE
 */
  int ho_gtk_pixbuf_save (const ho_pixbuf * pix, const char *filename, 
                          const char *ext);

/**
 writes font to file
 
 @param m_text ho_bitmap of main font
 @param m_nikud ho_bitmap of nikud
 @param m_mask ho_bitmap of line
 @param filename save as file name 
 @param ext file type extention, e.g jpeg, png ...
 @return FALSE
 */
  int
    ho_gtk_font_save (const ho_bitmap * m_text, const ho_bitmap * m_nikud,
    const ho_bitmap * m_mask, const char *filename, const char *ext);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* HO_GTK_H */
