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


#ifndef _GMEMO_LOADSAVE_H
#define _GMEMO_LOADSAVE_H   1


#define GMEMO_CONFIG_FILE   "gmemo_config.xml"
#define GMEMO_MEMOS_FILE    "gmemo.xml"
#define GMEMO_MEMOS_TEMP    "gmemo_temp.xml"
#define GMEMO_MEMOS_NEW     "gmemo_new.xml"
#define GMEMO_FEASTS_FILE   "gmemo_feasts.xml"


extern gboolean load_feasts();
extern gboolean load_memos_file(gchar *memo_file);
extern gboolean load_memos();
extern gboolean save_memos(gchar *memo_file);
extern void remove_temp_memos();

#endif /* gmemo_loadsave.h */
