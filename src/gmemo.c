/* ############################################################################
 * ###               _________                                              ###
 * ###               __  ____/______ _______________ _________              ###
 * ###               _  / __ __  __ `__ \  _ \_  __ `__ \  __ \             ###
 * ###               / /_/ / _  / / / / /  __/  / / / / / /_/ /             ###
 * ###               \____/  /_/ /_/ /_/\___//_/ /_/ /_/\____/              ###
 * ###                                                                      ###
 * ###                      Gmemo,  Copyright (c) 2003                      ###
 * ###                                                                      ###
 * ###                 By FlashCode <flashcode@flashtux.org>                ###
 * ###                                                                      ###
 * ###                  Web page: http://gmemo.flashtux.org                 ###
 * ###                                                                      ###
 * ############################################################################
 *
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

/* gmemo.c: core functions */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <errno.h>
                                                                                                                            
#include <gtk/gtk.h>

#include "gmemo.h"
#include "gmemo_display.h"
#include "gmemo_loadsave.h"
#include "gmemo_feast.h"
#include "gmemo_memo.h"


/*
 * Variables
 */

/* user can modify these variables with command line options */

gboolean        gmemo_daemon;                           /* daemon mode              */
gboolean        gmemo_withdrawn;                        /* withdrawn mode           */
gboolean        gmemo_quiet;                            /* quiet mode               */
gboolean        gmemo_verbose;                          /* verbose mode             */
gchar           gmemo_display[256];                     /* display (for withdrawn)  */
gint            gmemo_win_width;                        /* window width             */
gint            gmemo_win_height;                       /* window height            */
gint            gmemo_size_digits;                      /* digits size              */
gboolean        gmemo_display_seconds;                  /* display seconds          */
gchar           gmemo_colors[GMEMO_COLOR_COUNT][64];    /* colors                   */
gchar           gmemo_fonts[GMEMO_FONT_COUNT][128];     /* fonts                    */

/* default values */

gchar   gmemo_colors_default_wmaker[GMEMO_COLOR_COUNT][64] =
        { "000000000000",                           /* bg window            */
          "0000FFFF0000",                           /* fg digits            */
          "AAAAAAAAFFFF",                           /* fg date              */
          "FFFFFFFFAAAA",                           /* fg line 1            */
          "9999CCCCCCCC",                           /* fg line 2            */
          "FFFF77777777"                            /* bg alert             */
       };

gchar   gmemo_colors_default_gtk[GMEMO_COLOR_COUNT][64] =
        { "000000000000",                           /* bg window            */
          "00000000AAAA",                           /* fg digits            */
          "000000006666",                           /* fg date              */
          "000066665151",                           /* fg line 1            */
          "AAAA5555AAAA",                           /* fg line 2            */
          "FFFF77777777"                            /* bg alert             */
       };
                
gchar   gmemo_fonts_default[GMEMO_FONT_COUNT][128] =
        { "-*-helvetica-bold-r-normal-*-10-*-*-*-*-*-*-*",      /* date     */
          "-*-helvetica-medium-r-normal-*-10-*-*-*-*-*-*-*",    /* line 1   */
          "-*-helvetica-medium-r-normal-*-8-*-*-*-*-*-*-*"      /* line 2   */
        };
            
/* other variables */
        
gchar           gmemo_home_dir[1024];               /* gmemo home directory */
        
FILE            *gmemo_log_file;                    /* log file             */
        
struct t_date_time  date_of_day;                    /* system date & time   */

gchar           old_time[16];                       /* last time displayed  */
gchar           new_time[16];                       /* new time to display  */

gint            last_min;                           /* last min displayed   */
gint            last_day;                           /* last day displayed   */

gint            mouse_over_config;

gint            inverse;

gint            chiffre[10] =                       /* digital digits       */
                { /* 0 */ 1+4+8+16+32+64,           /*                      */
                  /* 1 */ 16+64,                    /*          1           */
                  /* 2 */ 1+2+4+16+32,              /*      +-------+       */
                  /* 3 */ 1+2+4+16+64,              /*      |       |       */
                  /* 4 */ 2+8+16+64,                /*    8 |   2   | 16    */
                  /* 5 */ 1+2+4+8+64,               /*      |-------|       */
                  /* 6 */ 1+2+4+8+32+64,            /*      |       |       */
                  /* 7 */ 1+16+64,                  /*   32 |   4   | 64    */
                  /* 8 */ 1+2+4+8+16+32+64,         /*      +-------+       */
                  /* 9 */ 1+2+4+8+16+64 };          /*                      */

gint            coord_barre[7][2] =
                { {0, 0}, {0, 1}, {0, 2},           /* les 3 barres horiz */
                  {0, 0}, {1, 0}, {0, 1}, {1, 1} }; /* les 4 barres vertic */

/* longueur barres:  0 = longue   1 = plus courte   2 = encore plus courte */

gint            long_barre[7][3] =
                { { 0, 1, 2 }, { 1, 0, 1 }, { 2, 1, 0 },
                  { 0, 1, 2 }, { 2, 1, 0 }, { 0, 1, 2 }, { 2, 1, 0 } };

gchar           day_name[7][9] =
                { N_("Monday"), N_("Tuesday"), N_("Wednesday"), 
                  N_("Thursday"), N_("Friday"), N_("Saturday"), N_("Sunday") };

gchar           short_day_name[7][4] =
                { N_("Mon"), N_("Tue"), N_("Wed"), N_("Thu"), N_("Fri"), 
                  N_("Sat"), N_("Sun") };

gchar           month_name[12][10] =
                { N_("January"), N_("Februar"), N_("March"), N_("April"), 
                  N_("May"), N_("June"), N_("July"), N_("August"),
                  N_("September"), N_("October"), N_("November"), 
                  N_("December") };

gchar           short_month_name[12][5] =
                { N_("Jan"), N_("Feb"), N_("Mar"), N_("Apr"), N_("May"),
                  N_("Jun"), N_("Jul"), N_("Aug"), N_("Sep"), N_("Oct"), 
                  N_("Nov"), N_("Dec") };

gchar           feast_type[12][31];

gchar           feast[12][31][128];



/* ========================================================================= */


/*
 * xmalloc: an malloc interface
 */
void *xmalloc(size_t size) {

  void *tmp=malloc(size);

  if(tmp == NULL)
    {

      GMEMO_ERROR("Virtual memory exhausted\n");
      gmemo_exit(EXIT_FAILURE);

    }

  return tmp;

}

/*
 * exist_directory
 */
gboolean exist_directory(const char *dirname)
{

  return opendir(dirname) != NULL;

}

/*
 * create_directory
 */
void create_directory(const char *dirname)
{

  int return_code = mkdir(dirname, 0755);
  extern int errno;

  if (return_code < 0)
    {
      if (errno != EEXIST)
        {
          GMEMO_ERROR("can't create directory '%s'.\n", dirname);
          gmemo_exit(EXIT_FAILURE);
        }
    }

}


/*
 * gmemo_init: init gmemo with default values and open log file
 */
void gmemo_init()
{
    gint    i;
    gchar   *home_dir, log_filename[4096];
    
    /* initialize some variables */
    memos_ok = FALSE;
    gmemo_daemon = FALSE;
    gmemo_withdrawn = FALSE;
    gmemo_verbose = FALSE;
    gmemo_display[0] = '\0';
    
    gmemo_win_width = -1;
    gmemo_win_height = -1;
    
    gmemo_size_digits = 7;
    gmemo_display_seconds = FALSE;
    
    /* default colors */
    for (i = 0; i < GMEMO_COLOR_COUNT; i++)
        strcpy(gmemo_colors[i], gmemo_colors_default_gtk[i]);   // FIXME for wmaker !
    
    for (i = 0; i < GMEMO_FONT_COUNT; i++)
        strcpy(gmemo_fonts[i], gmemo_fonts_default[i]);
    
    /* set gmemo home directory */
    home_dir = (gchar *)g_get_home_dir();
    if (home_dir == NULL)
        strcpy(gmemo_home_dir, "./");
    else
        sprintf(gmemo_home_dir, "%s/.gmemo/", home_dir);
    
    /* create gmemo log file */
    sprintf(log_filename, "%s/.gmemo/" GMEMO_LOG_NAME, home_dir);

    /* ceate gmemo home dire if doesn't exist */
    if(!exist_directory(gmemo_home_dir))
        create_directory(gmemo_home_dir);
    
    gmemo_log_file = fopen(log_filename, "w+");
    if (gmemo_log_file == NULL)
        GMEMO_WARNING("error opening gmemo log file '%s'", log_filename);
}

/*
 * gmemo_exit: exits from gmemo
 */

void gmemo_exit(gint return_code)
{
    fclose(gmemo_log_file);
    exit(return_code);
}

/*
 * gmemo_display_usage: displays gmemo usage
 */

void gmemo_display_usage()
{
    g_print("\n%s", USAGE);
    gmemo_exit(0);
}

/*
 * gmemo_display_licence: displays gmemo licence
 */

void gmemo_display_licence()
{
    g_print("\n%s", LICENCE);
    gmemo_exit(0);
}

/*
 * get_date_and_time: returns system date & time
 *                    returns : week day (0=sunday...6=saturday)
 *                              month day (1..31)
 *                              month (1..12)
 *                              year (4 digits)
 *                              hour, minutes, seconds
 */

void get_date_and_time(struct t_date_time *date)
{
    time_t      seconds;
    struct tm   *date_tmp;

    seconds = time(NULL);
    date_tmp = localtime(&seconds);
    date->hour = date_tmp->tm_hour;
    date->min  = date_tmp->tm_min;
    date->sec  = date_tmp->tm_sec;
    g_date_set_time(&(date->date), time(NULL));
}

/*
 * gmemo_message: displays a message (debug, message, warning, error)
 */

void gmemo_message(gchar *message, ...)
{
    gchar       buffer[8192], buffer2[8192];
    va_list     argptr;

    if (gmemo_quiet)
        return;
    
    va_start(argptr, message);
    vsprintf(buffer, message, argptr);
    va_end(argptr);
    get_date_and_time(&date_of_day);
    sprintf(buffer2, "[%04d/%02d/%02d %02d:%02d:%02d] %s\n",
        g_date_get_year(&date_of_day.date),
        g_date_get_month(&date_of_day.date)-G_DATE_JANUARY+1,
        g_date_get_day(&date_of_day.date),
        date_of_day.hour,
        date_of_day.min,
        date_of_day.sec,
        buffer);
    g_print(buffer2);
    if (gmemo_log_file != NULL)
        fprintf(gmemo_log_file, "%s", buffer2);
}

/*
 * signal_hup: SIGHUP signal handler
 */

#ifndef WIN32
void signal_usr1(int sig)
{
    GMEMO_MESSAGE("SIGUSR1 signal received, reloading memos...");
    memos_ok = FALSE;
    destroy_memos(memos);
    load_memos();
}
#endif

/* ========================================================================= */

/*
 * check_memos: TODO
 */

void check_memos(gboolean check_day, gboolean check_min)
{
    guchar  prenom_aujourdhui[256];
    guchar  prenom_demain[256];

    #if DEBUG >= 2
    GMEMO_DEBUG("Check !!! day:%s min:%s",
        (check_day == TRUE)?"yes":"no",
        (check_min == TRUE)?"yes":"no");
    #endif
    
    if (check_day)
    {
        get_date_and_time(&date_of_day);
        
        get_feast(g_date_get_day(&date_of_day.date),
            g_date_get_month(&date_of_day.date)-G_DATE_JANUARY+1,
            g_date_get_year(&date_of_day.date), 0, prenom_aujourdhui);
        
        get_next_feast(g_date_get_day(&date_of_day.date),
            g_date_get_month(&date_of_day.date)-G_DATE_JANUARY+1,
            g_date_get_year(&date_of_day.date), 0, prenom_demain);
        
        gmemo_display_feasts(prenom_aujourdhui, prenom_demain);
        
    }
    if (check_min)
    {
        // TODO: check for memos !
    }
}

/*
 * display_time: displays date and time
 */

void display_time(gboolean force_maj)
{
    guchar      masque[16];
    gint        i;
    gboolean    check_day, check_min;

    get_date_and_time(&date_of_day);
    if (gmemo_display_seconds)
        sprintf(new_time, "%02d:%02d:%02d", date_of_day.hour, date_of_day.min,
            date_of_day.sec);
    else
        sprintf(new_time, "%02d:%02d   ", date_of_day.hour, date_of_day.min);

    if (force_maj)
        strcpy(masque, "********");
    else
    {
        new_time[2] = (old_time[2] == ':')?' ':':';
        if (gmemo_display_seconds)
            new_time[5] = (old_time[5] == ':')?' ':':';
        for (i = 0; i < 8; i++)
            masque[i] = (new_time[i] != old_time[i])?'*':' ';
        masque[8] = '\0';
    }
    if (gmemo_withdrawn)
        gmemo_display_digital(new_time, masque, 3, 17, gmemo_size_digits, gmemo_size_digits, gmemo_size_digits/2+3);
    else
        gmemo_display_digital(new_time, masque, 1, 1, gmemo_size_digits, gmemo_size_digits, gmemo_size_digits/2+4);
    strcpy(old_time, new_time);

    check_day = (g_date_get_day(&date_of_day.date) != last_day)?TRUE:FALSE;
    check_min = (date_of_day.min != last_min)?TRUE:FALSE;
    if (check_day || check_min)
        check_memos(check_day, check_min);
    if (check_day)
        gmemo_display_date();
    last_day = g_date_get_day(&date_of_day.date);
    last_min = date_of_day.min;
    check_alarms(memos);
}

/*
 * update_time: updates date & time
 */

gboolean update_time(gboolean force_maj)
{
    #if DEBUG >= 2
    GMEMO_DEBUG("updating time");
    #endif
    
    if (force_maj)
        gmemo_clear_area();
        
    display_time(force_maj);
    return TRUE;
}
