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

/* gmemo_loadsave.c: load feasts and load/save memos (xml format) */
 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gtk/gtk.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#ifdef _WIN32
    #include <io.h>
#else
    #include <sys/stat.h>
#endif

#include "gmemo.h"
#include "gmemo_loadsave.h"
#include "gmemo_memo.h"


/*
 * load_feasts : read gmemo feasts (xml format)
 */

gboolean load_feasts()
{
    gchar       *home_dir;
    gchar       gmemo_xmlfile[256];
    xmlDocPtr   doc;
    xmlNodePtr  node;
    xmlChar     *date, *type, *name;
    gint        day, month;
    
    if (gmemo_verbose)
        GMEMO_MESSAGE("reading feasts");
    
    home_dir = (gchar *)g_get_home_dir();
    if (home_dir == NULL)
        strcpy(gmemo_xmlfile, GMEMO_FEASTS_FILE);
    else
        sprintf(gmemo_xmlfile, "%s/.gmemo/" GMEMO_FEASTS_FILE, home_dir);
    if (g_file_test(gmemo_xmlfile, G_FILE_TEST_EXISTS) == FALSE)
    {
        GMEMO_WARNING("feasts file not found (%s)... no feast will be displayed !", 
            gmemo_xmlfile);
        return FALSE;
    }
    
    doc = xmlParseFile(gmemo_xmlfile);
    if (doc == NULL)
    {
        GMEMO_WARNING("error parsing feasts file... using default config");
        return FALSE;
    }
    node = xmlDocGetRootElement(doc);
    if (node == NULL)
    {
        GMEMO_WARNING("empty feasts file... using default config");
        return FALSE;
    }
    if (xmlStrcmp(node->name, (const xmlChar *)"feasts"))
    {
        GMEMO_WARNING("wrong type for xml configuration... using default config");
        return FALSE;
    }
    node = node->xmlChildrenNode;
    while (node)
    {
        if (!xmlStrcmp(node->name, (const xmlChar *)"feast"))
        {
            date = xmlGetProp(node, "date");
            type = xmlGetProp(node, "type");
            name = xmlGetProp(node, "name");
            #if DEBUG >= 2
            //GMEMO_DEBUG("item read: date=%s type=%s name=%s", date, type, name);
            #endif
            sscanf(date, "%d/%d", &month, &day);
            feast_type[month-1][day-1] = type[0];
            strcpy(feast[month-1][day-1], name);
        }
        node = node->next;
    }
    if (gmemo_verbose)
        GMEMO_MESSAGE("feasts loaded");
    return TRUE;
}

/*
 * load_memos_file : load memos from xml file
 */

gboolean load_memos_file(gchar *filename)
{
    gchar           gmemo_xmlfile[1024];
    xmlDocPtr       doc;
    xmlNodePtr      node, node_memo;
    xmlChar         *title, *message, *alarm_date, *alarm_time, *last_alarm_date, *last_alarm_time;
    xmlChar         *repeat_type, *interval, *repeat_count;
    xmlChar         *sound_file, *sound_repeat, *sound_delay, *exec_cmd;
    gint            year, month, day, hour, min;
    t_memo          *memo;
    gint            i;
    
    if (gmemo_verbose)
        GMEMO_MESSAGE("reading memos file");
    
    sprintf(gmemo_xmlfile, "%s%s", gmemo_home_dir, filename);
    if (g_file_test(gmemo_xmlfile, G_FILE_TEST_EXISTS) == FALSE)
    {
        GMEMO_MESSAGE("memos file not found (%s)...", gmemo_xmlfile);
        return FALSE;
    }
    
    doc = xmlParseFile(gmemo_xmlfile);
    if (doc == NULL)
    {
        GMEMO_WARNING("error parsing xml file (%s)... no memo loaded", gmemo_xmlfile);
        return FALSE;
    }
    node = xmlDocGetRootElement(doc);
    if (node == NULL)
    {
        GMEMO_WARNING("empty xml file (%s)... no memo loaded", gmemo_xmlfile);
        return FALSE;
    }
    if (xmlStrcmp(node->name, (const xmlChar *)"gmemo"))
    {
        GMEMO_WARNING("wrong xml file (%s)... no memo loaded", gmemo_xmlfile);
        return FALSE;
    }
    node = node->xmlChildrenNode;
    while (node != NULL)
    {
        if (!xmlStrcmp(node->name, (const xmlChar *)"memo"))
        {
            title = NULL;
            message = NULL;
            alarm_date = NULL;
            alarm_time = NULL;
            last_alarm_date = NULL;
            last_alarm_time = NULL;
            repeat_type = NULL;
            interval = NULL;
            repeat_count = NULL;
            sound_file = NULL;
            sound_repeat = NULL;
            sound_delay = NULL;
            exec_cmd = NULL;
            
            /* get nodes & attributes */
            node_memo = node->xmlChildrenNode;
            while (node_memo != NULL)
            {
                if (!xmlStrcmp(node_memo->name, (const xmlChar *)"title"))
                    title = xmlNodeListGetString(doc, node_memo->xmlChildrenNode, 1);
                if (!xmlStrcmp(node_memo->name, (const xmlChar *)"message"))
                    message = xmlNodeListGetString(doc, node_memo->xmlChildrenNode, 1);
                if (!xmlStrcmp(node_memo->name, (const xmlChar *)"alarm"))
                {
                    alarm_date = xmlGetProp(node_memo, "date");
                    alarm_time = xmlGetProp(node_memo, "time");
                }
                if (!xmlStrcmp(node_memo->name, (const xmlChar *)"last_alarm"))
                {
                    last_alarm_date = xmlGetProp(node_memo, "date");
                    last_alarm_time = xmlGetProp(node_memo, "time");
                }
                if (!xmlStrcmp(node_memo->name, (const xmlChar *)"repeat"))
                {
                    repeat_type = xmlGetProp(node_memo, "unit");
                    interval = xmlGetProp(node_memo, "interval");
                    repeat_count = xmlGetProp(node_memo, "count");
                }
                if (!xmlStrcmp(node_memo->name, (const xmlChar *)"sound"))
                {
                    sound_file = xmlGetProp(node_memo, "file");
                    sound_repeat = xmlGetProp(node_memo, "repeat");
                    sound_delay = xmlGetProp(node_memo, "delay");
                }
                if (!xmlStrcmp(node_memo->name, (const xmlChar *)"exec"))
                {
                    exec_cmd = xmlGetProp(node_memo, "cmd");
                }
                node_memo = node_memo->next;
            }
            
            /* invalid memo ? */
            if ( (title == NULL) || (alarm_date == NULL) )
                GMEMO_WARNING("invalid memo from gmemo file, memo not loaded");
            else
            {
                /* title & text */
                memo = (t_memo *)g_malloc(sizeof(t_memo));
                memo->title = (gchar *)g_malloc(strlen(title)+1);
                strcpy(memo->title, title);
                if (message == NULL)
                    memo->message = NULL;
                else
                {
                    memo->message = (gchar *)g_malloc(strlen(message)+1);
                    strcpy(memo->message, message);
                }
                
                /* alarm */
                sscanf(alarm_date, "%d/%d/%d", &year, &month, &day);
                if (alarm_time != NULL)
                    sscanf(alarm_time, "%d:%d", &hour, &min);
                else
                {
                    hour = 0;
                    min = 0;
                }
                memo->alarm = date_to_glong(year, month, day, hour, min, 0);
                
                /* last alarm */
                if (last_alarm_date == NULL)
                    memo->last_alarm = 0;
                else
                {
                    sscanf(last_alarm_date, "%d/%d/%d", &year, &month, &day);
                    if (last_alarm_time != NULL)
                        sscanf(last_alarm_time, "%d:%d", &hour, &min);
                    else
                    {
                        hour = 0;
                        min = 0;
                    }
                    memo->last_alarm = date_to_glong(year, month, day, hour, min, 0);
                }
                
                /* repeat */
                memo->repeat_type = REPEAT_NONE;
                if (repeat_type != NULL)
                {
                    memo->repeat_type = -1;
                    for (i = 0; i < REPEAT_COUNT; i++)
                        if (strcasecmp(repeat_type, repeat_strings[i]) == 0)
                            memo->repeat_type = i;
                    if (memo->repeat_type == -1)
                    {
                        GMEMO_WARNING("unknown repeat unit time, repeat discarded for memo '%s'", memo->title);
                        memo->repeat_type = REPEAT_NONE;
                    }
                }
                if (memo->repeat_type == REPEAT_NONE)
                {
                    memo->interval = 0;
                    memo->repeat_remaining = 0;
                }
                else
                {
                    if (interval == NULL)
                    {
                        GMEMO_WARNING("no interval specified for memo '%s', repeat discarded", memo->title);
                        memo->repeat_type = REPEAT_NONE;
                        memo->interval = 0;
                        memo->repeat_remaining = 0;
                    }
                    else
                    {
                        memo->interval = (gint)(atoi(interval));
                        if (repeat_count == NULL)
                            memo->repeat_remaining = -1;    // infinite repeats if no count specified
                        else
                            memo->repeat_remaining = (gint)(atoi(repeat_count));
                    }
                }
                
                /* sound */
                if (sound_file == NULL)
                {
                    memo->sound_file = NULL;
                    memo->sound_repeat = 0;
                    memo->sound_delay = 0;
                }
                else
                {
                    memo->sound_file = (gchar *)g_malloc(strlen(sound_file)+1);
                    strcpy(memo->sound_file, sound_file);
                    if (sound_repeat)
                        memo->sound_repeat = (gint)(atoi(sound_repeat));
                    else
                        memo->sound_repeat = 1;
                    if (memo->sound_repeat < 1)
                        memo->sound_repeat = 1;
                    if (sound_delay)
                        memo->sound_delay = (gint)(atoi(sound_delay));
                    else
                        memo->sound_delay = 0;
                    if (memo->sound_delay < 0)
                        memo->sound_delay = 0;
                }
                
                /* command to execute */
                if (exec_cmd == NULL)
                    memo->exec_cmd = NULL;
                else
                {
                    memo->exec_cmd = (gchar *)g_malloc(strlen(exec_cmd)+1);
                    strcpy(memo->exec_cmd, exec_cmd);
                }
                
                /* is memo active ? */
                if ( ( (memo->last_alarm == 0) && (memo->repeat_type == REPEAT_NONE) )
                    || ( (memo->repeat_type != REPEAT_NONE) && (memo->repeat_remaining == 0) ) )
                    memo->active = FALSE;
                else
                    memo->active = TRUE;
                
                /* calculate first alarm for memo */
                set_first_alarm(memo);
                
                /* create the memo */
                memos = g_slist_append(memos, memo);
            }
        }
        node = node->next;
    }
    if (gmemo_verbose)
        GMEMO_MESSAGE("loading memos... done");
    memos_ok = TRUE;
    return TRUE;
}

/*
 * load_memos: check for existing temp memo file and load memo file
 */

gboolean load_memos()
{
    /* TODO: check temp file ! */
    return load_memos_file(GMEMO_MEMOS_FILE);
}

/*
 * save_memos : saves memos to xml file
 */

gboolean save_memos(gchar *filename)
{
    gchar       gmemo_xmlfile[1024];
    xmlDocPtr   doc;
    xmlNodePtr  node, node_memo, newelement, repeat, alarm, last_alarm, sound, exec;
    xmlAttrPtr  newattr;
    GSList      *ptr_memos;
    t_memo      *memo;
    struct tm   *ptr_tm;
    gchar       string[256];
    
    if (gmemo_verbose)
        GMEMO_MESSAGE("writing memos file");
    
    sprintf(gmemo_xmlfile, "%s%s", gmemo_home_dir, filename);
        
    doc = xmlNewDoc("1.0");
    doc->children = xmlNewDocNode(doc, NULL, "gmemo", NULL);

    node = xmlDocGetRootElement(doc);
    
    ptr_memos = memos;
    while (ptr_memos != NULL)
    {
        memo = (t_memo *)(ptr_memos->data);
        node_memo = xmlNewTextChild(node, NULL, "memo", NULL);
        
        /* title / description */
        newelement = xmlNewTextChild(node_memo, NULL, "title", memo->title);
        if (memo->message)
            newelement = xmlNewTextChild(node_memo, NULL, "message", memo->message);
        
        /* alarm */
        if (memo->alarm > 0)
        {
            alarm = xmlNewTextChild(node_memo, NULL, "alarm", NULL);
            ptr_tm = localtime(&(memo->alarm));
            sprintf(string, "%04d/%02d/%02d", ptr_tm->tm_year+1900, ptr_tm->tm_mon+1, ptr_tm->tm_mday);
            newattr = xmlNewProp(alarm, "date", string);
            sprintf(string, "%02d:%02d", ptr_tm->tm_hour, ptr_tm->tm_min);
            newattr = xmlNewProp(alarm, "time", string);
        }
        
        /* last alarm */
        if (memo->last_alarm > 0)
        {
            last_alarm = xmlNewTextChild(node_memo, NULL, "last_alarm", NULL);
            ptr_tm = localtime(&(memo->last_alarm));
            sprintf(string, "%04d/%02d/%02d", ptr_tm->tm_year+1900, ptr_tm->tm_mon+1, ptr_tm->tm_mday);
            newattr = xmlNewProp(last_alarm, "date", string);
            sprintf(string, "%02d:%02d", ptr_tm->tm_hour, ptr_tm->tm_min);
            newattr = xmlNewProp(last_alarm, "time", string);
        }
        
        /* repeat */
        if (memo->repeat_type != REPEAT_NONE)
        {
            repeat = xmlNewTextChild(node_memo, NULL, "repeat", NULL);
            newattr = xmlNewProp(repeat, "unit", repeat_strings[memo->repeat_type]);
            sprintf(string, "%d", memo->interval);
            newattr = xmlNewProp(repeat, "interval", string);
            if (memo->repeat_remaining >= 0)
            {
                sprintf(string, "%d", memo->repeat_remaining);
                newattr = xmlNewProp(repeat, "count", string);
            }
        }
        
        /* sound file */
        if (memo->sound_file)
        {
            sound = xmlNewTextChild(node_memo, NULL, "sound", NULL);
            newattr = xmlNewProp(sound, "file", memo->sound_file);
            if (memo->sound_repeat > 1)
            {
                sprintf(string, "%d", memo->sound_repeat);
                newattr = xmlNewProp(sound, "repeat", string);
                sprintf(string, "%d", memo->sound_delay);
                newattr = xmlNewProp(sound, "delay", string);
            }
        }
        
        /* command to execute */
        if (memo->exec_cmd)
        {
            exec = xmlNewTextChild(node_memo, NULL, "exec", NULL);
            newattr = xmlNewProp(exec, "cmd", memo->exec_cmd);
        }
        
        ptr_memos = ptr_memos->next;
    }
    
    return (xmlSaveFile(gmemo_xmlfile, doc) > 0);
}

/*
 * remove_temp_memos: remove temporary memos file
 */

void remove_temp_memos()
{
    /* TODO: code this function */
}
