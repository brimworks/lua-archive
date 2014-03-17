#!/usr/bin/env lua

package = 'lua-archive'
version = 'git-1'
source = {
   url = 'git://github.com/brimworks/lua-archive'
}

description = {
   summary = "libarchive integration for Lua",
   detailed = [[
       lua-archive is a light wrapper for the libarchive library so
       all archive file types supported by libarchive can be processed
       in Lua.
   ]],
   homepage = "https://github.com/brimworks/lua-archive/",
   license = "MIT"
}
dependencies = {
   "lua >= 5.1"
}

build = {
   type = "cmake",
   variables = {
      INSTALL_CMOD = "$(LIBDIR)",
      CMAKE_BUILD_TYPE = "$(CMAKE_BUILD_TYPE)",
   }
}
