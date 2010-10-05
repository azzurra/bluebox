/*
 *  bluebox: A slightly useful ircd.
 *  cloak.c: The ircd host cloaking subsystem.
 *
 *  Copyright (C) 1990 Jarkko Oikarinen and University of Oulu, Co Center
 *  Copyright (C) 1996-2002 Hybrid Development Team
 *  Copyright (C) 2002-2005 ircd-ratbox development team
 *  Copyright (C) 2009 Azzurra IRC Network development team
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
 */

#include <ratbox_lib.h>
#include "stdinc.h"
#include "struct.h"
#include "client.h"
#include "ircd.h"
#include "send.h"
#include "hostmask.h"
#include "s_conf.h"
#include "s_log.h"
#include "sha1.h"
#include "cloak.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

static unsigned char *cloak_network;
static size_t cloak_network_len;
static unsigned char *cloak_key;
static size_t cloak_key_len;

void init_cloak(void)
{
    int fd;

    /* FIXME: should this change after a rehash? */
    cloak_network = rb_strdup(ServerInfo.network_name);
    cloak_network_len = strlen(cloak_network);

    if ((fd = open(ETC_DIR "/ircd.cloak", O_RDONLY)) >= 0)
    {
        struct stat st;
        if (fstat(fd, &st) == 0)
        {
            int sz = st.st_size;
            if (sz > MIN_CLOAK_KEY_LEN)
            {
                int rv;
                if (sz > MAX_CLOAK_KEY_LEN)
                    sz = MAX_CLOAK_KEY_LEN;     /* we aren't NASA */
                cloak_key = rb_malloc(sz + 1);
                if ((rv = read(fd, (void *)cloak_key, sz)) != sz)
                {
                    /* Short read */
                    (void) close(fd);
                    ilog(L_MAIN,
                         "Error while reading cloak key (expected %d bytes, got %d): terminating ircd",
                         sz, rv);
                    exit(0);
                }
                (void) close(fd);
                cloak_key[sz] = '\0';
                cloak_key_len = strlen(cloak_key);
            }
            else
            {
                /* Wrong size */
                ilog(L_MAIN,
                     "Cloak key is too short (%d bytes): terminating ircd", sz);
                (void) close(fd);
                exit(0);
            }
        }
        else
        {
            /* fstat failure */
            ilog(L_MAIN,
                 "Failed to stat cloak key file (%s): terminating ircd", strerror(errno));
            (void) close(fd);
            exit(0);
        }
    }
    else
    {
        /* open failed */
        ilog(L_MAIN,
             "Can't open cloak key file (%s): terminating ircd", strerror(errno));
        exit(0);
    }
    /* phew! */
    ilog(L_MAIN, "Host cloaking subsystem initialized: host %s, key length %d bits",
         cloak_network, cloak_key_len * 8);
}

void update_cloak_key(const unsigned char *newkey, size_t newkeylen)
{
    ;
}

#define FNV_PRIME 16777619UL

static inline int32_t fnv_hash(const char *p, size_t s)
{
    int32_t h = 0;
    int i = 0;

    for (; i < s; i++)
        h = (h * FNV_PRIME) ^ p[i];

    return h;
}

#define SHABUFLEN SHA1_DIGEST_LENGTH*2

static char *sha1_hash(const char *s, size_t len)
{
    static char shabuf[SHABUFLEN + 1];
    unsigned char digest[SHA1_DIGEST_LENGTH];
    int i;
    SHA1_CTX ctx;

    SHA1Init(&ctx);
    SHA1Update(&ctx, s, len);
    SHA1Update(&ctx, cloak_key, cloak_key_len);
    SHA1Final(digest, &ctx);

    for (i = 0; i < SHA1_DIGEST_LENGTH; i++)
        rb_snprintf(shabuf+2*i, sizeof(shabuf) - 2*i, "%02x", digest[i]);
    shabuf[SHABUFLEN] = '\0';

    return shabuf;
}

int cloak_host(struct Client *target_p)
{
    int isdns = NO, chlen = cloak_network_len + 10;
    char *p, *shabuf;
    unsigned short i;
    int32_t csum;

    shabuf = sha1_hash(target_p->host, strlen(target_p->host));
    csum = fnv_hash(shabuf, strlen(shabuf));

    for (p = target_p->host, i = 0; *p; p++)
    {
        if (!isdns && isalpha(*p))
            isdns = YES;
        else if (*p == '.')
            i++;
    }

    memset(target_p->virthost, 0, sizeof(target_p->virthost));

    if (isdns)
    {
        switch (i)
        {
        case 0:
            return 0;
        case 1:
            rb_snprintf(target_p->virthost, sizeof(target_p->virthost),
                        "%s%c%X.%s",
                        cloak_network,
                        csum < 0 ? '=' : '-',
                        csum < 0 ? -csum : csum,
                        target_p->host);
            return 1;
        default:
            p = strchr(target_p->host, '.');
            while ((strlen(p) + chlen) > HOSTLEN)
            {
                if ((p = strchr(++p, '.')) == NULL)
                    return 0;
            }
            rb_snprintf(target_p->virthost, sizeof(target_p->virthost),
                        "%s%c%X.%s",
                        cloak_network,
                        csum < 0 ? '=' : '-',
                        csum < 0 ? -csum : csum,
                        p + 1);
            return 1;
        }
        /* NOT REACHED */
        return 0;
    }
    else
    {
        char ipmask[16];

        rb_strlcpy(ipmask, target_p->host, sizeof(ipmask));
        ipmask[sizeof(ipmask) - 1] = '\0';

        if ((p = strchr(ipmask, '.')) != NULL)
            if ((p = strchr(p + 1, '.')) != NULL)
                *p = '\0';

        if (p == NULL)
            rb_snprintf(target_p->virthost, sizeof(target_p->virthost),
                        "%s%c%X",
                        cloak_network,
                        csum < 0 ? '=' : '-',
                        csum < 0 ? -csum : csum);
        else
            rb_snprintf(target_p->virthost, sizeof(target_p->virthost),
                        "%s.%s%c%X",
                        ipmask, cloak_network,
                        csum < 0 ? '=' : '-',
                        csum < 0 ? -csum : csum);
        return 1;
    }
    /* NOT REACHED */
    return 0;
}
