
PROJECT(bandb-tools)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE_DIRECTORIES(${bluebox_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${bluebox_BINARY_DIR})
INCLUDE_DIRECTORIES(${libratbox_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${libratbox_BINARY_DIR})
INCLUDE_DIRECTORIES(${bandb-tools_SOURCE_DIR}/sqlite3)

ADD_SUBDIRECTORY(sqlite3)

ADD_EXECUTABLE(bandb bandb.c rsdb_sqlite3.c rsdb_snprintf.c)
ADD_DEPENDENCIES(bandb sqlite3 ratbox)
TARGET_LINK_LIBRARIES(bandb sqlite3 ratbox pthread)

ADD_EXECUTABLE(bantool bantool.c rsdb_sqlite3.c rsdb_snprintf.c)
ADD_DEPENDENCIES(bantool sqlite3 ratbox)
TARGET_LINK_LIBRARIES(bantool sqlite3 ratbox pthread)

SET_TARGET_PROPERTIES(bantool PROPERTIES VERSION 4.2.0 SOVERSION 4)
INSTALL(TARGETS bantool DESTINATION lib)

SET_TARGET_PROPERTIES(bandb PROPERTIES VERSION 4.2.0 SOVERSION 4)
INSTALL(TARGETS bandb DESTINATION lib)

ADD_CUSTOM_TARGET(bandb-tools ALL)
ADD_DEPENDENCIES(bandb-tools sqlite3 bandb bantool)
