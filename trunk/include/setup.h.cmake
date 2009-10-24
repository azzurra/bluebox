
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
