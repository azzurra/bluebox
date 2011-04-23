/*
 *  ircd-ratbox: A slightly useful ircd.
 *  s_serv.h: A header for the server functions.
 *
 *  Copyright (C) 1990 Jarkko Oikarinen and University of Oulu, Co Center
 *  Copyright (C) 1996-2002 Hybrid Development Team
 *  Copyright (C) 2002-2005 ircd-ratbox development team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 *
 *  $Id: s_serv.h 26094 2008-09-19 15:33:46Z androsyn $
 */

#ifndef INCLUDED_serv_h
#define INCLUDED_serv_h

/*
 * The number of seconds between calls to try_connections(). Fiddle with
 * this ONLY if you KNOW what you're doing!
 */
#define TRY_CONNECTIONS_TIME    60

/*
 * number of seconds to wait after server starts up, before
 * starting try_connections()
 * TOO SOON and you can nick collide like crazy.
 */
#define STARTUP_CONNECTIONS_TIME 60

struct Client;
struct server_conf;
struct Channel;

/* Capabilities */
struct Capability
{
    const char *name;   /* name of capability */
    unsigned int cap;   /* mask value */
};

#define CAP_CAP         0x000001    /* received a CAP to begin with */
#define CAP_QS          0x000002    /* Can handle quit storm removal */
#define CAP_EX          0x000004    /* Can do channel +e exemptions */
#define CAP_CHW         0x000008    /* Can do channel wall @# */
#define CAP_IE          0x000010    /* Can do invite exceptions */
#define CAP_GLN         0x000080    /* Can do GLINE message */
#define CAP_ZIP         0x000100    /* Can do ZIPlinks */
#define CAP_KNOCK       0x000400    /* supports KNOCK */
#define CAP_TB          0x000800    /* supports TBURST */
#define CAP_ENCAP       0x004000    /* supports ENCAP */
#define CAP_TS6         0x008000    /* supports TS6 or above */
#define CAP_SERVICE     0x010000
#define CAP_RSFNC       0x020000    /* rserv FNC */
#define CAP_SAVE        0x040000    /* supports SAVE (nick collision FNC) */
#define CAP_SAVETS_100  0x080000    /* supports SAVE at TS 100 */
#define CAP_SSJOIN      0x100000    /* support SSJOIN (compatibility hack for bahamut) */
#define CAP_NICKIP      0x200000    /* support NICKIP (compatibility hack for bahamut) */

#define CAP_MASK        (CAP_QS  | CAP_EX   | CAP_CHW  | \
             CAP_IE  | CAP_SERVICE |\
             CAP_GLN | CAP_ENCAP | \
             CAP_ZIP  | CAP_KNOCK  | \
             CAP_RSFNC | CAP_SAVE | CAP_SAVETS_100 | CAP_SSJOIN | CAP_NICKIP)
/*
 * Capability macros.
 */
#define IsCapable(x, cap)       (((x)->localClient->caps & (cap)) == cap)
#define NotCapable(x, cap)      (((x)->localClient->caps & (cap)) == 0)
#define ClearCap(x, cap)        ((x)->localClient->caps &= ~(cap))

/*
 * Globals
 *
 *
 * list of recognized server capabilities.  "TS" is not on the list
 * because all servers that we talk to already do TS, and the kludged
 * extra argument to "PASS" takes care of checking that.  -orabidoo
 */
extern struct Capability captab[];

extern int MaxClientCount;      /* GLOBAL - highest number of clients */
extern int MaxConnectionCount;  /* GLOBAL - highest number of connections */

extern int refresh_user_links;

/*
 * return values for hunt_server()
 */
#define HUNTED_NOSUCH   (-1)    /* if the hunted server is not found */
#define HUNTED_ISME     0       /* if this server should execute the command */
#define HUNTED_PASS     1       /* if message passed onwards successfully */


int hunt_server(struct Client *client_pt,
                struct Client *source_pt,
                const char *command, int server, int parc, const char **parv);
void send_capabilities(struct Client *, int);
const char *show_capabilities(struct Client *client);
void try_connections(void *unused);

int serv_connect(struct server_conf *, struct Client *);

#endif /* INCLUDED_s_serv_h */
