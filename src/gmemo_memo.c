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

/* gmemo_memo.c: memos management */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>
    #include <mmsystem.h>
#else
    #include <esd.h>
#endif

#include "gmemo.h"
#include "gmemo_memo.h"
#include "gmemo_loadsave.h"


gchar       *repeat_strings[REPEAT_COUNT] =
            { "no", "sec", "min", "hour", "day", "week", "month", "year" };
glong       unit_repeat[REPEAT_COUNT] =
            { 0, 1, 60, 3600, 3600*24, 3600*24*7,
              0, 0      /* special for month/year repeat */
            };

GSList      *memos;

gboolean    memos_ok;               /* true if memos loaded & ready for check & alarms */


/*
 * get_num_days_month: returns the number of days in a month
 */

gint get_num_days_month(gint month, gint year)
{
    gint    num_days[12] =
            { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    
    return ((month == 2) && (year%4 == 0)) ? 29 : num_days[month-1];
}
            
/*
 * date_to_gulong: converts a date to a glong integer
 */

glong date_to_glong(gint year, gint month, gint day, gint hour, gint min, gint sec)
{
    struct tm   tm_tmp;
    
    tm_tmp.tm_year = year-1900;
    tm_tmp.tm_mon = month-1;
    tm_tmp.tm_mday = day;
    tm_tmp.tm_hour = hour;
    tm_tmp.tm_min = min;
    tm_tmp.tm_sec = sec;
    tm_tmp.tm_isdst = -1;
    return (glong)mktime(&tm_tmp);
}

/*
 * date_add_months: add X months to a date
 */

glong date_add_months(glong ref_date, gint num_months)
{
    gint        i;
    struct tm   *ptr_tm, tm_tmp;
    
    ptr_tm = localtime(&ref_date);
    memcpy(&tm_tmp, ptr_tm, sizeof(struct tm));
    for (i = 0; i < num_months; i++)
    {
        if (tm_tmp.tm_mon == 11)
        {
            tm_tmp.tm_mon = 0;
            tm_tmp.tm_year++;
        }
        else
            tm_tmp.tm_mon++;
    }
    if (tm_tmp.tm_mday > get_num_days_month(tm_tmp.tm_mon+1, tm_tmp.tm_year+1900))
        return 0;   /* invalid date */
    else
        return date_to_glong(tm_tmp.tm_year+1900, tm_tmp.tm_mon+1, tm_tmp.tm_mday, tm_tmp.tm_hour, tm_tmp.tm_min, tm_tmp.tm_sec);
}

/*
 * date_add_years: add X years to a date
 */

glong date_add_years(glong ref_date, gint num_years)
{
    struct tm   *ptr_tm, tm_tmp;
    
    ptr_tm = localtime(&ref_date);
    memcpy(&tm_tmp, ptr_tm, sizeof(struct tm));
    tm_tmp.tm_year += num_years;
    if (tm_tmp.tm_mday > get_num_days_month(tm_tmp.tm_mon+1, tm_tmp.tm_year+1900))
        return 0;   /* invalid date */
    else
        return date_to_glong(tm_tmp.tm_year+1900, tm_tmp.tm_mon+1, tm_tmp.tm_mday, tm_tmp.tm_hour, tm_tmp.tm_min, tm_tmp.tm_sec);
}

/*
 * set_first_alarm: calculates first alarm for a memo
 */

void set_first_alarm(t_memo *memo)
{
    glong   current_time, ref_time, next_time;
    gint    i;
    
    current_time = (glong)(time(NULL));
    // if alarm is in the future, set next alarm to alarm
    if (memo->alarm >= current_time)
        memo->next_alarm = memo->alarm;
    else
    {
        // no repeat or no more repeat ?
        if ( (memo->repeat_type == REPEAT_NONE) || (memo->repeat_remaining == 0) )
        {
            memo->active = FALSE;
            memo->next_alarm = 0;
            #if DEBUG >= 1
            GMEMO_DEBUG("set_first_alarm: memo '%s' inactive ! (too old!)", memo->title);
            #endif
        }
        else
        {
            ref_time = (memo->last_alarm > memo->alarm) ? memo->last_alarm : memo->alarm;
            if ( (memo->repeat_type == REPEAT_MONTH) || (memo->repeat_type == REPEAT_YEAR) )
            {
                i = memo->interval;
                do
                {
                    next_time = (memo->repeat_type == REPEAT_MONTH) ? date_add_months(ref_time, i) : date_add_years(ref_time, i);
                    i += memo->interval;
                } while (next_time <= 0);
                memo->next_alarm = next_time;
            }
            else
            {
                next_time = (current_time - memo->alarm) / (memo->interval * unit_repeat[memo->repeat_type]);
                next_time = memo->alarm + (next_time * memo->interval * unit_repeat[memo->repeat_type]);
                if (next_time < current_time)
                    next_time += (memo->interval * unit_repeat[memo->repeat_type]);
                memo->next_alarm = next_time;
            }
        }
    }
}

/*
 * set_next_alarm: calculates next alarm for a memo (if repeat)
 */

void set_next_alarm(t_memo *memo)
{
    glong   current_time, ref_time, next_time, i;
    
    current_time = (glong)(time(NULL));
    // no repeat or no more repeat ?
    if ( (memo->repeat_type == REPEAT_NONE) || (memo->repeat_remaining == 0) )
    {
        memo->active = FALSE;
        memo->next_alarm = 0;
        #if DEBUG >= 1
        GMEMO_DEBUG("memo '%s' inactive ! (too old!)", memo->title);
        #endif
    }
    else
    {
        ref_time = (memo->last_alarm > memo->alarm) ? memo->last_alarm : memo->alarm;
        if ( (memo->repeat_type == REPEAT_MONTH) || (memo->repeat_type == REPEAT_YEAR) )
        {
            i = memo->interval;
            do
            {
                next_time = (memo->repeat_type == REPEAT_MONTH) ? date_add_months(ref_time, i) : date_add_years(ref_time, i);
                i += memo->interval;
            } while (next_time < current_time);
            memo->next_alarm = next_time;
        }
        else
            memo->next_alarm = memo->next_alarm + (memo->interval * unit_repeat[memo->repeat_type]);
    }
}

/*
 * display_memos: display memos list
 */

void display_memos(GSList *ptr_memos)
{
    int     num;
    t_memo  *memo;
    time_t  time;
    gchar   *string, string_alarm[64], string_next_alarm[64], *pos;
    
    num = 0;
    GMEMO_MESSAGE("displaying memos...");
    while (ptr_memos)
    {
        memo = (t_memo *)(ptr_memos->data);
        time = memo->alarm;
        string = (gchar *)ctime(&time);
        strcpy(string_alarm, string);
        pos = (gchar *)(strchr(string_alarm, '\n'));
        if (pos != NULL)
            pos[0] = '\0';
        time = memo->next_alarm;
        string = (gchar *)ctime(&time);
        strcpy(string_next_alarm, string);
        pos = (gchar *)(strchr(string_next_alarm, '\n'));
        if (pos != NULL)
            pos[0] = '\0';
        GMEMO_MESSAGE("memo #%d: active=%s, short lib='%s', long lib='%s' alarm='%d => %s' next alarm='%d => %s' repeat_type='%s' interval='%d', repeat_remaining='%d'",
            num, (memo->active)?"Yes":"No", memo->title, memo->message,
            memo->alarm, string_alarm, memo->next_alarm, string_next_alarm, 
            repeat_strings[memo->repeat_type], memo->interval, memo->repeat_remaining);
        ptr_memos = ptr_memos->next;
        num++;
    }
    GMEMO_MESSAGE("end of memos");
}

void bouton_ok(GtkWidget *widget, gpointer data)
{
    gtk_widget_destroy(GTK_WIDGET(data));
}

/*
 * play_sound_file: plays sound file
 */

void play_sound_file(gchar *filename, gint sound_repeat, gint sound_delay)
{    
    #ifdef _WIN32
    /* TODO : repeat for windows */
    if (sound_repeat > 0)
        GMEMO_WARNING("repeat does not work under windows...");
    if (!PlaySound(filename, 0, SND_ASYNC | SND_FILENAME))
        GMEMO_WARNING("error playing sound '%s'", filename);
    #else
    int     pid, i;
    
    pid = fork();
    if (pid < 0)
        return;
    
    if (pid == 0)
    {
        if (sound_repeat < 1)
        {
            sound_repeat = 1;
            sound_delay = 0;
        }
        for (i = 0; i < sound_repeat; i++)
        {
            esd_play_file(NULL, filename, 1);
            usleep(sound_delay*1000);
        }
        _exit(0);
    }
    #endif
}

/*
 * execute_command: execute a file
 */

void execute_command(gchar *command)
{
    gchar   **argv;
    
    argv = g_strsplit(command, " ", 127);
    if (!g_spawn_async(".", argv, NULL, 0, NULL, NULL, NULL, NULL))
        GMEMO_WARNING("error launching command '%s'", command);
    g_strfreev(argv);
}

/*
 * display_alarm: display an alarm for a memo
 */

void display_alarm(t_memo *memo)
{
    GtkWidget   *window_memo, *vbox, *label, *label_missed, *button;
    gchar       window_title[128], *pos;
    glong       current_time;
    
    window_memo = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    strcpy(window_title, ctime(&(memo->next_alarm)));
    pos = (gchar *)(strchr(window_title, '\n'));
    pos[0] = '\0';
    strcat(window_title, " - ");
    strcat(window_title, memo->title);
    gtk_window_set_title(GTK_WINDOW(window_memo), window_title);
    gtk_window_set_default_size(GTK_WINDOW(window_memo), 320, 200);
    vbox = gtk_vbox_new(FALSE, 5);
    label = gtk_label_new(memo->message);
    current_time = (glong)(time(NULL));
    if (memo->next_alarm < current_time-120)
    {
        label_missed = gtk_label_new("<b>Missed alarm !</b>");
        gtk_label_set_use_markup(GTK_LABEL(label_missed), TRUE);
        gtk_box_pack_start(GTK_BOX(vbox), label_missed, TRUE, TRUE, 0);
    }
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);
    button = gtk_button_new_with_label("OK");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window_memo), vbox);
    gtk_widget_show_all(window_memo);
    gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(bouton_ok), window_memo);
    if (memo->sound_file)
        play_sound_file(memo->sound_file, memo->sound_repeat, memo->sound_delay);
    if (memo->exec_cmd)
        execute_command(memo->exec_cmd);
}

/*
 * check_alarms: check for alarms, and display alarms if date/time >= alarm
 */

void check_alarms(GSList *list_memos)
{
    glong       current_time;
    GSList      *ptr_memos;
    t_memo      *memo;
    gboolean    save_flag;
    
    if (!memos_ok)
        return;
    current_time = (glong)(time(NULL));
    ptr_memos = list_memos;
    save_flag = FALSE;
    while (ptr_memos != NULL)
    {
        memo = (t_memo *)(ptr_memos->data);
        if (memo->active)
        {
            if (memo->next_alarm <= current_time)
            {
                if (memo->message)
                    display_alarm(memo);
                set_next_alarm(memo);
                save_flag = TRUE;
            }
        }
        ptr_memos = ptr_memos->next;
    }
    if (save_flag)
        save_memos(GMEMO_MEMOS_TEMP);
}

/*
 * destroy_memos: destroy all memos
 */

void destroy_memos(GSList *list_memos)
{
    GSList  *ptr_memos;
    t_memo  *memo;
    
    ptr_memos = list_memos;
    while (ptr_memos)
    {
        memo = (t_memo *)(ptr_memos->data);
        if (memo->title)
            g_free(memo->title);
        if (memo->message)
            g_free(memo->message);
        if (memo->sound_file)
            g_free(memo->sound_file);
        if (memo->exec_cmd)
            g_free(memo->exec_cmd);
        ptr_memos = ptr_memos->next;
    }
    g_slist_free(list_memos);
}
