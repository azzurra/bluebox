
PROJECT(libsqlite3)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

#INCLUDE(CheckIncludeFiles)
#INCLUDE(CheckLibraryExists)
#INCLUDE(CheckFunctionExists)
#INCLUDE(CheckTypeSize)
#INCLUDE(${libratbox_SOURCE_DIR}/CheckStructMember.cmake)

#CHECK_INCLUDE_FILES(port.h HAVE_PORT_H)

#IF (UNIX)
#  SET(CMAKE_EXTRA_INCLUDE_FILES "sys/types.h;sys/socket.h;netinet/in.h")
#ENDIF(UNIX)

#CHECK_TYPE_SIZE(socklen_t SOCKLEN_T)
#CHECK_TYPE_SIZE("struct sockaddr_storage" SOCKADDR_STORAGE)

#CHECK_STRUCT_MEMBER("struct sockaddr" "sa_len" sys/socket.h RB_SOCKADDR_HAS_SA_LEN)
#
#CHECK_LIBRARY_EXISTS(socket connect "" HAVE_SOCKET)
#
#CHECK_FUNCTION_EXISTS(arc4random HAVE_ARC4RANDOM)

SET(sqlite3_LIB_SRCS
  sqlite3.c
)

#INCLUDE_DIRECTORIES(../include)
INCLUDE_DIRECTORIES(include ${PROJECT_BINARY_DIR})

ADD_LIBRARY(sqlite3 STATIC ${sqlite3_LIB_SRCS})
ADD_EXECUTABLE(ratbox-sqlite3 shell.c)
ADD_DEPENDENCIES(ratbox-sqlite3 sqlite3)
TARGET_LINK_LIBRARIES(ratbox-sqlite3 sqlite3)
TARGET_LINK_LIBRARIES(ratbox-sqlite3 pthread)

#ADD_DEPENDENCIES(sqlite3 version_c)

SET_TARGET_PROPERTIES(sqlite3 PROPERTIES VERSION 4.2.0 SOVERSION 4)
INSTALL(TARGETS sqlite3 DESTINATION lib)
