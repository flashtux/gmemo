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

/* gmemo_display.c: display functions for gmemo (gtk/gnome/wmaker) */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#ifndef WIN32
    #include <gdk/gdkx.h>
#endif

#include "gmemo.h"
#include "gmemo_display.h"

#include "cle_petite.xpm"


GtkWidget       *window, *icon_win, *frame;
GtkWidget       *hbox;
GtkWidget       *vbox;
GtkWidget       *drawing_area, *drawing_config;
GtkWidget       *eventbox_heure, *eventbox_config, *eventbox_date;
GtkWidget       *eventbox_fete, *eventbox_memo;
GtkWidget       *label_date, *label_fete, *label_memo;
GdkPixmap       *gdkpixmap;
GdkBitmap       *gdkbitmap = NULL;
GdkGC           *gc;
GdkColor        color_bg_window;
GdkColor        color_fg_digits = { 0, 0x0000, 0x0000, 0xAAAA };
GdkColor        color_fg_date   = { 0, 0x0000, 0x0000, 0x6666 };
GdkColor        color_fg_fete   = { 0, 0x0000, 0x6666, 0x5151 };
GdkColor        color_fg_memo   = { 0, 0xAAAA, 0x5555, 0xAAAA };
GdkColor        color_bg_alert  = { 0, 0xFFFF, 0x7777, 0x7777 };

/*
 * gmemo_create_window: creates window for gtk standalone program
 */

void gmemo_create_window(int argc, char *argv[])
{
    #ifndef WIN32
    XWMHints    hints;
    XSizeHints  sizehints;
    #endif
    
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize(window, gmemo_win_width, gmemo_win_height);
    gtk_window_set_title(GTK_WINDOW(window), "gmemo");
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_widget_set_app_paintable(window, TRUE);
    gtk_widget_realize(window);
    
    #ifndef WIN32
    if (gmemo_withdrawn)
    {
        icon_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_widget_set_app_paintable(icon_win, TRUE);
        gtk_widget_set_uposition(icon_win, 0, 0);
        gtk_widget_set_usize(icon_win, gmemo_win_width, gmemo_win_height);
        gtk_widget_set_events(icon_win, GDK_BUTTON_PRESS_MASK |
			GDK_EXPOSURE_MASK);
        
        sizehints.flags = USSize;
        sizehints.width = gmemo_win_width;
        sizehints.height = gmemo_win_height;
        XSetWMNormalHints(GDK_WINDOW_XDISPLAY(window->window), GDK_WINDOW_XWINDOW(window->window), &sizehints);
        
        hints.initial_state = WithdrawnState;
        hints.icon_window = GDK_WINDOW_XWINDOW(icon_win->window);
        hints.window_group = GDK_WINDOW_XWINDOW(window->window);
        hints.flags = StateHint | IconWindowHint | IconPositionHint | WindowGroupHint;
        XSetWMHints(GDK_WINDOW_XDISPLAY(window->window), GDK_WINDOW_XWINDOW(window->window), &hints);
    }
    #endif
    
    g_signal_connect (G_OBJECT(window), "destroy",  gtk_main_quit, NULL);
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    if (gmemo_withdrawn)
        /* gmemo_init_window_withdrawn(); */
        /* !!!!!!!!!!!! TODO: write this function !!!!!!!!!!!! */
        gmemo_init_window_gtk();
    else
        gmemo_init_window_gtk();
}

/*
 * gmemo_gtk_expose_event : called by gtk
 */

gboolean gmemo_gtk_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    display_time(TRUE);
    if (mouse_over_config)
    {
        gdk_window_clear_area (drawing_config->window, 0, 0, 34, 30);
        gdk_draw_drawable(drawing_config->window, gc, gdkpixmap, 0, 0, 7, 0, 17, 16);
    }
    return TRUE;
}

void click_time()
{
    #if DEBUG >= 1
    GMEMO_DEBUG("clic heure !");
    #endif
    gmemo_display_seconds ^= TRUE;
    gdk_window_clear_area(drawing_area->window, 0, 0, gmemo_win_width, gmemo_win_height);
    strcpy(old_time, "########");
    display_time(TRUE);
}

void click_line1()
{
    #if DEBUG >= 1
    GMEMO_DEBUG("clic fete !");
    #endif
    //gmemo_message("Saving xml file...");
    //write_xml_config();
}

void click_line2()
{
    #if DEBUG >= 1
    GMEMO_DEBUG("clic memo !");
    #endif
}

void mouse_enter_config()
{
    mouse_over_config = 1;
    gdk_window_clear_area (drawing_config->window, 0, 0, 34, 30);
    gdk_draw_drawable(drawing_config->window, gc, gdkpixmap, 0, 0, 7, 0, 17, 16);
}

void mouse_leave_config()
{
    mouse_over_config = 0;
    gdk_window_clear_area (drawing_config->window, 0, 0, 34, 30);
}

/*
 * gmemo_continue_create_window_gtk : continue to create window for gtk / gnome applet
 */

void gmemo_init_window_gtk()
{
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(vbox), hbox);
    gtk_widget_set_usize(hbox, gmemo_win_width, gmemo_win_height/2);

    /* hour */
    if ( (eventbox_heure = gtk_event_box_new()) == NULL)
        GMEMO_WARNING("error creating event box for time");
    if ( (drawing_area = gtk_drawing_area_new()) == NULL)
        GMEMO_WARNING("error creating drawing area");
    gtk_container_add(GTK_CONTAINER(eventbox_heure), drawing_area);
    gtk_box_pack_start(GTK_BOX(hbox), eventbox_heure, TRUE, TRUE, 0);
    gtk_drawing_area_size(GTK_DRAWING_AREA(drawing_area), gmemo_win_width, gmemo_win_height);
    gtk_signal_connect(GTK_OBJECT(drawing_area), "expose_event",
        GTK_SIGNAL_FUNC(gmemo_gtk_expose_event), NULL);
    gtk_signal_connect(GTK_OBJECT(eventbox_heure), "button_press_event",
        GTK_SIGNAL_FUNC(click_time), NULL);
    gtk_widget_set_usize(window, gmemo_win_width, gmemo_win_height);
    
    /* config button */
    eventbox_config = gtk_event_box_new();
    drawing_config = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(eventbox_config), drawing_config);
    gtk_box_pack_start(GTK_BOX(hbox), eventbox_config, FALSE, FALSE, 0);
    gtk_drawing_area_size(GTK_DRAWING_AREA(drawing_config), 34, 16);
    mouse_over_config = 0;
    gtk_signal_connect(GTK_OBJECT(eventbox_config), "enter_notify_event",
        GTK_SIGNAL_FUNC(mouse_enter_config), NULL);
    gtk_signal_connect(GTK_OBJECT(eventbox_config), "leave_notify_event",
        GTK_SIGNAL_FUNC(mouse_leave_config), NULL);
    /*gtk_signal_connect(GTK_OBJECT(eventbox_config), "button_press_event",
        GTK_SIGNAL_FUNC(gmemo_config), NULL);*/
    
    /* date */
    eventbox_date = gtk_event_box_new();
    label_date = gtk_label_new("");
    gtk_container_add(GTK_CONTAINER(eventbox_date), label_date);
    gtk_box_pack_start(GTK_BOX(hbox), eventbox_date, FALSE, FALSE, 0);

    /* feast (1st line) */
    eventbox_fete = gtk_event_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), eventbox_fete, FALSE, FALSE, 0);
    label_fete = gtk_label_new("");
    gtk_container_add(GTK_CONTAINER(eventbox_fete), label_fete);
    gtk_widget_set_usize(label_fete, gmemo_win_width, 12);
    gtk_signal_connect(GTK_OBJECT(eventbox_fete), "button_press_event",
        GTK_SIGNAL_FUNC(click_line1), NULL);

    /* memo (2nd line) */
    eventbox_memo = gtk_event_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), eventbox_memo, FALSE, FALSE, 0);
    label_memo = gtk_label_new("");
    gtk_container_add(GTK_CONTAINER(eventbox_memo), label_memo);
    gtk_widget_set_usize(label_memo, gmemo_win_width, 12);
    gtk_signal_connect(GTK_OBJECT(eventbox_memo), "button_press_event",
        GTK_SIGNAL_FUNC(click_line2), NULL);
    
    gtk_widget_show_all(window);
    
    /* allocation couleurs pour le drawing_area */
    memcpy(&color_bg_window, &window->style->bg[GTK_STATE_NORMAL],
        sizeof(GdkColor));
    if (!gdk_colormap_alloc_color(drawing_area->style->colormap, &color_fg_digits, FALSE, TRUE) )
        GMEMO_WARNING("unable to allocate color");
    if (!gdk_colormap_alloc_color(drawing_area->style->colormap, &color_fg_date, FALSE, TRUE) )
        GMEMO_WARNING("unable to allocate color");
    if (!gdk_colormap_alloc_color(label_fete->style->colormap, &color_fg_fete, FALSE, TRUE) )
        GMEMO_WARNING("unable to allocate color");
    if (!gdk_colormap_alloc_color(label_memo->style->colormap, &color_fg_memo, FALSE, TRUE) )
        GMEMO_WARNING("unable to allocate color");
    if (!gdk_colormap_alloc_color(label_memo->style->colormap, &color_bg_alert, FALSE, TRUE) )
        GMEMO_WARNING("unable to allocate color");
    
    if ( (gc = gdk_gc_new(drawing_area->window)) == NULL)
        GMEMO_WARNING("unable to create GC");
    

    //gdkpixmap = gdk_pixmap_create_from_xpm_d(drawing_area->window, &gdkbitmap,
    //    &drawing_area->style->bg[GTK_STATE_NORMAL], essai_xpm);
    //window->style->bg_pixmap[window->state] = gdkpixmap;
    
    gdkpixmap = gdk_pixmap_create_from_xpm_d(drawing_config->window, &gdkbitmap,
        &drawing_config->style->bg[GTK_STATE_NORMAL], cle_petite_xpm);
    if (gdkpixmap == NULL)
        GMEMO_WARNING("error creating gdkpixmap for small key");
    //drawing_config->style->bg_pixmap[drawing_config->state] = gdkpixmap;
    gdk_draw_drawable(drawing_config->window, gc, gdkpixmap, 0, 0, 0, 0, 17, 16);

    // force display of time for the first time
    strcpy(old_time, "########");
    last_day = -1;
    last_min = -1;
    update_time(TRUE);
    gtk_timeout_add(1000, (GtkFunction)(update_time), FALSE);

    /*inverse = 0;
    gtk_timeout_add(200, (GtkFunction)(gmemo_invert_memo), NULL);*/
    
    
  //  leader=gdk_window_foreign_new(gdk_leader_window);
  //gdk_window_set_icon(leader, icon_win->window, NULL, NULL);
  //gdk_window_set_icon(GDK_WINDOW(window), icon_win->window, NULL, NULL);
  //gdk_window_reparent(icon_win->window, window, 0, 0);
    /*unreference stuff and show the windows*/
  //gdk_gc_unref(mask_gc);
  //gdk_window_unref(window); 
  //gtk_widget_show(icon_win);
  //gtk_widget_show(window);
}

/*
 * gmemo_display_feasts : display today's and tomorrow's feasts
 */

void gmemo_display_feasts(guchar *prenom_aujourdhui, guchar *prenom_demain)
{
    guchar  prenom_aujourdhui2[256], prenom_demain2[256];
    
    #if DEBUG >= 2
    GMEMO_DEBUG("displaying feasts");
    #endif
    
    sprintf(prenom_aujourdhui2, 
            "<span font_desc=\"8\" foreground=\"#006651\"><b>%s</b></span>", 
            prenom_aujourdhui);
    
    sprintf(prenom_demain2,
            "<span font_desc=\"7\" foreground=\"#9A459A\"><b>Demain: %s</b></span>", 
            prenom_demain);
    
    gtk_label_set_text(GTK_LABEL(label_fete), prenom_aujourdhui2);
    gtk_label_set_use_markup(GTK_LABEL(label_fete), TRUE);
                
    gtk_label_set_text(GTK_LABEL(label_memo), prenom_demain2);
    gtk_label_set_use_markup(GTK_LABEL(label_memo), TRUE);
}

/*
 * gmemo_display_digital : display hour with digital digits
 */

void gmemo_display_digital(guchar *chaine, guchar *masque,
                           gint x, gint y, gint size_x, gint size_y, gint espace_x)
{
    gint        i, j, k, m, x_deb_barre, y_deb_barre, inc;
    GdkColor    *couleur_barre, *couleur_antialias;

    #if DEBUG >= 2
    GMEMO_DEBUG("displaying digital digits");
    #endif
    
    i = 0;
    while (chaine[i])
    {
        if ( (chaine[i] == ':') || (chaine[i] == ' ') )
        {
            if (masque[i] != ' ')
            {
                if (chaine[i] == ':')
                    couleur_barre = &color_fg_digits;
                else
                    couleur_barre = &drawing_area->style->bg[drawing_area->state];
                x_deb_barre = x+size_x/4-1;
                y_deb_barre = y+size_x;
                gdk_gc_set_foreground(gc, couleur_barre);
                gdk_draw_line(drawing_area->window, gc,
                    x_deb_barre, y_deb_barre,
                    x_deb_barre+2, y_deb_barre);
                gdk_draw_line(drawing_area->window, gc,
                    x_deb_barre, y_deb_barre+1,
                    x_deb_barre+2, y_deb_barre+1);
                gdk_draw_line(drawing_area->window, gc,
                    x_deb_barre, y_deb_barre+2,
                    x_deb_barre+2, y_deb_barre+2);
                gdk_draw_line(drawing_area->window, gc,
                    x_deb_barre, y_deb_barre+size_y,
                    x_deb_barre+2, y_deb_barre+size_y);
                gdk_draw_line(drawing_area->window, gc,
                    x_deb_barre, y_deb_barre+size_y+1,
                    x_deb_barre+2, y_deb_barre+size_y+1);
                gdk_draw_line(drawing_area->window, gc,
                    x_deb_barre, y_deb_barre+size_y+2,
                    x_deb_barre+2, y_deb_barre+size_y+2);
            }
            x += size_x/4+espace_x-2;
        }
        else
        {
            if (masque[i] != ' ')
            {
                j = chaine[i]-'0';
                if ( (j >= 0) && (j <= 9) )
                {
                    for (k = 0; k < 7; k++)
                    {
                        x_deb_barre = x+((size_x+1)*(gint)(coord_barre[k][0]));
                        y_deb_barre = y+((size_y+1)*(gint)(coord_barre[k][1]));
                        if (k < 3)
                            x_deb_barre += 2;
                        else
                            y_deb_barre += 1;
                        if (k == 1) y_deb_barre--;
                        if (k == 2) y_deb_barre -= 2;
                        if ( (chiffre[j] & (1 << k)) != 0)
                        {
                            couleur_barre = &color_fg_digits;
                            couleur_antialias = NULL;
                        }
                        else
                        {
                            couleur_barre = &drawing_area->style->bg[drawing_area->state];
                            couleur_antialias = &drawing_area->style->bg[drawing_area->state];
                        }
                        if (k < 3)
                        {
                            /* barre horizontale */
                            for (m = 0; m < 3; m++)
                            {
                                inc = long_barre[k][m];
                                gdk_gc_set_foreground(gc, couleur_barre);
                                gdk_draw_line(drawing_area->window, gc,
                                    x_deb_barre+inc, y_deb_barre+m,
                                    x_deb_barre+size_x-inc-1, y_deb_barre+m);
                                /*if ((coul_antialias != NULL) && (inc > 0))
                                {
                                    gdk_gc_set_foreground(gc, couleur_antialias);
                                    gdk_draw_point(drawing_area->window, gc,
                                        x_deb_barre+inc-1, y_deb_barre+m);
                                    gdk_draw_point(drawing_area->window, gc,
                                        x_deb_barre+size_x-inc, y_deb_barre+m);
                                }*/
                            }
                        }
                        else
                        {
                            /* barre verticale */
                            for (m = 0; m < 3; m++)
                            {
                                inc = long_barre[k][m];
                                gdk_gc_set_foreground(gc, couleur_barre);
                                gdk_draw_line(drawing_area->window, gc,
                                    x_deb_barre+m, y_deb_barre+inc,
                                    x_deb_barre+m, y_deb_barre+size_x-inc-1);
                                /*if ((coul_antialias != NULL) && (inc > 0))
                                {
                                    gdk_gc_set_foreground(gc, couleur_antialias);
                                    gdk_draw_point(drawing_area->window, gc,
                                        x_deb_barre+m, y_deb_barre+inc-1);
                                    gdk_draw_point(drawing_area->window, gc,
                                        x_deb_barre+m, y_deb_barre+size_y-inc);
                                }*/
                            }
                        }
                    }
                }
            }
            x += size_x+espace_x;
        }
        i++;
    }
}

/*
 * gmemo_display_date : displays current date
 */

void gmemo_display_date()
{
    guchar  str_date[256];
    
    #if DEBUG >= 2
    GMEMO_DEBUG("displaying date");
    #endif
    
    sprintf(str_date, "<span font_desc=\"8\"><b>%s %d %s </b></span>",
        short_day_name[g_date_get_weekday(&date_of_day.date)-G_DATE_MONDAY],
        g_date_get_day(&date_of_day.date),
        short_month_name[g_date_get_month(&date_of_day.date)-G_DATE_JANUARY]);
        
    gtk_label_set_text(GTK_LABEL(label_date), str_date);
    gtk_label_set_use_markup(GTK_LABEL(label_date), TRUE);
}

/*
 * gmemo_clear_area : clear area with date / time
 */

void gmemo_clear_area()
{
    #if DEBUG >= 2
    GMEMO_DEBUG("clearing area");
    #endif

    gdk_window_clear_area (drawing_area->window, 0, 0, 100, 30);
}

/*
 * gmemo_invert_memo : invert memo color
 */

gboolean gmemo_invert_memo()
{
    inverse ^= 1;
    gtk_widget_modify_bg(GTK_WIDGET(eventbox_fete), GTK_STATE_NORMAL,
        (inverse == 1)?&color_bg_alert:&color_bg_window);
    gtk_widget_modify_bg(GTK_WIDGET(eventbox_memo), GTK_STATE_NORMAL,
        (inverse == 1)?&color_bg_window:&color_bg_alert);
    return TRUE;
}
