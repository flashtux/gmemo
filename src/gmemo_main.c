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

/* gmemo_main.c: main program (except for Gnome version: gmemo_gnome_applet.c) */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _WIN32
    #include <windows.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "gmemo.h"
#include "gmemo_loadsave.h"
#include "gmemo_memo.h"
#include "gmemo_display.h"


#ifdef _WIN32
int     argc = 0;
char    *argv[128];
#endif


/*
 * explode_args_win32: explode args (win32 only)
 */

#ifdef _WIN32
void explode_args_win32(char *command_line)
{
    char    *pos;
    
    argc = 1;
    argv[0] = (char *)malloc(1);
    argv[0][0] = '\0';
    while (command_line != NULL)
    {
        if (command_line[0] == '\0')
            return;
        pos = strchr(command_line, ' ');
        if (pos == NULL)
        {
            argv[argc] = (char *)malloc(strlen(command_line)+1);
            strcpy(argv[argc], command_line);
            argc++;
            command_line = NULL;
        }
        else
        {
            pos[0] = '\0';
            argv[argc] = (char *)malloc(strlen(command_line)+1);
            strcpy(argv[argc], command_line);
            argc++;
            command_line = pos+1;
            while (command_line[0] == ' ')
                command_line++;
        }
    }
}
#endif

/*
 * gmemo_parse_args: parse command line args
 */

void gmemo_parse_args(int argc, char *argv[])
{
    int     i;
    
    for (i = 1; i < argc; i++)
    {
        if ( (strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "--daemon") == 0) )
        {
            gmemo_daemon = TRUE;
        }
        else if ( (strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0) )
        {
            gmemo_display_usage();
            gmemo_exit(0);
        }
        else if ( (strcmp(argv[i], "-l") == 0) || (strcmp(argv[i], "--licence") == 0) )
        {
            gmemo_display_licence();
            gmemo_exit(0);
        }
        else if ( (strcmp(argv[i], "-q") == 0) || (strcmp(argv[i], "--quiet") == 0) )
        {
            gmemo_quiet = TRUE;
        }
        else if ( (strcmp(argv[i], "-s") == 0) || (strcmp(argv[i], "--seconds") == 0) )
        {
            gmemo_display_seconds = TRUE;
        }
        else if ( (strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--verbose") == 0) )
        {
            gmemo_verbose = TRUE;
        }
        else if ( (strcmp(argv[i], "-w") == 0) || (strcmp(argv[i], "--withdrawn") == 0) )
        {
            gmemo_withdrawn = TRUE;
        }
        else
        {
            GMEMO_WARNING("parsing command line: unknown parameter '%s', ignored", argv[i]);
        }
    }
    
    if (gmemo_daemon && gmemo_withdrawn)
    {
        GMEMO_ERROR("daemon and withdrawn are incompatible options");
        gmemo_exit(1);
    }
    
    if (gmemo_win_width == -1)
        gmemo_win_width = (gmemo_withdrawn) ? 64 : 220;
    if (gmemo_win_height == -1)
        gmemo_win_height = (gmemo_withdrawn) ? 64 : 48;
    
    if (gmemo_verbose)
    {
        GMEMO_MESSAGE("finished parsing command line. Results below :");
        GMEMO_MESSAGE("application type is %s", (gmemo_daemon) ? "daemon" : ((gmemo_withdrawn) ? "withdrawn" : "gtk"));
        GMEMO_MESSAGE("using display '%s'", gmemo_display);
        GMEMO_MESSAGE("window width  = %d", gmemo_win_width);
        GMEMO_MESSAGE("window height = %d", gmemo_win_height);
        GMEMO_MESSAGE("digits size   = %d\n", gmemo_size_digits);
    }
    
    if (gmemo_withdrawn)
    {
        GMEMO_ERROR("window maker stuff not developed !");
        gmemo_exit(1);
    }
}

/*
 * main: main function for gmemo (except for Gnome applet)
 */

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
#else
int main (int argc, char *argv[])
#endif
{
    // gtk2.0: gdk_draw_layout () et pango_layout_new pour faire le layout...

    // Initialize the i18n stuff
    //bindtextdomain (PACKAGE, GNOMELOCALEDIR);
    //textdomain (PACKAGE);
    
    #ifdef _WIN32
    explode_args_win32(szCmdLine);
    gtk_init(NULL, NULL);
    #else
    gtk_init(&argc, &argv);
    #endif
    
    gmemo_init();
    gmemo_parse_args(argc, argv);
    
    load_feasts();
    memos = NULL;
    load_memos();
    /*if (!save_memos())
        GMEMO_WARNING("error saving memos file");*/
    
    #ifndef WIN32
    signal(SIGUSR1, signal_usr1);
    #endif
    
    gmemo_create_window(argc, argv);
    
    gtk_main();
    
    return 0;
}
