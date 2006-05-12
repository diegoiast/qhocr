
/***************************************************************************
 *            hocr.c
 *
 *  Fri Aug 12 20:18:56 2005
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "consts.h"
#include "page_layout.h"
#include "font_layout.h"
#include "font_recognition.h"
#include "hocr.h"

/* 
 visualization helper finction
 */

int
print_font (hocr_pixbuf * pix, hocr_box font)
{
	int x, y;
	int new_color;

	for (x = font.x1; x <= font.x2; x++)
	{
		printf ("---");
	}
	printf ("\n");

	for (y = font.y1; y <= font.y2; y++)
	{
		for (x = font.x1; x <= font.x2; x++)
		{
			if ((new_color = hocr_pixbuf_get_object (pix, x, y)))
				printf ("%3d", new_color);
			else
				printf ("   ");
		}
		printf ("\n");
	}

	for (x = font.x1; x <= font.x2; x++)
	{
		printf ("---");
	}
	printf ("\n");

	printf ("\n");

	return 0;
}

int
color_hocr_box_full (hocr_pixbuf * pix, hocr_box rect, int chanell, int value,
		     int only_main_object)
{
	int x, y;
	unsigned int obj = 0;
	unsigned int object_array[MAX_OBJECTS_IN_FONT];

	if (only_main_object)
	{
		obj = hocr_pixbuf_get_objects_in_box (pix, rect, object_array,
						      MAX_OBJECTS_IN_FONT);
	}

	for (y = rect.y1 + 1; y <= (rect.y2 - 1); y++)
		for (x = rect.x1 + 1; x <= (rect.x2 - 1); x++)
		{
			if (!obj || obj == hocr_pixbuf_get_object (pix, x, y))
				hocr_pixbuf_set_pixel (pix, x, y, chanell,
						       value);
		}

	return 0;
}

int
color_hocr_box (hocr_pixbuf * pix, hocr_box rect, int chanell, int value)
{
	int x, y;

	for (y = rect.y1; y <= rect.y2; y++)
	{
		hocr_pixbuf_set_pixel (pix, rect.x1, y, chanell, value);
		hocr_pixbuf_set_pixel (pix, rect.x2, y, chanell, value);
	}

	for (x = rect.x1; x <= rect.x2; x++)
	{
		hocr_pixbuf_set_pixel (pix, x, rect.y1, chanell, value);
		hocr_pixbuf_set_pixel (pix, x, rect.y2, chanell, value);
	}

	return 0;
}

int
color_hocr_line_eq (hocr_pixbuf * pix, hocr_line_eq * line, int x1, int x2,
		    int chanell, int value)
{
	int x, y;

	for (x = x1; x < x2; x++)
	{
		y = hocr_line_eq_get_y (*line, x);
		hocr_pixbuf_set_pixel (pix, x, y, chanell, value);
	}

	return 0;
}

/*
 */

int
hocr_do_ocr (hocr_pixbuf * pix, hocr_text_buffer * text_buffer)
{
	hocr_box *columns;	/* [MAX_COLUMNS] */
	hocr_box *lines;	/* [MAX_COLUMNS][MAX_LINES] */
	hocr_box *fonts;	/* [MAX_COLUMNS][MAX_LINES][MAX_FONTS_IN_LINE] */

	hocr_line_eq line_eqs[MAX_COLUMNS][MAX_LINES][2];

	int *object_width_counters[MAX_FONT_WIDTH];	/* need to be zero at
							 * start */
	int *object_hight_counters[MAX_FONT_HIGHT];	/* need to be zero at
							 * start */

	int num_of_fonts[MAX_COLUMNS][MAX_LINES];
	int num_of_lines[MAX_COLUMNS];

	int num_of_columns_in_page = 0;
	int num_of_lines_in_page = 0;
	int num_of_fonts_in_page = 0;

	int avg_line_hight_in_page = 0;
	int avg_line_x_start_in_column[MAX_COLUMNS];
	int avg_diff_between_fonts_in_page = 0;
	int avg_font_width_in_page = 0;
	int avg_font_hight_in_page = 0;

	int c;
	int i, j, k;

	/* need this to put in the text_buffer */
	char chars[10];
	int symbols;

	/* format args */
	int end_of_word = 0;
	int tabs = 0;
	int indent = 0;
	int font_number = 0;

	/* init the progress indicators */
	pix->progress = 0;
	pix->progress_phase = 0;

	/* memory allocation */
	columns = (hocr_box *) malloc (sizeof (hocr_box) * MAX_COLUMNS);
	if (!columns)
		return -1;

	lines = (hocr_box *) malloc (sizeof (hocr_box) * MAX_COLUMNS *
				     MAX_LINES);
	if (!lines)
	{
		free (columns);
		return -1;
	}

	fonts = (hocr_box *) malloc (sizeof (hocr_box) * MAX_COLUMNS *
				     MAX_LINES * MAX_FONTS_IN_LINE);
	if (!fonts)
	{
		free (lines);
		free (columns);
		return -1;
	}

	/* create and fill the object map */
	pix->progress_phase = 1;
	hocr_pixbuf_create_object_map (pix);

	/* clean picture from small object arteffacts */
	hocr_pixbuf_clean (pix);

	/* get columns for this page */
	pix->progress_phase = 2;
	fill_columns_array (pix, columns, &num_of_columns_in_page, MAX_COLUMNS);

	/* get lines in this column */
	num_of_lines_in_page = 0;

	for (c = 0; c < num_of_columns_in_page; c++)
	{
		num_of_lines[c] = 0;

		fill_lines_array (pix, columns[c], &(lines[c * MAX_LINES]),
				  &(num_of_lines[c]), MAX_LINES);
		num_of_lines_in_page += num_of_lines[c];
	}

	/* get avg_line_hight_in_page */
	avg_line_hight_in_page = 0;

	for (c = 0; c < num_of_columns_in_page; c++)
	{
		avg_line_x_start_in_column[c] = 0;

		for (i = 0; i < num_of_lines[c]; i++)
		{
			avg_line_x_start_in_column[c] +=
				lines[c * MAX_LINES + i].x2;
			avg_line_hight_in_page +=
				lines[c * MAX_LINES + i].hight;
		}

		if (num_of_lines[c] != 0)
		{
			avg_line_x_start_in_column[c] =
				avg_line_x_start_in_column[c] / num_of_lines[c];
		}
	}

	if (num_of_lines_in_page != 0)
		avg_line_hight_in_page =
			avg_line_hight_in_page / num_of_lines_in_page;

	/* get all fonts for all the lines */
	for (c = 0; c < num_of_columns_in_page; c++)
	{
		for (i = 0; i < num_of_lines[c]; i++)
		{
			fill_fonts_array (pix, lines[c * MAX_LINES + i],
					  &(fonts
					    [c * MAX_LINES * MAX_FONTS_IN_LINE +
					     i * MAX_FONTS_IN_LINE]),
					  &(num_of_fonts[c][i]),
					  MAX_FONTS_IN_LINE);
		}
	}

	/* get size statistics for all fonts for all the lines */
	num_of_fonts_in_page = 0;
	avg_font_hight_in_page = 0;
	avg_font_width_in_page = 0;
	avg_diff_between_fonts_in_page = 0;

	/* set hight and width counters to zero */
	for (i = 0; i < MAX_FONT_HIGHT; i++)
	{
		object_hight_counters[i] = 0;
	}
	for (i = 0; i < MAX_FONT_WIDTH; i++)
	{
		object_width_counters[i] = 0;
	}

	for (c = 0; c < num_of_columns_in_page; c++)
	{
		for (i = 0; i < num_of_lines[c]; i++)
		{
			if (lines[c * MAX_LINES + i].hight >
			    (pix->avg_hight_of_objects))
			{
				for (j = 0; j < num_of_fonts[c][i]; j++)
				{

					num_of_fonts_in_page++;
					avg_font_width_in_page +=
						fonts[c * MAX_LINES *
						      MAX_FONTS_IN_LINE +
						      i * MAX_FONTS_IN_LINE +
						      j].width;
					avg_font_hight_in_page +=
						fonts[c * MAX_LINES *
						      MAX_FONTS_IN_LINE +
						      i * MAX_FONTS_IN_LINE +
						      j].hight;

					/* if font is big put it in the common
					 * fonts array */
					if (fonts[c * MAX_LINES *
						  MAX_FONTS_IN_LINE +
						  i * MAX_FONTS_IN_LINE +
						  j].width > 4 &&
					    fonts[c * MAX_LINES *
						  MAX_FONTS_IN_LINE +
						  i * MAX_FONTS_IN_LINE +
						  j].hight > 4 &&
					    fonts[c * MAX_LINES *
						  MAX_FONTS_IN_LINE +
						  i * MAX_FONTS_IN_LINE +
						  j].width < MAX_FONT_WEIGHT &&
					    fonts[c * MAX_LINES *
						  MAX_FONTS_IN_LINE +
						  i * MAX_FONTS_IN_LINE +
						  j].hight < MAX_FONT_HIGHT)
					{
						object_width_counters[fonts
								      [c *
								       MAX_LINES
								       *
								       MAX_FONTS_IN_LINE
								       +
								       i *
								       MAX_FONTS_IN_LINE
								       +
								       j].
								      width -
								      2] += 1;
						object_width_counters[fonts
								      [c *
								       MAX_LINES
								       *
								       MAX_FONTS_IN_LINE
								       +
								       i *
								       MAX_FONTS_IN_LINE
								       +
								       j].
								      width -
								      1] += 1;
						object_width_counters[fonts
								      [c *
								       MAX_LINES
								       *
								       MAX_FONTS_IN_LINE
								       +
								       i *
								       MAX_FONTS_IN_LINE
								       +
								       j].
								      width +
								      0] += 1;
						object_hight_counters[fonts
								      [c *
								       MAX_LINES
								       *
								       MAX_FONTS_IN_LINE
								       +
								       i *
								       MAX_FONTS_IN_LINE
								       +
								       j].
								      hight -
								      2] += 1;
						object_hight_counters[fonts
								      [c *
								       MAX_LINES
								       *
								       MAX_FONTS_IN_LINE
								       +
								       i *
								       MAX_FONTS_IN_LINE
								       +
								       j].
								      hight -
								      1] += 1;
						object_hight_counters[fonts
								      [c *
								       MAX_LINES
								       *
								       MAX_FONTS_IN_LINE
								       +
								       i *
								       MAX_FONTS_IN_LINE
								       +
								       j].
								      hight -
								      0] += 1;
					}

					if (j < (num_of_fonts[c][i] - 1))
					{
						avg_diff_between_fonts_in_page
							+=
							(fonts
							 [c * MAX_LINES *
							  MAX_FONTS_IN_LINE +
							  i *
							  MAX_FONTS_IN_LINE +
							  j].x1 -
							 fonts[c * MAX_LINES *
							       MAX_FONTS_IN_LINE
							       +
							       i *
							       MAX_FONTS_IN_LINE
							       + j + 1].x2);
					}
				}
			}
			/* this line is too thin */
			else
			{
				num_of_fonts[c][i] = 0;
			}
		}
	}

	if (num_of_fonts_in_page != 0)
	{
		avg_font_width_in_page /= num_of_fonts_in_page;
		avg_font_hight_in_page /= num_of_fonts_in_page;

		if (num_of_fonts_in_page != 1)
			avg_diff_between_fonts_in_page /=
				(num_of_fonts_in_page - 1);
	}

	/* get the common width and hight (common weight or hight sould not be
	 * * smaller then the avvarage */
	pix->common_width_of_objects = 0;
	pix->common_hight_of_objects = 0;

	/* set common hight and width */
	for (i = 1; i < MAX_FONT_HIGHT; i++)
	{
		if (object_hight_counters[i] >
		    object_hight_counters[pix->common_hight_of_objects])
			pix->common_hight_of_objects = i;
	}
	for (i = 1; i < MAX_FONT_WIDTH; i++)
	{
		if (object_width_counters[i] >
		    object_width_counters[pix->common_width_of_objects])
			pix->common_width_of_objects = i;
	}

	/* get line equations for non horizontal lines */
	pix->num_of_lines = 0;
	for (c = 0; c < num_of_columns_in_page; c++)
	{
		for (i = 0; i < num_of_lines[c]; i++)
		{
			/* ignure lines with no fonts */
			if (num_of_fonts[c][i] == 0)
				continue;

			find_font_baseline_eq (pix, lines[c * MAX_LINES + i],
					       &(fonts[c * MAX_LINES *
						       MAX_FONTS_IN_LINE +
						       i * MAX_FONTS_IN_LINE]),
					       &(line_eqs[c][i][0]),
					       &(line_eqs[c][i][1]),
					       num_of_fonts[c][i]);

			pix->num_of_lines++;
		}
	}

	/* color the results of page layout functions */
	if (pix->command & HOCR_COMMAND_COLOR_BOXES)
	{
		for (c = 0; c < num_of_columns_in_page; c++)
		{
			color_hocr_box (pix, columns[c], 1, 0);
			for (i = 0; i < num_of_lines[c]; i++)
			{
				if (num_of_fonts[c][i] == 0)
					continue;
				/* color line boxes */
				color_hocr_line_eq (pix,
						    &(line_eqs
						      [c][i]
						      [0]),
						    lines[c * MAX_LINES +
							  i].x1,
						    lines[c * MAX_LINES +
							  i].x2, 1, 100);
				color_hocr_line_eq (pix, &(line_eqs[c][i][1]),
						    lines[c * MAX_LINES +
							  i].x1,
						    lines[c * MAX_LINES +
							  i].x2, 2, 100);
				/* color individual font boxes */
				for (j = 0; j < num_of_fonts[c][i]; j++)
				{
					/* if arteffact do not recognize */
					if (fonts
					    [c * MAX_LINES * MAX_FONTS_IN_LINE +
					     i * MAX_FONTS_IN_LINE + j].width <
					    3
					    || fonts[c * MAX_LINES *
						     MAX_FONTS_IN_LINE +
						     i * MAX_FONTS_IN_LINE +
						     j].hight < 3)
						continue;
					if (fonts
					    [c * MAX_LINES * MAX_FONTS_IN_LINE +
					     i * MAX_FONTS_IN_LINE + j].width >
					    (3.5 *
					     (double)
					     avg_font_width_in_page)
					    || fonts[c * MAX_LINES *
						     MAX_FONTS_IN_LINE +
						     i * MAX_FONTS_IN_LINE +
						     j].hight >
					    (3.5 *
					     (double) avg_font_hight_in_page))
						continue;
					color_hocr_box (pix,
							fonts[c * MAX_LINES *
							      MAX_FONTS_IN_LINE
							      +
							      i *
							      MAX_FONTS_IN_LINE
							      + j], 1, 0);
				}
			}
		}
	}

	/* do ocr ? */
	pix->progress_phase = 3;
	font_number = 0;
	if ((pix->command & HOCR_COMMAND_OCR))
	{
		/* get all fonts for all the lines */
		for (c = 0; c < num_of_columns_in_page; c++)
		{
			for (i = 0; i < num_of_lines[c]; i++)
			{
				/* check for indentation */
				if (fonts
				    [c * MAX_LINES * MAX_FONTS_IN_LINE +
				     i * MAX_FONTS_IN_LINE + 0].x2 <
				    (avg_line_x_start_in_column[c] -
				     NUM_OF_FONTS_IN_INDENT *
				     avg_font_width_in_page))
				{
					indent = TRUE;

					if ((pix->
					     command &
					     HOCR_COMMAND_USE_INDENTATION) != 0)
					{
						if ((pix->
						     command &
						     HOCR_COMMAND_USE_SPACE_FOR_TAB)
						    != 0)
						{
							hocr_text_buffer_add_string
								(text_buffer,
								 "    ");
						}
						else
						{
							hocr_text_buffer_add_string
								(text_buffer,
								 "\t");
						}
					}
				}
				else
				{
					indent = FALSE;
				}

				for (j = 0; j < num_of_fonts[c][i]; j++)
				{
					/* progress the progress indicator */
					/* start at 128 because 128 is the end
					 * of object count */
					pix->progress =
						128 +
						((double) font_number /
						 (double) num_of_fonts_in_page)
						* 127.0;
					font_number++;

					if ((j + 1) < num_of_fonts[c][i])
					{
						/* check for end of word */
						end_of_word =
							((fonts
							  [c * MAX_LINES *
							   MAX_FONTS_IN_LINE +
							   i *
							   MAX_FONTS_IN_LINE +
							   j].x1 -
							  fonts[c * MAX_LINES *
								MAX_FONTS_IN_LINE
								+
								i *
								MAX_FONTS_IN_LINE
								+ j + 1].x2) >
							 (avg_font_width_in_page
							  / 2));

						/* check for tabs */
						if (avg_font_width_in_page)
							tabs = (fonts
								[c *
								 MAX_LINES *
								 MAX_FONTS_IN_LINE
								 +
								 i *
								 MAX_FONTS_IN_LINE
								 + j].x1 -
								fonts[c *
								      MAX_LINES
								      *
								      MAX_FONTS_IN_LINE
								      +
								      i *
								      MAX_FONTS_IN_LINE
								      + j +
								      1].x2) /
								(NUM_OF_FONTS_IN_TAB
								 *
								 avg_font_width_in_page);
						else
							tabs = 0;
					}

					/* if arteffact do not recognize */
					if (fonts
					    [c * MAX_LINES * MAX_FONTS_IN_LINE +
					     i * MAX_FONTS_IN_LINE + j].width <
					    3
					    || fonts[c * MAX_LINES *
						     MAX_FONTS_IN_LINE +
						     i * MAX_FONTS_IN_LINE +
						     j].hight < 3)
					{
						/* check for end of word */
						if (end_of_word)
							hocr_text_buffer_add_string
								(text_buffer,
								 " ");
						continue;
					}

					if (fonts
					    [c * MAX_LINES * MAX_FONTS_IN_LINE +
					     i * MAX_FONTS_IN_LINE + j].width >
					    (3.5 *
					     (double)
					     avg_font_width_in_page)
					    || fonts[c * MAX_LINES *
						     MAX_FONTS_IN_LINE +
						     i * MAX_FONTS_IN_LINE +
						     j].hight >
					    (3.5 *
					     (double) avg_font_hight_in_page))
					{
						/* check for end of word */
						if (end_of_word)
							hocr_text_buffer_add_string
								(text_buffer,
								 " ");
						continue;
					}

					/* recognize font */
					hocr_recognize_font (pix,
							     &(fonts[c *
								     MAX_LINES
								     *
								     MAX_FONTS_IN_LINE
								     +
								     i *
								     MAX_FONTS_IN_LINE]),
							     num_of_fonts[c][i],
							     j, line_eqs[c][i],
							     chars, &symbols);

					/* color unknown fonts in the pixbuf */
					if ((!chars[0] || chars[0] == '*')
					    && (pix->
						command &
						HOCR_COMMAND_COLOR_MISREAD))
						color_hocr_box_full (pix,
								     fonts[c *
									   MAX_LINES
									   *
									   MAX_FONTS_IN_LINE
									   +
									   i *
									   MAX_FONTS_IN_LINE
									   + j],
								     1, 255,
								     TRUE);

					/* add new recognizzed fonts to text */
					hocr_text_buffer_add_string
						(text_buffer, chars);

					/* check for end of word */
					if (end_of_word)
						hocr_text_buffer_add_string
							(text_buffer, " ");

					/* check for tabs */
					if ((pix->
					     command &
					     HOCR_COMMAND_USE_SPACE_FOR_TAB) !=
					    0)
					{
						for (k = 0; k < tabs; k++)
						{
							hocr_text_buffer_add_string
								(text_buffer,
								 "    ");
						}
					}
					else
					{
						for (k = 0; k < tabs; k++)
						{
							hocr_text_buffer_add_string
								(text_buffer,
								 "\t");
						}
					}

					/* if more then one font processed
					 * incrise i */
					i += symbols;
				}

				/* end of line */
				hocr_text_buffer_add_string (text_buffer, "\n");

				/* check for end of paragraph */
				if (num_of_lines[c] > (i + 1)
				    && 3 * lines[c * MAX_LINES + i].hight <
				    (lines[c * MAX_LINES + i + 1].y1 -
				     lines[c * MAX_LINES + i].y2))
					hocr_text_buffer_add_string
						(text_buffer, "\n");
			}

			/* end of column */
			hocr_text_buffer_add_string (text_buffer, "\n");
		}
	}

	/* free memory */
	free (fonts);
	free (lines);
	free (columns);

#ifdef WITH_HSPELL

	/* use the dictionary spell check */
	if (pix->command & HOCR_COMMAND_DICT)
	{
		hocr_text_buffer *iso_text_buffer = NULL;
		hocr_text_buffer *utf_text_buffer = NULL;
		hocr_text_buffer *iso_text_buffer_spell = NULL;

		/* hspell only work for iso-8859-8 */
		iso_text_buffer = hocr_text_buffer_copy_convert (text_buffer,
								 "iso8859-8",
								 "utf-8");

		/* create a spelled buffer using hspell */
		iso_text_buffer_spell =
			hocr_text_buffer_copy_hspell (iso_text_buffer);

		/* free the iso8859-8 buffer */
		hocr_text_buffer_unref (iso_text_buffer);

		/* FIXME: do double spell ? */
		/* create a spelled buffer using hspell */
		iso_text_buffer =
			hocr_text_buffer_copy_hspell (iso_text_buffer_spell);

		/* free the iso8859-8 buffer */
		hocr_text_buffer_unref (iso_text_buffer_spell);
		
		/* convert text back to utf-8 */
		utf_text_buffer =
			hocr_text_buffer_copy_convert (iso_text_buffer,
						       "utf-8", "iso8859-8");

		/* free temporary iso8859-8 spelled buffer */
		hocr_text_buffer_unref (iso_text_buffer);

		/* copy spelled utf-8 back to text_buffer */
		hocr_text_buffer_set_string (text_buffer,
					     utf_text_buffer->text);

		/* free temporary utf text buffer */
		hocr_text_buffer_unref (utf_text_buffer);
	}

#endif /* WITH_HSPELL */

	return 0;
}
