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


#ifndef __GMEMO_H
#define __GMEMO_H   1

#include <stdio.h>
#include <gtk/gtk.h>


// debug mode, 0 for normal use, 1 or 2 for developers (2 = full debug messages)
#define DEBUG               0

// internationalization
#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

// log filename

#define GMEMO_LOG_NAME  "gmemo.log"

// gmemo messages, warnings, errors

#if DEBUG >= 1
    #define GMEMO_DEBUG(fmt, argz...) \
        gmemo_message("[DEBUG] gmemo: " fmt, ##argz)
#else
    #define GMEMO_DEBUG(fmt, argz...) /* */
#endif

#define GMEMO_MESSAGE(fmt, argz...) \
	gmemo_message("gmemo: " fmt, ##argz)

#define GMEMO_WARNING(fmt, argz...) \
	gmemo_message("gmemo warning: " fmt, ##argz)

#define GMEMO_ERROR(fmt, argz...) \
	gmemo_message("gmemo error: " fmt, ##argz)

#define LICENCE \
    N_( \
    "GMEMO " VERSION " (c) Copyright 2002-2003 by FlashCode (Sebastien Helleu)\n\n" \
    "This program is free software; you can redistribute it and/or modify\n" \
    "it under the terms of the GNU General Public License as published by\n" \
    "the Free Software Foundation; either version 2 of the License, or\n" \
    "(at your option) any later version.\n" \
    "\n" \
    "This program is distributed in the hope that it will be useful,\n" \
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n" \
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" \
    "GNU General Public License for more details.\n" \
    "\n" \
    "You should have received a copy of the GNU General Public License\n" \
    "along with this program; if not, write to the Free Software\n" \
    "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n\n")

#define USAGE \
    N_( \
    "GMEMO " VERSION " (c) Copyright 2002-2003 by FlashCode (Sebastien Helleu)\n\n" \
    "  -d, --daemon     run gmemo as a daemon (no display, Linux only)\n" \
    "  -h, --help       this help screen\n" \
    "  -l, --licence    display program licence\n" \
    "  -q, --quiet      doesn't display any message\n" \
    "  -s, --seconds    display seconds\n" \
    "  -v, --verbose    verbose mode\n" \
    "  -w, --withdrawn  runs gmemo in withdrawn mode (for slit or window maker, Linux only)\n\n")
    


typedef enum
{
    COLOR_BG_WINDOW = 0,
    COLOR_FG_DIGITS,
    COLOR_FG_DATE,
    COLOR_FG_LINE1,
    COLOR_FG_LINE2,
    COLOR_BG_ALERT,
    GMEMO_COLOR_COUNT
} t_gmemo_color;

typedef enum
{
    FONT_DATE = 0,
    FONT_LINE1,
    FONT_LINE2,
    GMEMO_FONT_COUNT
} t_gmemo_font;

struct t_date_time
{
    GDate   date;
    guint   hour : 5;
    guint   min  : 6;
    guint   sec  : 6;
};

extern gboolean             gmemo_daemon;
extern gboolean             gmemo_withdrawn;
extern gboolean             gmemo_quiet;
extern gboolean             gmemo_verbose;
extern gchar                gmemo_display[256];
extern gint                 gmemo_win_width;
extern gint                 gmemo_win_height;
extern gint                 gmemo_size_digits;
extern gboolean             gmemo_display_seconds;
extern gchar                gmemo_colors[GMEMO_COLOR_COUNT][64];
extern gchar                gmemo_colors_default_gtk[GMEMO_COLOR_COUNT][64];
extern gchar                gmemo_colors_default_wmaker[GMEMO_COLOR_COUNT][64];
extern gchar                gmemo_fonts[GMEMO_FONT_COUNT][128];
extern gchar                gmemo_fonts_default[GMEMO_FONT_COUNT][128];

extern gchar                gmemo_home_dir[1024];

extern FILE                 *gmemo_log_file;

extern struct t_date_time   date_of_day;

extern gchar                new_time[16];
extern gchar                old_time[16];

extern gint                 last_min;
extern gint                 last_day;

extern gint                 mouse_over_config;

extern gint                 chiffre[10];

extern gint                 coord_barre[7][2];
extern gint                 long_barre[7][3];

extern gchar                day_name[7][9];
extern gchar                short_day_name[7][4];
extern gchar                month_name[12][10];
extern gchar                short_month_name[12][5];
extern gchar                feast_type[12][31];
extern gchar                feast[12][31][128];

extern gint                 inverse;


extern void gmemo_init();
extern void gmemo_exit(gint);
extern void gmemo_display_usage();
extern void gmemo_display_licence();
extern void gmemo_message(gchar *, ...);
extern void signal_usr1(int);
extern void display_time(gboolean);
extern gboolean update_time(gboolean);

#endif /* gmemo.h */
