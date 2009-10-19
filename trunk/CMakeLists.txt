
PROJECT(bluebox)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

SET(CMAKE_BUILD_TYPE "Debug")
#SET(CMAKE_BUILD_TYPE "Release")

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/include/setup.h.cmake ${PROJECT_BINARY_DIR}/setup.h)

ADD_SUBDIRECTORY(libratbox)
ADD_SUBDIRECTORY(ssld)

ADD_CUSTOM_TARGET(bluebox ALL)
ADD_DEPENDENCIES(bluebox libratbox ssld)
