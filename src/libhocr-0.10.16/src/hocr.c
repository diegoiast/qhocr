
/***************************************************************************
 *            hocr.c
 *
 *  Fri Aug 12 20:13:33 2005
 *  Copyright  2005-2008  Yaacov Zamir
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

#include <stdio.h>
#include <math.h>

#include "hocr.h"

#ifndef TRUE
#define TRUE -1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef NULL
#define NULL ((void*)0)
#endif

/**
 convert a gray pixbuf to bitmap 

 @param pix_in the input ho_pixbuf
 @param scale the scale to use
 @param no_auto_scale do not try to autoscale
 @param rotate the rotation angle to use
 @param no_auto_rotate do not try to autorotate
 @param adaptive what type of thresholding to use
 @param threshold the threshold to use 0..100
 @param a_threshold the threshold to use for adaptive thresholding 0..100
 @param progress a progress indicator 0..100
 @return newly allocated gray ho_bitmap
 */
ho_bitmap *
hocr_image_processing (const ho_pixbuf * pix_in,
  const unsigned char scale,
  const unsigned char no_auto_scale,
  double rotate,
  const unsigned char no_auto_rotate,
  const unsigned char adaptive,
  const unsigned char threshold, const unsigned char a_threshold, int *progress)
{

  ho_bitmap *bitmap_out = NULL;

  ho_bitmap *bitmap_temp = NULL;

  double angle = 0.0;

  int scale_by = 0;

  unsigned char size = 0;

  /* init progress */
  *progress = 0;

  /* get the raw b/w bitmap from the pixbuf */
  bitmap_temp = ho_pixbuf_to_bitmap_wrapper (pix_in,
    scale, adaptive, threshold, a_threshold, size);
  if (!bitmap_temp)
    return NULL;

  /* update progress */
  *progress = 25;

  /* do we want to auto scale ? */
  if (!scale && !no_auto_scale)
  {
    /* get fonts size for autoscale */
    if (ho_dimentions_font_width_height_nikud (bitmap_temp, 6, 200, 6, 200))
      return NULL;

    /* if fonts are too small, re-scale image */
    if (bitmap_temp->font_height < 15)
      scale_by = 3;
    else if (bitmap_temp->font_height < 30)
      scale_by = 2;
    else
      scale_by = 1;

    if (scale_by > 1)
    {
      /* re-create bitmap */
      ho_bitmap_free (bitmap_temp);
      bitmap_temp = ho_pixbuf_to_bitmap_wrapper (pix_in,
        scale_by, adaptive, threshold, a_threshold, size);
      if (!bitmap_temp)
        return NULL;
    }
  }

  /* update progress */
  *progress = 50;

  /* remove very small and very large things */
  bitmap_out =
    ho_bitmap_filter_by_size (bitmap_temp, 3, 3 * bitmap_temp->height / 4, 3,
    3 * bitmap_temp->width / 4);
  ho_bitmap_free (bitmap_temp);
  if (!bitmap_out)
    return NULL;

  /* update progress */
  *progress = 75;

  /* rotate image */
  if (rotate)
  {
    bitmap_temp = ho_bitmap_rotate (bitmap_out, rotate);
    ho_bitmap_free (bitmap_out);
    if (!bitmap_temp)
      return NULL;

    bitmap_out = bitmap_temp;
  }
  else if (!no_auto_rotate)
  {
    /* get fonts size for auto angle */
    if (ho_dimentions_font_width_height_nikud (bitmap_out, 6, 200, 6, 200))
      return NULL;

    angle = ho_dimentions_get_lines_angle (bitmap_out);
    if (angle)
    {
      bitmap_temp = ho_bitmap_rotate (bitmap_out, angle);
      ho_bitmap_free (bitmap_out);
      if (!bitmap_temp)
        return NULL;

      bitmap_out = bitmap_temp;
    }
  }

  return bitmap_out;
}

/**
 new ho_layout 

 @param m_in a pointer to a text bitmap
 @param font_spacing_code -3 tight .. 0 .. 3 spaced
 @param paragraph_setup free text blocks or boxed in columns
 @param slicing_threshold percent of line fill to cut fonts
 @param slicing_width what is a wide font
 @param dir true-ltr false-rtl
 @param progress a progress indicator 0..100
 @return a newly allocated and filled layout
 */
ho_layout *
hocr_layout_analysis (const ho_bitmap * m_in, const int font_spacing_code,
  const int paragraph_setup, const int slicing_threshold,
  const int slicing_width, const unsigned char dir_ltr, int *progress)
{
  int cols = paragraph_setup;

  int block_index;

  int line_index;

  int word_index;

  ho_layout *layout_out = NULL;

  /* init progress */
  *progress = 0;

  /* check paragraph_setup */
  if (!cols)
    cols = ho_dimentions_get_columns (m_in);

  /* create a new layout */
  layout_out = ho_layout_new (m_in, font_spacing_code, cols, dir_ltr);
  if (!layout_out)
    return NULL;

  *progress = 10;
  ho_layout_create_block_mask (layout_out);

  /* look for lines inside blocks */
  for (block_index = 0; block_index < layout_out->n_blocks; block_index++)
  {
    ho_layout_create_line_mask (layout_out, block_index);

    /* look for words inside line */
    for (line_index = 0; line_index < layout_out->n_lines[block_index];
      line_index++)
    {
      ho_layout_create_word_mask (layout_out, block_index, line_index);

      /* look for fonts inside word */
      for (word_index = 0;
        word_index < layout_out->n_words[block_index][line_index]; word_index++)
      {
        ho_layout_create_font_mask (layout_out, block_index, line_index,
          word_index, slicing_threshold, slicing_width);
      }

      /* update progress */
      *progress = 100 *
        ((double) block_index /
        (double) layout_out->n_blocks +
        (double) line_index /
        (double) (layout_out->n_lines[block_index] * layout_out->n_blocks));
    }
  }

  return layout_out;
}

/**
 fill a text buffer with fonts recognized from a page layout

 @param l_page the page layout to recognize
 @param s_text_out the text buffer to fill
 @param html output format is html
 @param font_code code for the font to use
 @param nikud recognize nikud
 @param progress a progress indicator 0..100
 @return FALSE
 */
int
hocr_font_recognition (const ho_layout * l_page, ho_string * s_text_out,
  const unsigned char html, int font_code, const unsigned char nikud,
  unsigned char do_linguistics, int *progress)
{
  int block_index;

  int line_index;

  int word_index;

  int font_index;

  int current_font_number = 0;

  int number_of_fonts = l_page->number_of_fonts;

  ho_bitmap *m_text = NULL;

  ho_bitmap *m_mask = NULL;

  ho_bitmap *m_font_main_sign = NULL;

  ho_bitmap *m_font_nikud = NULL;

  char text_out[200];

  const char *font;

  const char *font_nikud;

  const char *font_dagesh;

  const char *font_shin;

  /* init progress */
  *progress = 0;

  /* did we get a text buffer and a layout ? */
  if (!s_text_out || !l_page)
    return TRUE;

  /* loop over the layout */
  for (block_index = 0; block_index < l_page->n_blocks; block_index++)
  {
    /* start of paragraph */
    if (html)
    {
      /* FIXME: text overflow ?! */
      sprintf (text_out,
        "    <div class=\"ocr_par\" id=\"par_%d\" title=\"bbox %d %d %d %d\">\n",
        block_index + 1, l_page->m_blocks_text[block_index]->x,
        l_page->m_blocks_text[block_index]->y,
        l_page->m_blocks_text[block_index]->x +
        l_page->m_blocks_text[block_index]->width,
        l_page->m_blocks_text[block_index]->y +
        l_page->m_blocks_text[block_index]->height);
      ho_string_cat (s_text_out, text_out);
    }

    for (line_index = 0; line_index < l_page->n_lines[block_index];
      line_index++)
    {
      /* start of line */

      /* loop on all the words in this line */
      for (word_index = 0;
        word_index < l_page->n_words[block_index][line_index]; word_index++)
      {
        int word_length = l_page->n_fonts[block_index][line_index][word_index];

        unsigned char word_end = FALSE;

        unsigned char word_start = TRUE;

        int last_char_i = 0;

        int char_i = 0;

        /* start of word */
        for (font_index = 0; font_index < word_length; font_index++)
        {
          word_end = (font_index == (word_length - 1));
          word_start = (font_index == 0);

          /* get font images */

          /* get the font */
          m_text =
            ho_layout_get_font_text (l_page, block_index,
            line_index, word_index, font_index);
          if (!m_text)
            return TRUE;

          /* get font line mask */
          m_mask =
            ho_layout_get_font_line_mask (l_page, block_index,
            line_index, word_index, font_index);
          if (!m_mask)
            return TRUE;

          /* get font main sign */
          m_font_main_sign = ho_font_main_sign (m_text, m_mask);
          if (!m_font_main_sign)
            return TRUE;

          /* recognize font from images */
          last_char_i = char_i;
          font =
            ho_recognize_font (m_font_main_sign, m_mask,
            font_code, do_linguistics, word_end, word_start, &char_i,
            last_char_i);

          /* insert font to text out */
          ho_string_cat (s_text_out, font);

          /* get font nikud */
          if (nikud)
          {
            m_font_nikud = ho_bitmap_clone (m_text);
            if (!m_font_nikud)
              return TRUE;
            ho_bitmap_andnot (m_font_nikud, m_font_main_sign);

            /* recognize font from images */
            font_nikud = ho_recognize_nikud (m_font_nikud, m_mask,
              font_code, &font_dagesh, &font_shin);

            /* free bitmaps */
            ho_bitmap_free (m_font_nikud);
            m_font_nikud = NULL;

            /* insert font nikud to text out */
            ho_string_cat (s_text_out, font_shin);
            ho_string_cat (s_text_out, font_dagesh);
            ho_string_cat (s_text_out, font_nikud);
          }

          /* free bitmaps */
          ho_bitmap_free (m_font_main_sign);
          ho_bitmap_free (m_text);
          ho_bitmap_free (m_mask);

          /* this are empty pointers */
          m_text = m_mask = m_font_main_sign = NULL;

          /* update progress */
          current_font_number++;
          *progress = 100 * current_font_number / number_of_fonts;
        }

        /* end of word */
        ho_string_cat (s_text_out, " ");
      }

      /* end of line */
      if (html)
        ho_string_cat (s_text_out, "<br/>\n");
      else
        ho_string_cat (s_text_out, "\n");
    }

    /* end of block */
    if (html)
      ho_string_cat (s_text_out, "<br/>\n    </div>\n");
    else
      ho_string_cat (s_text_out, "\n");
  }

  return FALSE;
}

int
hocr_do_ocr_fine (const ho_pixbuf * pix_in,
  ho_string * s_text_out,
  const unsigned char scale,
  const unsigned char no_auto_scale,
  double rotate,
  const unsigned char no_auto_rotate,
  const unsigned char adaptive,
  const unsigned char threshold, const unsigned char a_threshold,
  const int font_spacing_code, const int paragraph_setup,
  const int slicing_threshold, const int slicing_width,
  const unsigned char dir_ltr,
  const unsigned char html, int font_code, const unsigned char nikud,
  const unsigned char do_linguistics, int *progress)
{
  ho_bitmap *m_in = NULL;

  ho_layout *l_page = NULL;

  int return_val;

  if (!pix_in)
    return TRUE;

  m_in = hocr_image_processing (pix_in,
    scale,
    no_auto_scale,
    rotate, no_auto_rotate, adaptive, threshold, a_threshold, progress);

  if (!m_in)
    return TRUE;

  l_page = hocr_layout_analysis (m_in,
    font_spacing_code, paragraph_setup,
    slicing_threshold, slicing_width, dir_ltr, progress);

  if (!l_page)
  {
    ho_bitmap_free (m_in);
    return TRUE;
  }

  return_val = hocr_font_recognition (l_page, s_text_out,
    html, font_code, nikud, do_linguistics, progress);

  return return_val;
}

int
hocr_do_ocr (const ho_pixbuf * pix_in,
  ho_string * s_text_out,
  const unsigned char html, int font_code, const unsigned char do_linguistics,
  int *progress)
{
  return hocr_do_ocr_fine (pix_in,
    s_text_out,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, html, font_code, 1, do_linguistics,
    progress);
}

const char *
hocr_get_build_string ()
{
  return BUILD;
}

const char *
hocr_get_version_string ()
{
  return VERSION;
}
