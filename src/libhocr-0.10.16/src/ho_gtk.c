
/***************************************************************************
 *            ho_gtk.c
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

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "ho_gtk.h"

ho_pixbuf *
ho_gtk_pixbuf_from_gdk (const GdkPixbuf * gdk_pix)
{
  ho_pixbuf *pix = NULL;

  if (!gdk_pix)
  {
    return NULL;
  }

  /* create hocr pixbuf from gkd pixbuf */
  pix = ho_pixbuf_new (gdk_pixbuf_get_n_channels
    (gdk_pix), gdk_pixbuf_get_width
    (gdk_pix), gdk_pixbuf_get_height (gdk_pix),
    gdk_pixbuf_get_rowstride (gdk_pix));

  if (!pix)
    return NULL;

  /* copy the pixels */
  memcpy (pix->data, gdk_pixbuf_get_pixels (gdk_pix),
    (pix->height * pix->rowstride));

  return pix;
}

ho_pixbuf *
ho_gtk_pixbuf_from_gdk_share_data (const GdkPixbuf * gdk_pix)
{
  ho_pixbuf *pix = NULL;

  if (!gdk_pix)
  {
    return NULL;
  }

  /* create hocr pixbuf from gkd pixbuf */
  pix = ho_pixbuf_new (gdk_pixbuf_get_n_channels
    (gdk_pix), gdk_pixbuf_get_width
    (gdk_pix), gdk_pixbuf_get_height (gdk_pix),
    gdk_pixbuf_get_rowstride (gdk_pix));

  if (!pix)
    return NULL;

  /* free ho_pixbuf data */
  if (pix->data)
    free (pix->data);

  /* share the pixels */
  pix->data = gdk_pixbuf_get_pixels (gdk_pix);

  return pix;
}

void
ho_gdk_free_pixels (guchar * pixels, gpointer data)
{
  free (pixels);
}

GdkPixbuf *
ho_gtk_pixbuf_to_gdk (const ho_pixbuf * hocr_pix)
{
  GdkPixbuf *pix = NULL;
  ho_pixbuf *pix_color = NULL;
  unsigned char *data;

  /* gdk has only RGB pixbufs */
  pix_color = ho_pixbuf_to_rgb (hocr_pix);
  if (!pix_color)
    return NULL;

  /* create data */
  data = malloc (pix_color->height * pix_color->rowstride);
  if (!data)
  {
    ho_pixbuf_free (pix_color);
    return NULL;
  }
  memcpy (data, pix_color->data, pix_color->height * pix_color->rowstride);

  /* load gdk pixbuf from ho_pixbuf */
  pix = gdk_pixbuf_new_from_data (data,
    GDK_COLORSPACE_RGB,
    FALSE,
    8,
    ho_pixbuf_get_width (pix_color),
    ho_pixbuf_get_height (pix_color),
    ho_pixbuf_get_rowstride (pix_color), ho_gdk_free_pixels, NULL);

  ho_pixbuf_free (pix_color);

  return pix;
}

ho_pixbuf *
ho_gtk_pixbuf_load (const char *filename)
{
  ho_pixbuf *pix = NULL;
  GdkPixbuf *gdk_pix = NULL;

  /* load gdk pixbuf from file */
  gdk_pix = gdk_pixbuf_new_from_file (filename, NULL);

  if (!gdk_pix)
  {
    return NULL;
  }

  /* create hocr pixbuf from gkd pixbuf */
  pix = ho_gtk_pixbuf_from_gdk (gdk_pix);

  /* free gdk pixbuf */
  g_object_unref (gdk_pix);

  return pix;
}

int
ho_gtk_pixbuf_save (const ho_pixbuf * pix, const char *filename, const char *ext)
{
  GError *error = NULL;
  GdkPixbuf *gdk_pix = NULL;

  /* load gdk pixbuf from ho_pixbuf */
  gdk_pix = ho_gtk_pixbuf_to_gdk (pix);
  if (!gdk_pix)
    return TRUE;

  /* save gdk pixbuf */
  gdk_pixbuf_save (gdk_pix, filename, ext, &error, NULL);

  /* free memory */
  g_object_unref (gdk_pix);

  if (error)
  {
    g_error_free (error);
    return TRUE;
  }

  return FALSE;
}

int
ho_gtk_font_save (const ho_bitmap * m_text, const ho_bitmap * m_nikud,
  const ho_bitmap * m_mask, const char *filename, const char *ext)
{
  int return_value;
  ho_pixbuf *p_out;

  p_out = ho_pixbuf_new (3, m_text->width, m_text->height, 0);
  ho_pixbuf_draw_rgb_bitmap (p_out, m_text, m_nikud, m_mask);
  
  return_value = ho_gtk_pixbuf_save (p_out, filename, ext);

  ho_pixbuf_free (p_out);

  return return_value;
}
