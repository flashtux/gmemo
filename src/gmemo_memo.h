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


#ifndef _GMEMO_MEMO_H
#define _GMEMO_MEMO_H  1

#include <gtk/gtk.h>

#include "gmemo.h"


typedef enum
{
	REPEAT_NONE = 0,
	REPEAT_SEC,
	REPEAT_MIN,
	REPEAT_HOUR,
	REPEAT_DAY,
    REPEAT_WEEK,
    REPEAT_MONTH,
    REPEAT_YEAR,
    REPEAT_COUNT
} t_repeat;


typedef struct
{
    gboolean    active;             // active memo (false = old memo, discarded)
    gchar       *title;             // short description of memo (title of alert window, if any)
    gchar       *message;           // (optional) message for memo window
    glong       alarm;              // date/time of alarm (read from file)
    glong       last_alarm;         // last date/time alarm was displayed (0 = never displayed)
    glong       next_alarm;         // date/time of next alarm (calculated by program)
    t_repeat    repeat_type;        // repeat type
    gint        interval;           // number of [units]
    gint        repeat_remaining;   // remaining repeats (-1 = infinite repeats)
    gchar       *sound_file;        // (optional) sound filename for alarm
    gint        sound_repeat;       // number of repeats for sound (must be >= 1)
    gint        sound_delay;        // delay between each repeat in ms (only if sound_repeat > 1)
    gchar       *exec_cmd;          // (optional) command to execute when alarm
} t_memo;


extern gchar        *repeat_strings[REPEAT_COUNT];
extern GSList       *memos;
extern gboolean     memos_ok;


extern gint get_num_days_month(gint, gint);
extern glong date_to_glong(gint, gint, gint, gint, gint, gint);
extern void set_first_alarm(t_memo *);
extern void display_memos(GSList *);
extern void check_alarms(GSList *);
extern void destroy_memos(GSList *);

#endif /* gmemo_memo.h */
