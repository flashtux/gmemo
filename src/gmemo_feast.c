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

/* gmemo_feast.c: feasts management */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <libintl.h>

#include "gmemo.h"
#include "gmemo_feast.h"
#include "gmemo_memo.h"


/*
 * get_feast: extracts a feast for a given date
 *            day      : 1..31
 *            month    : 1..12
 *            num_feast: 0 = if two feasts, extracts both feasts
 *                       1 = only the first feast
 *                       2 = only second feast (only if 2 feasts this day)
 */

void get_feast(gint day, gint month, gint year, gint num_feast,
               guchar *string)
{
    guchar  buffer[128], buffer2[128], *pos, *ptr_feast;

    string[0] = '\0';
    
    if (day > get_num_days_month(month, year))
        return;
    
    strcpy(buffer, feast[month-1][day-1]);
    pos = (guchar *)strchr(buffer, '+');
    if (pos != NULL)
    {
        strcpy(buffer2, pos+1);
        pos[0] = '\0';
    }
    else
        buffer2[0] = '\0';
    
    ptr_feast = (num_feast == 2) ? buffer2 : buffer;
    if (strlen(ptr_feast) == 0)
        return;
    
    string[0] = '\0';
    
    switch (feast_type[month-1][day-1])
    {
        case 'F':
            sprintf(string, "%s %s", _("Ste"), ptr_feast);
            break;
        case 'M':
            sprintf(string, "%s %s", _("St"), ptr_feast);
            break;
        default:
            strcpy(string, buffer);
            break;
    }
    if ( (num_feast == 0) && (strlen(buffer2) > 0) )
    {
        strcat(string, " ");
        strcat(string, (char *)_("and"));
        strcat(string, " ");
        strcat(string, buffer2);
    }
}

/*
 * get_net_feast: extracts the next feast for a given date (date + 1 day)
 *                parameters: same as get_feast()
 */

void get_next_feast(gint day, gint month, gint year, gint num_feast,
                    guchar *string)
{
    if ( (day >= get_num_days_month(month, year)) || (strlen(feast[month-1][day-1]) == 0) )
    {
        day = 1;
        month = (month == 12) ? 1 : month+1;
        if (month == 1)
            year++;
    }
    else
        day++;
    
    get_feast(day, month, year, num_feast, string);
}

/* ========================================================================= */

/*
 * get_feast_only: returns feast (first or 2nd) for a given date
 *                 num_feast = 1 or 2
 */

void get_feast_only(gint day, gint month, gint year, gint num_feast,
                    guchar *string)
{
    guchar  res[128], *pos;

    if (num_feast == 2)
    {
        pos = (guchar *)strchr(feast[month-1][day-1], '&');
        if (pos == NULL)
        {
            string[0] = '\0';
            return;
        }
        strcpy(res, pos+1);
    }
    else
        strcpy(res, feast[month-1][day-1]);
    if ( (strlen(res) > 0) && (day <= get_num_days_month(month, year)) )
    {
        if ( (res[0] == '!') || (res[0] == '-') )
            strcpy(res, &res[1]);
    }
    strcpy(string, res);
}

/*
 * get_feast_all: returns all feasts for a given date
 */

/*void get_feast_all(gint day, gint month, gint year,
                   guchar *feast1, guchar *feast2)
{
    guchar  res[50], *pos;

    if ( (feast_type[month-1][day-1][0] == '!') || (prenom[mois][jour-1][0] == '!') )
        strcpy(prenom1, &prenom[mois][jour-1][1]);
    else
        strcpy(prenom1, prenom[mois][jour-1]);
    pos = (guchar *)strchr(prenom1, '&');
    if (pos == NULL)
        prenom2[0] = '\0';
    else
    {
        strcpy(prenom2, pos+1);
        pos[0] = '\0';
    }
}*/
