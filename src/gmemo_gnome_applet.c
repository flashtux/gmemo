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

/* gmemo_gnome_applet.c: creation of Gnome applet (specific for Gnome panel) */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <panel-applet.h>
#include <gtk/gtk.h>

#include "gmemo.h"
#include "gmemo_gnome_applet.h"
#include "gmemo_loadsave.h"
#include "gmemo_display.h"
#include "gmemo_memo.h"


#define GETTEXT_PACKAGE NULL


void gmemo_create_window_gnome()
{
    frame = gtk_frame_new(NULL);
    gtk_container_add(GTK_CONTAINER(window), frame);
    gtk_frame_set_shadow_type(GTK_FRAME (frame), GTK_SHADOW_OUT);
    gtk_widget_set_usize(frame, gmemo_win_width, gmemo_win_height);
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(frame), vbox);

    gmemo_init_window_gtk();
}

void gmemo_edition(BonoboUIComponent *uic,
                   gpointer           data,
                   const gchar       *verbname)
{
    // TODO: memos management
}

void gmemo_check(BonoboUIComponent *uic,
                 gpointer           data,
                 const gchar       *verbname)
{
    // TOTO: ???
}

void gmemo_help(BonoboUIComponent *uic,
                gpointer           data,
                const gchar       *verbname)
{
    // TODO: help
}

void gmemo_about(BonoboUIComponent *uic,
                 gpointer           data,
                 const gchar       *verbname)
{
    // TODO: about "gmemo"
}

const BonoboUIVerb gmemo_applet_menu_verbs [] = {
    BONOBO_UI_VERB ("Edition", gmemo_edition),
    BONOBO_UI_VERB ("Check",   gmemo_check),
    BONOBO_UI_VERB ("Help",    gmemo_help),
    BONOBO_UI_VERB ("About",   gmemo_about),

    BONOBO_UI_VERB_END
};

/*
<Root>
  <popups>
    <popup name="mon_bouton">
      <menuitem name="Edition memos" verb="Edition" _label="_Edition memos..."/>
      <menuitem name="Check!!!"      verb="Check"   _label="_Check"     type="toggle"/>
      <menuitem name="Aide"          verb="Help"    _label="_Help"     pixtype="stock" pixname="gtk-help"/>
      <menuitem name="A propos"      verb="About"   _label="_A propos..." pixtype="stock" pixname="gnome-stock-about"/>
    </popup>
  </popups>
</Root>
*/

static const char gmemo_menu_xml[] =
"<Root>\n"
"  <popups>\n"
"    <popup name=\"mon_bouton\">\n"
"      <menuitem name=\"Edition memos\" verb=\"Edition\" _label=\"_Edition memos...\" pixtype=\"stock\" pixname=\"gtk-properties\"/>\n"
"      <menuitem name=\"Check!!!\" verb=\"Check\" _label=\"_Check\" pixtype=\"stock\" pixname=\"gtk-properties\"/>\n"
"      <menuitem name=\"Aide\" verb=\"Help\" _label=\"_Help\" pixtype=\"stock\" pixname=\"gtk-properties\"/>\n"
"      <menuitem name=\"A propos\" verb=\"About\" _label=\"_A propos...\" pixtype=\"stock\" pixname=\"gtk-properties\"/>\n"
"    </popup>\n"
"  </popups>\n"
"</Root>\n";

static gboolean
gmemo_applet_new(PanelApplet *applet)
{
    int     i = 123;
    
    gmemo_init();
    gmemo_win_width = 220;
    gmemo_win_height = 48;
    load_feasts();
    memos = NULL;
    load_memos();
    panel_applet_setup_menu(applet, gmemo_menu_xml, gmemo_applet_menu_verbs, NULL);
    window = GTK_WIDGET(applet);
    gmemo_create_window_gnome();
    /*panel_applet_setup_menu_from_file (applet,
                       NULL,
                       "GNOME_GmemoApplet.xml",
                       NULL,
                       gmemo_applet_menu_verbs,
                       NULL);*/
    gtk_widget_show_all(GTK_WIDGET (applet));
    return TRUE;
}

static gboolean
gmemo_applet_factory(PanelApplet *applet,
                     const gchar          *iid,
                     gpointer              data)
{
    gmemo_applet_new(applet);
    return TRUE;
}

PANEL_APPLET_BONOBO_FACTORY ("OAFIID:GNOME_GmemoApplet_Factory",
                             PANEL_TYPE_APPLET,
                             "gmemo",
                             "0",
                             (PanelAppletFactoryCallback)gmemo_applet_factory,
                             NULL)
