/*
 * Copyright (c) 2003 FlashCode <flashcode@flashtux.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#ifndef _GMEMO_DISPLAY_H
#define _GMEMO_DISPLAY_H    1

#include <gtk/gtk.h>

extern GtkWidget       *window, *frame;
extern GtkWidget       *hbox;
extern GtkWidget       *vbox;
extern GtkWidget       *drawing_area, *drawing_config;
extern GtkWidget       *eventbox_heure, *eventbox_config, *eventbox_date;
extern GtkWidget       *eventbox_fete, *eventbox_memo;
extern GtkWidget       *label_date, *label_fete, *label_memo;
extern GdkPixmap       *gdkpixmap;
extern GdkBitmap       *gdkbitmap;
extern GdkGC           *gc;
extern GdkColor        color_bg_window;
extern GdkColor        color_fg_digits;
extern GdkColor        color_fg_date;
extern GdkColor        color_fg_fete;
extern GdkColor        color_fg_memo;
extern GdkColor        color_bg_alert;

extern void gmemo_create_window(int, char *[]);
extern void gmemo_init_window_gtk();
extern void gmemo_display_feasts(guchar *, guchar *);
extern void gmemo_display_digital(guchar *, guchar *, gint, gint, gint, gint, gint);
extern void gmemo_display_date();
extern void gmemo_clear_area();

#endif /* gmemo_display.h */
