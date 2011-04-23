/* contrib/m_hdump.c
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
#include "ircd.h"
#include "numeric.h"
#include "s_conf.h"
#include "hash.h"
#include "parse.h"
#include "hook.h"
#include "match.h"
#include "modules.h"
#include "s_serv.h"
#include "s_conf.h"
#include "s_newconf.h"

static int mo_hdump(struct Client *, struct Client *, int, const char **);
static int hdump_find_hook(const char *);
static const char *resolve_module_name(hookfn);
static void dump_all_hooks(struct Client *);
static void dump_single_hook(struct Client *, const char *);
static void dump_single_hook_byid(struct Client *, int);

struct Message hdump_msgtab = {
    "HDUMP", 0, 0, 0, MFLG_SLOW,
    {mg_ignore, mg_not_oper, mg_ignore, mg_ignore, mg_ignore, {mo_hdump, 0}}
};

mapi_clist_av2 hdump_clist[] = {
    &hdump_msgtab,
    NULL
};

/* Bring in external symbols directly.
 * I know, this is not the right way of doing things, but this is just a debug module */
extern hook *hooks;
extern int max_hooks;
extern struct module **modlist;
extern int num_mods;

DECLARE_MODULE_AV2(hdump, NULL, NULL, hdump_clist, NULL, NULL, "0.1");

static int
mo_hdump(struct Client *client_p, struct Client *source_p, int parc, const char *parv[])
{
    const char *hname;
    /* :hook_name (optional) */

    /* Command can only be used by local admins */
    if (!IsOperAdmin(source_p))
    {
        sendto_one_numeric(source_p, ERR_NOPRIVILEGES, form_str(ERR_NOPRIVILEGES));
        return 0;
    }
	
    SetCork(source_p);

    if (parc < 2 || EmptyString(parv[1]))
        dump_all_hooks(source_p);
    else
        dump_single_hook(source_p, parv[1]);

    ClearCork(source_p);
    send_pop_queue(source_p);

    return 0;
}

static void
dump_all_hooks(struct Client *source_p)
{
    int i;

    for (i = 0; i < max_hooks; i++)
    {
        if (EmptyString(hooks[i].name))
            continue;

        dump_single_hook_byid(source_p, i);
    }
}

static void
dump_single_hook(struct Client *source_p, const char *hook_name)
{
    int i;
    
    i = hdump_find_hook(hook_name);
    if (i == -1)
    {
        sendto_one_notice(source_p, ":Hook %s is not defined", hook_name);
        return;
    }
    
    dump_single_hook_byid(source_p, i);
}

static const char *hprio_to_str[] = {
    "HPRIO_LOWEST", "HPRIO_LOW", "HPRIO_NORMAL", "HPRIO_HIGH", "HPRIO_HIGHEST"
};

static void
dump_single_hook_byid(struct Client *source_p, int id)
{
    rb_dlink_node *ptr;
    struct hook_info *hinfo;
    int i = 1;
    
    sendto_one_notice(source_p, ":*** Hook function list for %s", hooks[id].name);
    RB_DLINK_FOREACH(ptr, hooks[id].hooks.head)
    {
        hinfo = ptr->data;
        sendto_one_notice(source_p, ": %i. 0x%p [Module: %s - Priority: %s]",
                i++, (void *)hinfo->fn, resolve_module_name(hinfo->fn), ((hinfo->prio >= HPRIO_LOWEST && hinfo->prio <= HPRIO_HIGHEST) ? hprio_to_str[hinfo->prio] : "<unknown>"));
    }
    sendto_one_notice(source_p, ":*** End of hook function list");
}

static int
hdump_find_hook(const char *name)
{
    int i;

    for (i = 0; i < max_hooks; i++)
    {
        if (!hooks[i].name)
            continue;

        if (!irccmp(hooks[i].name, name))
            return i;
    }

    return -1;
}

static const char *
resolve_module_name(hookfn fn)
{
    static const char *unknown_modname = "<unknown>";
    int i;
    
    for (i = 0; i < num_mods; i++)
    {
        switch(modlist[i]->mapi_version)
        {
        case 1:
        {
            struct mapi_mheader_av1 *mheader = modlist[i]->mapi_header;
            if (mheader->mapi_hfn_list)
            {
                mapi_hfn_list_av1 *m;
                for (m = mheader->mapi_hfn_list; m->hapi_name; ++m)
                {
                    if (m->hookfn == fn)
                        return modlist[i]->name;
                }
            }
            break;
        }
        case 2:
        {
            struct mapi_mheader_av2 *mheader = modlist[i]->mapi_header;
            if (mheader->mapi_hfn_list)
            {
                mapi_hfn_list_av2 *m;
                for (m = mheader->mapi_hfn_list; m->hapi_name; ++m)
                {
                    if (m->hookfn == fn)
                        return modlist[i]->name;
                }
            }
            break;
        }
        default:
            break;
        }
    }
    /* No match */
    return unknown_modname;
}
