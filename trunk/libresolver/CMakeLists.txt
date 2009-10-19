
PROJECT(libresolver)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF (UNIX)
  SET(CMAKE_EXTRA_INCLUDE_FILES "sys/types.h;sys/socket.h;netinet/in.h")
ENDIF(UNIX)

SET(resolver_LIB_SRCS
  res.c
  reslib.c
  resolver.c
)

#INCLUDE_DIRECTORIES(../include)

ADD_LIBRARY(resolver SHARED ${resolver_LIB_SRCS})
ADD_DEPENDENCIES(resolver ratbox)
INCLUDE_DIRECTORIES(include ${bluebox_BINARY_DIR} ${libratbox_SOURCE_DIR}/include ${libratbox_BINARY_DIR})

#IF (HAVE_SOCKET)
#  TARGET_LINK_LIBRARIES(resolver socket)
#ENDIF (HAVE_SOCKET)
#
#IF (HAVE_RESOLV)
#  TARGET_LINK_LIBRARIES(resolver resolv)
#ENDIF (HAVE_RESOLV)
#
#IF (OPENSSL_FOUND)
#  ADD_DEFINITIONS(-DHAVE_OPENSSL -DHAVE_SSL)
#  TARGET_LINK_LIBRARIES(resolver ${OPENSSL_LIBRARIES})
#ENDIF (OPENSSL_FOUND)
#
#IF (HAVE_CRYPT)
#  TARGET_LINK_LIBRARIES(resolver crypt)
#ENDIF (HAVE_CRYPT)


SET_TARGET_PROPERTIES(resolver PROPERTIES VERSION 4.2.0 SOVERSION 4)
INSTALL(TARGETS resolver DESTINATION lib)

#CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/librb-config.h.cmake
#               ${PROJECT_BINARY_DIR}/librb-config.h)
#
#CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/libresolver_config.h.cmake
#               ${PROJECT_BINARY_DIR}/libresolver_config.h)

########### install files ###############

