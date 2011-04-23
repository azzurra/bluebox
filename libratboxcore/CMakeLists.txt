
PROJECT(libratboxcore)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckTypeSize)
INCLUDE(FindOpenSSL)
INCLUDE(FindCrypto)

SET(libratboxcore_LIB_SRCS
  bandbi.c
  cache.c
  channel.c
  class.c
  client.c
  cloak.c
  dns.c
  getopt.c
  hash.c
  hook.c
  hostmask.c
  ircd.c
  ircd_lexer.c
  ircd_lexer.l
  ircd_parser.c
  ircd_parser.h
  ircd_parser.y
  ircd_signal.c
  listener.c
  match.c
  messages.tab
  modules.c
  monitor.c
  newconf.c
  numeric.c
  operhash.c
  packet.c
  parse.c
  reject.c
  restart.c
  s_auth.c
  s_conf.c
  s_log.c
  s_newconf.c
  s_serv.c
  s_user.c
  scache.c
  send.c
  sha1.c
  sslproc.c
  supported.c
  whowas.c
  version.c
)

INCLUDE_DIRECTORIES(${bluebox_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} ${libratbox_SOURCE_DIR}/include ${libratbox_BINARY_DIR})

ADD_CUSTOM_COMMAND(OUTPUT version.c
                  COMMAND ${PROJECT_SOURCE_DIR}/version.c.SH
                  COMMENT "generating version.c")

ADD_LIBRARY(ratboxcore SHARED ${libratboxcore_LIB_SRCS})
ADD_DEPENDENCIES(ratboxcore version_c)
INCLUDE_DIRECTORIES(include ${PROJECT_BINARY_DIR})
IF (APPLE)
  TARGET_LINK_LIBRARIES(ratboxcore ratbox)
ENDIF(APPLE)

IF (HAVE_SOCKET)
  TARGET_LINK_LIBRARIES(ratboxcore socket)
ENDIF (HAVE_SOCKET)

IF (HAVE_RESOLV)
  TARGET_LINK_LIBRARIES(ratboxcore resolv)
ENDIF (HAVE_RESOLV)

IF (OPENSSL_FOUND)
  ADD_DEFINITIONS(-DHAVE_OPENSSL -DHAVE_SSL)
  TARGET_LINK_LIBRARIES(ratboxcore ${OPENSSL_LIBRARIES})
  IF (APPLE)
    # OSX's broken linker requires explicit linking against libcrypto
    IF (CRYPTO_FOUND)
      TARGET_LINK_LIBRARIES(ratboxcore ${CRYPTO_LIBRARIES})
    ELSE (CRYPTO_FOUND)
      MESSAGE(FATAL_ERROR "libcrypto not found (broken OpenSSL setup?)")
    ENDIF (CRYPTO_FOUND)
  ENDIF (APPLE)
ENDIF (OPENSSL_FOUND)

IF (HAVE_CRYPT)
  TARGET_LINK_LIBRARIES(ratboxcore crypt)
ENDIF (HAVE_CRYPT)

#IF (ZLIB_FOUND)
#  INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
#  TARGET_LINK_LIBRARIES(ssld ${ZLIB_LIBRARIES})
#  ADD_DEFINITIONS(-DHAVE_ZLIB)
#ENDIF (ZLIB_FOUND)

SET_TARGET_PROPERTIES(ratboxcore PROPERTIES VERSION 4.2.0 SOVERSION 4)
INSTALL(TARGETS ratboxcore DESTINATION lib)

