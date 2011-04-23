
PROJECT(libratbox)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(CheckIncludeFile)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckTypeSize)
INCLUDE(${libratbox_SOURCE_DIR}/CheckStructMember.cmake)
INCLUDE(FindOpenSSL)
INCLUDE(FindCrypto)

CHECK_INCLUDE_FILE(port.h HAVE_PORT_H)
CHECK_INCLUDE_FILE(spawn.h HAVE_SPAWN_H)
CHECK_INCLUDE_FILE(sys/devpoll.h HAVE_SYS_DEVPOLL_H)
CHECK_INCLUDE_FILE(sys/event.h HAVE_SYS_EVENT_H)
CHECK_INCLUDE_FILE(sys/poll.h HAVE_SYS_POLL_H)
CHECK_INCLUDE_FILE(sys/select.h HAVE_SYS_SELECT_H)
CHECK_INCLUDE_FILE(sys/signalfd.h HAVE_SYS_SIGNALFD_H)
CHECK_INCLUDE_FILE(sys/uio.h HAVE_SYS_UIO_H)

IF (UNIX)
  SET(CMAKE_EXTRA_INCLUDE_FILES "sys/types.h;sys/socket.h;netinet/in.h")
ENDIF(UNIX)

CHECK_TYPE_SIZE(socklen_t SOCKLEN_T)
CHECK_TYPE_SIZE("struct sockaddr_storage" SOCKADDR_STORAGE)
CHECK_TYPE_SIZE("struct sockaddr_in6" SOCKADDR_IN6)

CHECK_STRUCT_MEMBER("struct sockaddr" "sa_len" sys/socket.h RB_SOCKADDR_HAS_SA_LEN)

CHECK_LIBRARY_EXISTS(socket connect "" HAVE_SOCKET)
CHECK_LIBRARY_EXISTS(resolv gethostbyname "" HAVE_RESOLV)
CHECK_LIBRARY_EXISTS(crypt crypt "" HAVE_CRYPT)

CHECK_FUNCTION_EXISTS(arc4random HAVE_ARC4RANDOM)
CHECK_FUNCTION_EXISTS(devpoll HAVE_DEVPOLL)
CHECK_FUNCTION_EXISTS(epoll_ctl HAVE_EPOLL_CTL)
CHECK_FUNCTION_EXISTS(getrusage HAVE_GETRUSAGE)
CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)
CHECK_FUNCTION_EXISTS(gmtime_r HAVE_GMTIME_R)
CHECK_FUNCTION_EXISTS(kevent HAVE_KEVENT)
CHECK_FUNCTION_EXISTS(mmap HAVE_MMAP)
CHECK_FUNCTION_EXISTS(nanosleep HAVE_NANOSLEEP)
CHECK_FUNCTION_EXISTS(poll HAVE_POLL)
CHECK_FUNCTION_EXISTS(port_create HAVE_PORT_CREATE)
CHECK_FUNCTION_EXISTS(posix_spawn HAVE_POSIX_SPAWN)
CHECK_FUNCTION_EXISTS(select HAVE_SELECT)
CHECK_FUNCTION_EXISTS(sendmsg HAVE_SENDMSG)
CHECK_FUNCTION_EXISTS(signalfd HAVE_SIGNALFD)
CHECK_FUNCTION_EXISTS(socketpair HAVE_SOCKETPAIR)
CHECK_FUNCTION_EXISTS(strlcat HAVE_STRLCAT)
CHECK_FUNCTION_EXISTS(strlcpy HAVE_STRLCPY)
CHECK_FUNCTION_EXISTS(strnlen HAVE_STRNLEN)
CHECK_FUNCTION_EXISTS(strtok_r HAVE_STRTOK_R)
CHECK_FUNCTION_EXISTS(writev HAVE_WRITEV)

SET(ratbox_LIB_SRCS
src/unix.c      
src/crypt.c     
src/balloc.c    
src/commio.c    
src/openssl.c   
src/gnutls.c    
src/nossl.c     
src/event.c     
src/ratbox_lib.c   
src/rb_memory.c  
src/linebuf.c   
src/snprintf.c  
src/tools.c     
src/helper.c    
src/devpoll.c   
src/epoll.c     
src/poll.c      
src/ports.c     
src/sigio.c     
src/select.c    
src/kqueue.c    
src/rawbuf.c    
src/patricia.c  
src/arc4random.c  
version.c
)

INCLUDE_DIRECTORIES(../include)
ADD_CUSTOM_COMMAND(OUTPUT version.c
                  COMMAND ${PROJECT_SOURCE_DIR}/src/version.c.SH
                  COMMENT "generating version.c")

ADD_LIBRARY(ratbox SHARED ${ratbox_LIB_SRCS})
ADD_DEPENDENCIES(ratbox version_c)
INCLUDE_DIRECTORIES(include ${PROJECT_BINARY_DIR})

IF (HAVE_SOCKET)
  TARGET_LINK_LIBRARIES(ratbox socket)
ENDIF (HAVE_SOCKET)

IF (HAVE_RESOLV)
  TARGET_LINK_LIBRARIES(ratbox resolv)
ENDIF (HAVE_RESOLV)

IF (OPENSSL_FOUND)
  ADD_DEFINITIONS(-DHAVE_OPENSSL -DHAVE_SSL)
  TARGET_LINK_LIBRARIES(ratbox ${OPENSSL_LIBRARIES})
  IF (APPLE)
    # OSX's broken linker requires explicit linking against libcrypto
    IF (CRYPTO_FOUND)
      TARGET_LINK_LIBRARIES(ratbox ${CRYPTO_LIBRARIES})
    ELSE (CRYPTO_FOUND)
      MESSAGE(FATAL_ERROR "libcrypto not found (broken OpenSSL setup?)")
    ENDIF (CRYPTO_FOUND)
  ENDIF (APPLE)
ENDIF (OPENSSL_FOUND)

IF (HAVE_CRYPT)
  TARGET_LINK_LIBRARIES(ratbox crypt)
ENDIF (HAVE_CRYPT)


SET_TARGET_PROPERTIES(ratbox PROPERTIES VERSION 4.2.0 SOVERSION 4)
INSTALL(TARGETS ratbox DESTINATION lib)

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/librb-config.h.cmake
               ${PROJECT_BINARY_DIR}/librb-config.h)

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/libratbox_config.h.cmake
               ${PROJECT_BINARY_DIR}/libratbox_config.h)
