/* m_rsshortcut.c
 *   Contains the code for command shortcuts for ratbox-services
 *
 * Copyright (C) 2005 Lee Hardy <lee -at- leeh.co.uk>
 * Copyright (C) 2005 ircd-ratbox development team
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
 */
#include "stdinc.h"
#include "ratbox_lib.h"
#include "struct.h"
#include "modules.h"
#include "parse.h"
#include "hook.h"
#include "client.h"
#include "ircd.h"
#include "send.h"
#include "s_serv.h"
#include "hash.h"
static int m_nickserv(struct Client *, struct Client *, int, const char **);
static int m_chanserv(struct Client *, struct Client *, int, const char **);
static int m_memoserv(struct Client *, struct Client *, int, const char **);
static int m_operserv(struct Client *, struct Client *, int, const char **);
static int m_azzurra(struct Client *, struct Client *, int, const char **);
static int m_alis(struct Client *, struct Client *, int, const char **);

struct Message nickserv_msgtab = {
	"NICKSERV", 0, 0, 0, MFLG_SLOW,
	{mg_ignore, {m_nickserv, 2}, mg_ignore, mg_ignore, mg_ignore, {m_nickserv, 2}}
};
struct Message ns_msgtab = {
	"NS", 0, 0, 0, MFLG_SLOW,
	{mg_ignore, {m_nickserv, 2}, mg_ignore, mg_ignore, mg_ignore, {m_nickserv, 2}}
};

struct Message chanserv_msgtab = {
	"CHANSERV", 0, 0, 0, MFLG_SLOW,
	{mg_ignore, {m_chanserv, 2}, mg_ignore, mg_ignore, mg_ignore, {m_chanserv, 2}}
};
struct Message cs_msgtab = {
	"CS", 0, 0, 0, MFLG_SLOW,
	{mg_ignore, {m_chanserv, 2}, mg_ignore, mg_ignore, mg_ignore, {m_chanserv, 2}}
};

struct Message memoserv_msgtab = {
	"MEMOSERV", 0, 0, 0, MFLG_SLOW,
	{mg_ignore, {m_memoserv, 2}, mg_ignore, mg_ignore, mg_ignore, {m_memoserv, 2}}
};
struct Message ms_msgtab = {
	"MS", 0, 0, 0, MFLG_SLOW,
	{mg_ignore, {m_memoserv, 2}, mg_ignore, mg_ignore, mg_ignore, {m_memoserv, 2}}
};

struct Message operserv_msgtab = {
	"OPERSERV", 0, 0, 0, MFLG_SLOW,
	{mg_ignore, {m_operserv, 2}, mg_ignore, mg_ignore, mg_ignore, {m_operserv, 2}}
};
struct Message os_msgtab = {
	"OS", 0, 0, 0, MFLG_SLOW,
	{mg_ignore, {m_operserv, 2}, mg_ignore, mg_ignore, mg_ignore, {m_operserv, 2}}
};

struct Message azzurra_msgtab = {
	"AZZURRA", 0, 0, 0, MFLG_SLOW,
	{mg_ignore, {m_azzurra, 2}, mg_ignore, mg_ignore, mg_ignore, {m_azzurra, 2}}
};

struct Message alis_msgtab = {
	"ALIS", 0, 0, 0, MFLG_SLOW,
	{mg_ignore, {m_alis, 2}, mg_ignore, mg_ignore, mg_ignore, {m_alis, 2}}
};

mapi_clist_av1 rsshortcut_clist[] = {
	&nickserv_msgtab,
	&ns_msgtab,
	&chanserv_msgtab,
	&cs_msgtab,
	&memoserv_msgtab,
	&ms_msgtab,
	&operserv_msgtab,
	&os_msgtab,
	&azzurra_msgtab,
	&alis_msgtab,
	NULL
};

DECLARE_MODULE_AV1(rsshortcut, NULL, NULL, rsshortcut_clist, NULL, NULL, "1.0");


static int
m_nickserv(struct Client *client_p, struct Client *source_p,
	int parc, const char *parv[])
{
	char buf[BUFSIZE];
	int i = 1;

	if(find_server(NULL, "services.azzurra.org") == NULL)
	{
		sendto_one(source_p, 
			":%s 440 %s nickserv :Services are currently unavailable",
			me.name, source_p->name);
		return 0;
	}

	buf[0] = '\0';

	while(i < parc)
	{
		rb_strlcat(buf, parv[i], sizeof(buf));
		rb_strlcat(buf, " ", sizeof(buf));
		i++;
	}

	sendto_match_servs(client_p, "services.azzurra.org", CAP_ENCAP, NOCAPS,
			"ENCAP services.azzurra.org RSMSG nickserv %s",
			buf);
	return 0;
}

static int
m_chanserv(struct Client *client_p, struct Client *source_p,
	int parc, const char *parv[])
{
	char buf[BUFSIZE];
	int i = 1;

	if(find_server(NULL, "services.azzurra.org") == NULL)
	{
		sendto_one(source_p, 
			":%s 440 %s chanserv :Services are currently unavailable",
			me.name, source_p->name);
		return 0;
	}

	buf[0] = '\0';

	while(i < parc)
	{
		rb_strlcat(buf, parv[i], sizeof(buf));
		rb_strlcat(buf, " ", sizeof(buf));
		i++;
	}

	sendto_match_servs(client_p, "services.azzurra.org", CAP_ENCAP, NOCAPS,
			"ENCAP services.azzurra.org RSMSG chanserv %s",
			buf);
	return 0;
}

static int
m_memoserv(struct Client *client_p, struct Client *source_p,
	int parc, const char *parv[])
{
	char buf[BUFSIZE];
	int i = 1;

	if(find_server(NULL, "services.azzurra.org") == NULL)
	{
		sendto_one(source_p, 
			":%s 440 %s memoserv :Services are currently unavailable",
			me.name, source_p->name);
		return 0;
	}

	buf[0] = '\0';

	while(i < parc)
	{
		rb_strlcat(buf, parv[i], sizeof(buf));
		rb_strlcat(buf, " ", sizeof(buf));
		i++;
	}

	sendto_match_servs(client_p, "services.azzurra.org", CAP_ENCAP, NOCAPS,
			"ENCAP services.azzurra.org RSMSG memoserv %s",
			buf);
	return 0;
}

static int
m_operserv(struct Client *client_p, struct Client *source_p,
	int parc, const char *parv[])
{
	char buf[BUFSIZE];
	int i = 1;

	if(find_server(NULL, "services.azzurra.org") == NULL)
	{
		sendto_one(source_p, 
			":%s 440 %s operserv :Services are currently unavailable",
			me.name, source_p->name);
		return 0;
	}

	buf[0] = '\0';

	while(i < parc)
	{
		rb_strlcat(buf, parv[i], sizeof(buf));
		rb_strlcat(buf, " ", sizeof(buf));
		i++;
	}

	sendto_match_servs(client_p, "services.azzurra.org", CAP_ENCAP, NOCAPS,
			"ENCAP services.azzurra.org RSMSG operserv %s",
			buf);
	return 0;
}

static int
m_azzurra(struct Client *client_p, struct Client *source_p,
	int parc, const char *parv[])
{
	char buf[BUFSIZE];
	int i = 1;

	if(find_server(NULL, "services.azzurra.org") == NULL)
	{
		sendto_one(source_p, 
			":%s 440 %s azzurra :Services are currently unavailable",
			me.name, source_p->name);
		return 0;
	}

	buf[0] = '\0';

	while(i < parc)
	{
		rb_strlcat(buf, parv[i], sizeof(buf));
		rb_strlcat(buf, " ", sizeof(buf));
		i++;
	}

	sendto_match_servs(client_p, "services.azzurra.org", CAP_ENCAP, NOCAPS,
			"ENCAP services.azzurra.org RSMSG azzurra %s",
			buf);
	return 0;
}

static int
m_alis(struct Client *client_p, struct Client *source_p,
	int parc, const char *parv[])
{
	char buf[BUFSIZE];
	int i = 1;

	if(find_server(NULL, "services.azzurra.org") == NULL)
	{
		sendto_one(source_p, 
			":%s 440 %s alis :Services are currently unavailable",
			me.name, source_p->name);
		return 0;
	}

	buf[0] = '\0';

	while(i < parc)
	{
		rb_strlcat(buf, parv[i], sizeof(buf));
		rb_strlcat(buf, " ", sizeof(buf));
		i++;
	}

	sendto_match_servs(client_p, "services.azzurra.org", CAP_ENCAP, NOCAPS,
			"ENCAP services.azzurra.org RSMSG alis %s",
			buf);
	return 0;
}
