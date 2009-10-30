_pwd := $(pwd)

include $(make-common.dir)/tool/cc.mk
include $(make-common.dir)/tool/lua.mk
include $(make-common.dir)/layout.mk

_lib  := $(lua.c.lib.dir)/archive.so
_objs := $(call cc.c.to.o,$(addprefix $(_pwd)/, \
    lua_archive.c \
    lua_archive_write.c \
    lua_archive_registry.c \
    lua_archive_read.c \
    lua_archive_entry.c \
))

all: | $(_lib)
$(_lib): cc.libs += lua archive
$(_lib): cc.objs := $(_objs)
$(_lib): $(_objs)
	$(cc.so.rule)

# How to run lua_archive tests:
.PHONY: lua_archive.test
test: | lua_archive.test

lua ?= lua
lua_archive.test: | $(lua.c.lib.dir)/archive.so
lua_archive.test: lua.path += $(_pwd)
lua_archive.test: $(wildcard $(_pwd)/test*)
	@mkdir -p $(tmp.dir)
	cd $(tmp.dir); for t in $<; do \
		echo "TESTING: $$t"; \
		env -i $(lua.run) $(lua) $$t; \
	done
