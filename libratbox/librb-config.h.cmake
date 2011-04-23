
#ifndef __LIBRB_CONFIG_H
#define __LIBRB_CONFIG_H

#include "libratbox_config.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#cmakedefine HAVE_SOCKLEN_T
#cmakedefine HAVE_SOCKADDR_STORAGE

#ifdef HAVE_SOCKLEN_T
typedef socklen_t rb_socklen_t;
#else
typedef int rb_socklen_t;
#endif

#ifdef HAVE_SOCKADDR_STORAGE
#define rb_sockaddr_storage sockaddr_storage
#else
struct rb_sockaddr_storage { uint8_t _padding[[128]]; };
#endif

#cmakedefine RB_SOCKADDR_HAS_SA_LEN 1

#cmakedefine HAVE_SOCKADDR_IN6

#ifdef HAVE_SOCKADDR_IN6
#define RB_IPV6 1
#endif

#endif // ifndef __LIBRB_CONFIG_H
