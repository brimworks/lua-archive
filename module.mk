_pwd := $(pwd)

include $(make-common.dir)/tool/cc.mk
include $(make-common.dir)/tool/lua.mk
include $(make-common.dir)/layout.mk

_lib  := $(lua.c.lib.dir)/archive.so
_objs := $(call cc.c.to.o,$(addprefix $(_pwd)/, \
    ar.c \
    ar_write.c \
    ar_registry.c \
    ar_read.c \
    ar_entry.c \
))

all: | $(_lib)
$(_lib): cc.libs += lua archive
$(_lib): cc.objs := $(_objs)
$(_lib): $(_objs)
	$(cc.so.rule)

# How to run ar tests:
.PHONY: ar.test
test: | ar.test

lua ?= lua
ar.test: | $(lua.c.lib.dir)/archive.so
ar.test: lua.path += $(_pwd)
ar.test: $(wildcard $(_pwd)/test*)
	@mkdir -p $(tmp.dir)
	cd $(tmp.dir); for t in $<; do \
		echo "TESTING: $$t"; \
		env -i $(lua.run) $(lua) $$t; \
	done
