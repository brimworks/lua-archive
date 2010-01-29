# Copyright (C) 2007-2009 LuaDist.
# Submitted by David Manura
# Redistribution and use of this file is allowed according to the
# terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own
# license.

PROJECT(lua-archive C)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

# Basic configurations
  SET(INSTALL_CMOD share/lua/cmod CACHE PATH "Directory to install Lua binary modules (configure lua via LUA_CPATH)")
# / configs

# Find libarchive
  FIND_LIBRARY (LIBARCHIVE_LIBRARY NAMES archive)
  FIND_PATH (LIBARCHIVE_INCLUDE_DIR archive.h
    PATH_SUFFIXES include/archive include
    ) # Find header
  INCLUDE(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(libarchive  DEFAULT_MSG  LIBARCHIVE_LIBRARY LIBARCHIVE_INCLUDE_DIR)
# / Find libarchive

# Find lua
  FIND_PACKAGE(Lua51 REQUIRED)
# / Find lua

# Define how to build archive.so:
  INCLUDE_DIRECTORIES(${LIBARCHIVE_INCLUDE_DIR} ${LUA_INCLUDE_DIR})
  ADD_LIBRARY(cmod_archive MODULE
    ar.c ar_write.c ar_registry.c ar_read.c ar_entry.c archive.def)
  SET_TARGET_PROPERTIES(cmod_archive PROPERTIES PREFIX "")
  SET_TARGET_PROPERTIES(cmod_archive PROPERTIES OUTPUT_NAME archive)
  TARGET_LINK_LIBRARIES(cmod_archive ${LUA_LIBRARY} ${LIBARCHIVE_LIBRARY})  
# / build archive.so

# Define how to test archive.so:
  INCLUDE(CTest)
  FIND_PROGRAM(LUA NAMES lua lua.bat)
  ADD_TEST(basic ${LUA} ${CMAKE_CURRENT_SOURCE_DIR}/test.lua ${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_CURRENT_BINARY_DIR}/)
# / test archive.so

# Where to install stuff
  INSTALL (TARGETS cmod_archive DESTINATION ${INSTALL_CMOD})
# / Where to install.
