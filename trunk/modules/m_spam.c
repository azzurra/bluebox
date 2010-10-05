/* modules/m_spam.c
 *   Copyright (C) 2005 Lee Hardy <lee -at- leeh.co.uk>
 *   Copyright (C) 2005 ircd-ratbox development team
 *   Copyright (C) 2010 Azzurra IRC Network
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1.Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * 2.Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3.The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "stdinc.h"

#include "struct.h"
#include "send.h"
#include "client.h"
#include "channel.h"
#include "ircd.h"
#include "numeric.h"
#include "s_conf.h"
#include "hash.h"
#include "parse.h"
#include "hook.h"
#include "modules.h"
#include "match.h"
#include "s_serv.h"
#include "s_conf.h"
#include "s_newconf.h"
#include "operhash.h"

typedef enum
{
    SPAM_BLOCK,
    SPAM_BLOCK_WALLOPS,
    SPAM_BLOCK_SHUN,
    LAST_SPAM_TYPE
} spam_type;

static int me_spam(struct Client *, struct Client *, int, const char **);
static int me_unspam(struct Client *, struct Client *, int, const char **);
static int me_bspam(struct Client *, struct Client *, int, const char **);

static void h_spam_stats(hook_data_int *);
static void h_spam_burst_finished(hook_data_client *);
static void h_spam_client_message(hook_data_message *);
static void h_spam_channel_message(hook_data_message *);

static void add_spam(const char *, int, const char *, const char *);

static const char *spam_get_type(spam_type);
static int check_for_spam(struct Client *, const char *, const char *, const char *);

struct Message spam_msgtab = {
    "SPAM", 0, 0, 0, MFLG_SLOW,
    {mg_ignore, mg_ignore, mg_ignore, mg_ignore, {me_spam, 4}, mg_ignore}
};

struct Message unspam_msgtab = {
    "UNSPAM", 0, 0, 0, MFLG_SLOW,
    {mg_ignore, mg_ignore, mg_ignore, mg_ignore, {me_unspam, 2}, mg_ignore}
};

struct Message bspam_msgtab = {
    "BSPAM", 0, 0, 0, MFLG_SLOW,
    {mg_ignore, mg_ignore, mg_ignore, mg_ignore, {me_bspam, 5}, mg_ignore}
};

mapi_clist_av1 spam_clist[] = {
    &spam_msgtab,
    &unspam_msgtab,
    &bspam_msgtab,
    NULL
};

mapi_hfn_list_av1 spam_hfnlist[] = {
    {"burst_finished", (hookfn) h_spam_burst_finished},
    {"doing_stats", (hookfn) h_spam_stats},
    {"client_message", (hookfn) h_spam_client_message},
    {"channel_message", (hookfn) h_spam_channel_message},
    {NULL, NULL}
};

DECLARE_MODULE_AV1(spam, NULL, NULL, spam_clist, NULL, spam_hfnlist, "0.1");

static int
me_spam(struct Client *client_p, struct Client *source_p, int parc, const char *parv[])
{
    const char *mask, *reason, *oper;
    int type;

    /* mask type :reason */

    /* Command can be used only by services */
    if (!IsClient(source_p) || !IsService(source_p))
        return 0;

    mask = parv[1];
    type = atoi(parv[2]);
    reason = parv[3];
    oper = get_oper_name(source_p);

    add_spam(mask, type, oper, reason);
    return 0;
}

static int
me_unspam(struct Client *client_p, struct Client *source_p, int parc, const char *parv[])
{
    const char *mask;
    struct ConfItem *aconf;
    rb_dlink_node *ptr;

    /* mask */
    if (!IsClient(source_p) && !IsService(source_p))
        return 0;

    mask = parv[1];

    RB_DLINK_FOREACH(ptr, sline_conf_list.head)
    {
        aconf = ptr->data;

        if (irccmp(aconf->host, mask))
            continue;

        free_conf(aconf);
        rb_dlinkDestroy(ptr, &sline_conf_list);
        return 0;
    }
    return 0;
}

static int
me_bspam(struct Client *client_p, struct Client *source_p, int parc, const char *parv[])
{
    const char *mask, *reason, *oper;
    int type;

    /* mask type oper :reason */

    /* Command can be used only by bursting servers */
    if (IsClient(source_p) || HasSentEob(source_p))
        return 0;

    mask = parv[1];
    type = atoi(parv[2]);
    oper = parv[3];
    reason = parv[4];

    add_spam(mask, type, oper, reason);
    return 0;
}

static void
add_spam(const char *mask, int type, const char *oper, const char *reason)
{
    struct ConfItem *aconf;

    /* Reject invalid S:lines */
    if (type < 0 || type >= LAST_SPAM_TYPE)
        return;

    if ((aconf = find_sline_mask(mask)) != NULL)
    {
        /* already added, change type and reason */
        aconf->flags = type;
        rb_free(aconf->passwd);
        aconf->passwd = rb_strdup(reason);
    }
    else
    {
        aconf = make_conf();
        aconf->status = CONF_SLINE;
        aconf->host = rb_strdup(mask);
        aconf->passwd = rb_strdup(reason);
        aconf->port = type;

        collapse(aconf->host);

        aconf->info.oper = operhash_add(oper);

        rb_dlinkAddAlloc(aconf, &sline_conf_list);
    }
}

static void
h_spam_burst_finished(hook_data_client *hdata)
{
    struct ConfItem *aconf;
    rb_dlink_node *ptr;

    /* Don't burst S:lines to servers who don't understand ENCAP and to services */
    if (IsCapable(hdata->client, CAP_ENCAP) && !(hdata->client->flags & FLAGS_SERVICE))
    {
        /* Burst S:lines */
        RB_DLINK_FOREACH(ptr, sline_conf_list.head)
        {
            aconf = ptr->data;

            if(IsCapable(hdata->client, CAP_TS6))
                sendto_one(hdata->client, ":%s ENCAP * BSPAM %s %d %s :%s",
                       use_id(&me), aconf->host, aconf->port, aconf->info.oper,
                       aconf->passwd);
            else
                sendto_one(hdata->client, ":%s ENCAP * BSPAM %s %d %s :%s",
                       me.name, aconf->host, aconf->port, aconf->info.oper,
                       aconf->passwd);
        }
    }
}

static void
h_spam_stats(hook_data_int *hdata)
{
    char statchar = (char)hdata->arg2;
    struct ConfItem *aconf;
    rb_dlink_node *ptr;

    if (statchar == 'S' && IsOperAdmin(hdata->client))
    {
        SetCork(hdata->client);
        RB_DLINK_FOREACH(ptr, sline_conf_list.head)
        {
            aconf = ptr->data;
            sendto_one_numeric(hdata->client, RPL_STATSSPAM,
                       form_str(RPL_STATSSPAM), aconf->host,
                       spam_get_type(aconf->port), aconf->passwd);
        }
        ClearCork(hdata->client);
        send_pop_queue(hdata->client);
    }
}

static void
h_spam_client_message(hook_data_message *hdata)
{
    struct Client *target_p = (struct Client *)hdata->target;

    /* If messages is already marked as blocked don't process it */
    if (hdata->block)
        return;

    /* TODO: skip spam filter for targets with UMODE_NOSPAM */
    if (!IsOper(target_p) && !IsService(target_p) && (target_p != hdata->client))
        hdata->block = check_for_spam(hdata->client, target_p->name, hdata->command, hdata->text);
    else
        hdata->block = FALSE;
}

static void
h_spam_channel_message(hook_data_message *hdata)
{
    struct Channel *chptr = (struct Channel *)hdata->target;

    /* Message already blocked, skip it */
    if (hdata->block)
        return;

    /* TODO: skip spam filter for channels with MODE_NOSPAM */
    hdata->block = check_for_spam(hdata->client, chptr->chname, hdata->command, hdata->text);
}

static int
check_for_spam(struct Client *source_p, const char *target, const char *command, const char *text)
{
    struct ConfItem *aconf;
    char line[BUFSIZE];
    char *s = line;
    char *in_msg = (char *)text;
    
    int spam_block = FALSE;
    int spam_notice = FALSE;
    int spam_shun = FALSE;
    
    /* Skip spam check for IRC Operators, remote clients and services agents */
    if (IsOper(source_p) || !MyClient(source_p) || IsService(source_p))
        return FALSE;
    
    /* Strip control codes from message (partially stolen from xchat) */
    while (*in_msg)
    {
        /* Color codes: d/\x03[0-9]{,2},?[0-9]{,2}/ */
        if (*in_msg == '\003')
        {
            in_msg++;
            if (isdigit(*in_msg))
            {
                in_msg++;
                if (isdigit(*in_msg))
                    in_msg++;
            }
            if (*in_msg == ',')
            {
                in_msg++;
                if (isdigit(*in_msg))
                {
                    in_msg++;
                    if (isdigit(*in_msg))
                        in_msg++;
                }
            }
        }
        
        switch (*in_msg)
        {
            case '\002':        /* Bold */
            case '\007':        /* Beep */
            case '\017':        /* Reset */
            case '\026':        /* Reverse */
            case '\037':        /* Underline */
                break;
            default:
                *s++ = *in_msg;
                break;
        }
        in_msg++;
    }
    *s = '\0';
    
    if ((aconf = find_sline(line)) != NULL)
    {
        /* Got a match, check pattern type */
        switch (aconf->port)
        {
            case SPAM_BLOCK:
                spam_block = TRUE;
                break;
            case SPAM_BLOCK_WALLOPS:
                spam_block = TRUE;
                spam_notice = TRUE;
                break;
            case SPAM_BLOCK_SHUN:
                spam_block = TRUE;
                spam_notice = TRUE;
                spam_shun = TRUE;
                break;
            default:
                /* WTF??? */
                break;
        }
        
        /* Warn all opers */
        if (spam_notice)
        {
            rb_snprintf(line, sizeof(line),
                        "Blocked SPAM %s from %s (%s@%s) to %s. Matched with: %s (%s)",
                        command, source_p->name, source_p->username, source_p->host, target,
                        aconf->host, aconf->passwd);
            sendto_wallops_flags(UMODE_WALLOP, &me, "%s", line);
            sendto_server(NULL, NULL, CAP_TS6, NOCAPS, ":%s WALLOPS :%s",
                          use_id(&me), line);
            sendto_server(NULL, NULL, NOCAPS, CAP_TS6, ":%s WALLOPS :%s",
                          me.name, line);
        }
        
        /* TODO: shun sender */
        
        /* Block message */
        return spam_block;
    }
    
    /* No match, let this message through */
    return FALSE;
}

static const char *spam_type_names[LAST_SPAM_TYPE] = {
    "Block",
    "Block, send WALLOPS",
    "Block, send WALLOPS, shun sender"
};

static const char *
spam_get_type(spam_type t)
{
    if (t < 0 || t >= LAST_SPAM_TYPE)
        return "Invalid spam type?";
    else
        return spam_type_names[t];
}
