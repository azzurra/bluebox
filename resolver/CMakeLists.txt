
PROJECT(resolver)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF (UNIX)
  SET(CMAKE_EXTRA_INCLUDE_FILES "sys/types.h;sys/socket.h;netinet/in.h")
ENDIF(UNIX)

SET(resolver_SRCS
  res.c
  reslib.c
  resolver.c
)

ADD_EXECUTABLE(resolver ${resolver_SRCS})
ADD_DEPENDENCIES(resolver ratbox)
INCLUDE_DIRECTORIES(include ${bluebox_BINARY_DIR} ${libratbox_SOURCE_DIR}/include ${libratbox_BINARY_DIR})
TARGET_LINK_LIBRARIES(resolver ratbox)

INSTALL(TARGETS resolver DESTINATION libexec/bluebox)

