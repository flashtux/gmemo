#
# Copyright (c) 2003 FlashCode <flashcode@flashtux.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#


bin_PROGRAMS = gmemo gmemo_gnome

gmemo_SOURCES = \
    gmemo.c \
    gmemo_main.c \
    gmemo_feast.c \
    gmemo_memo.c \
    gmemo_loadsave.c \
    gmemo_display.c \
    gmemo.h \
    gmemo_feast.h \
    gmemo_memo.h \
    gmemo_loadsave.h \
    gmemo_display.h \
    cle_petite.xpm \
    gmemo_feasts.xml \
    gmemo.xml \
    Makefile.gnome \
    Makefile.win32 \
    m.bat

gmemo_LDADD = -lxml2 -lesd

gmemo_gnome_SOURCES = \
    gmemo_gnome_applet.c \
    gmemo_gnome_applet.h \
    gmemo.c \
    gmemo_feast.c \
    gmemo_memo.c \
    gmemo_loadsave.c \
    gmemo_display.c \
    gmemo.h \
    gmemo_feast.h \
    gmemo_memo.h \
    gmemo_loadsave.h \
    gmemo_display.h \
    GNOME_GmemoApplet.server \
    GNOME_GmemoApplet.xml

gmemo_gnome_LDADD = -lxml2 -lesd -lpanel-applet-2

LIBS = @LIBS@ $(GTK_LIBS)

INCLUDES = -I/usr/include/panel-2.0 -I/usr/include/libgnomeui-2.0 \
    -I/usr/include/libbonoboui-2.0 -I/usr/include/libbonobo-2.0 -I/usr/include/orbit-2.0 \
    -I/usr/include/linc-1.0 -I/usr/include/bonobo-activation-2.0 -I/usr/include/libgnome-2.0 \
    -I/usr/include/libxml2
