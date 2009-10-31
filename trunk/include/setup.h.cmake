
#cmakedefine HAVE_SYS_RESOURCE_H 1
#cmakedefine HAVE_SNPRINTF 1

#cmakedefine HAVE_SYS_DIR_H 1
#cmakedefine HAVE_SYS_NDIR_H 1
#cmakedefine HAVE_SYS_PARAM_H 1
#cmakedefine HAVE_SYS_SOCKET_H 1
#cmakedefine HAVE_SYS_STAT_H 1
#cmakedefine HAVE_SYS_TYPES_H 1
#cmakedefine HAVE_SYS_WAIT_H 1
#cmakedefine HAVE_ALLOCA_H 1
#cmakedefine HAVE_ARPA_INET_H 1
#cmakedefine HAVE_DIRENT_H 1
#cmakedefine HAVE_DLFCN_H 1
#cmakedefine HAVE_ERRNO_H 1
#cmakedefine HAVE_INTTYPES_H 1
#cmakedefine HAVE_MEMORY_H 1
#cmakedefine HAVE_NDIR_H 1
#cmakedefine HAVE_NETINET_IN_H 1
#cmakedefine HAVE_STDDEF_H 1
#cmakedefine HAVE_STDINT_H 1
#cmakedefine HAVE_STDLIB_H 1
#cmakedefine HAVE_STRINGS_H 1
#cmakedefine HAVE_STRING_H 1
#cmakedefine HAVE_UNISTD_H 1
#cmakedefine USE_CHALLENGE 1

#cmakedefine SHLIBEXT "${SHLIBEXT}"

/* static values XXX perhaps add a shell script to obtain these values? 
 * Maybe some cmake options? -morph
 */
#define NICKLEN 30

#cmakedefine IRCD_PREFIX "${IRCD_PREFIX}"

#define LIBEXEC_DIR IRCD_PREFIX "/libexec/bluebox"
#define ETC_DIR IRCD_PREFIX "/etc/bluebox"
#define LOG_DIR IRCD_PREFIX "/var/log/bluebox"
#define HELP_DIR IRCD_PREFIX "/etc/bluebox/help"
#define MODULE_DIR IRCD_PREFIX "/modules"

#define RATBOX_CONFIGURE_OPTS "powered by t[c]make"

#define NICKNAMEHISTORYLENGTH 30000

#define AUTH_HEAP_SIZE 256
#define AWAY_HEAP_SIZE 512
#define BAN_HEAP_SIZE 4096
#define CHANNEL_HEAP_SIZE 8192
#define CLIENT_HEAP_SIZE 8192
#define CONFITEM_HEAP_SIZE 1024
#define DNODE_HEAP_SIZE 8192
#define FD_HEAP_SIZE 1024
#define LCLIENT_HEAP_SIZE 1024
#define LINEBUF_HEAP_SIZE 2048
#define MEMBER_HEAP_SIZE 32768
#define MONITOR_HEAP_SIZE 1024
#define ND_HEAP_SIZE 512
#define TOPIC_HEAP_SIZE 4096
#define USER_HEAP_SIZE 8192

/* Try to figure out *TARGET* endianess.
 * We rely on target's libc and/or compiler-defined macros so
 * cross-compilation (or multitarget compilation - hello, Darwin!)
 * works properly.
 *
 * Code shamelessly stolen and adapted from Boost's endian.hpp.
 * Copyright 2005 Caleb Epstein
 * Copyright 2006 John Maddock
 * Distributed under the Boost Software License, Version 1.0. (See
 * http://www.boost.org/LICENSE_1_0.txt)
 * Copyright (c) 1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */
 
#if defined(__GLIBC__)
#include <endian.h>
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#define BLUEBOX_LITTLE_ENDIAN
#elif (__BYTE_ORDER == __BIG_ENDIAN)
#define BLUEBOX_BIG_ENDIAN
#else
#error "Is this a real PDP-11? Wow..."
#endif
#define BLUEBOX_BYTE_ORDER __BYTE_ORDER
#elif defined(_BIG_ENDIAN)
#define BLUEBOX_BIG_ENDIAN
#define BLUEBOX_BYTE_ORDER 4321
#elif defined(_LITTLE_ENDIAN)
#define BLUEBOX_LITTLE_ENDIAN
#define BLUEBOX_BYTE_ORDER 1234
#elif defined(__sparc) || defined(__sparc__) \
	|| defined(_POWER) || defined(__powerpc__) \
	|| defined(__ppc__) || defined(__hpux) \
	|| defined(_MIPSEB) || defined(_POWER) \
	|| defined(__s390__)
#define BLUEBOX_BIG_ENDIAN
#define BLUEBOX_BYTE_ORDER 4321
#elif defined(__i386__) || defined(__alpha__) \
	|| defined(__ia64) || defined(__ia64__) \
	|| defined(_M_IX86) || defined(_M_IA64) \
	|| defined(_M_ALPHA) || defined(__amd64) \
	|| defined(__amd64__) || defined(_M_AMD64) \
	|| defined(__x86_64) || defined(__x86_64__) \
	|| defined(_M_X64)
#define BLUEBOX_LITTLE_ENDIAN
#define BLUEBOX_BYTE_ORDER 1234
#else
#error "Unknown CPU endianess."
#endif
