/*
 *  bluebox: A slightly useful ircd.
 *  cloak.h: The ircd host cloaking header.
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

#ifndef INCLUDED_cloak_h
#define INCLUDED_cloak_h

#define MIN_CLOAK_KEY_LEN 64
#define MAX_CLOAK_KEY_LEN 512

void init_cloak(void);
int cloak_host(struct Client *);
void update_cloak_key(const unsigned char *, size_t);

#endif /* INCLUDED_cloak_h */
