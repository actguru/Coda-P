# Apple, Linux, Windows (has install issues) # 09/20/2026

VERSION = 3.1
MAJOR   = 3
COMPAT  = 3.1

LIBS = -lcoda-c

MACMIN  = 10.8

UNAME := $(shell uname)
CC    = gcc
ARCMD = ar q
OSCF  =
LIBSUF= so.$(VERSION)
LDL   =

ifeq ($(UNAME), Darwin)
$(info Make for APPLE, uname = $(UNAME))
CC = clang
ARCMD = libtool -static -o
OSCF = -mmacosx-version-min=$(MACMIN)
LIBSUF= $(MAJOR).dylib
ifeq ($(PREFIX),)
PREFIX := /usr/local
endif
else ifeq ($(UNAME), Linux)
$(info Make for LINUX, uname = $(UNAME))
LDL = -ldl
endif

ifeq ($(PREFIX),)
PREFIX := /usr
endif

$(info Library: libcoda-p   Verson $(VERSION), Major $(MAJOR), Compatible $(COMPAT), PREFIX $(PREFIX))

default: all

CFILES = coda-p.c

OFILES = $(CFILES:.c=.o)

CFLAGS = $(OSCF) -fPIC -std=gnu17 -g0 -O1 -Wall -Wfatal-errors

THELIB = libcoda-p

ARCHIVE = $(THELIB).$(LIBSUF)

$(ARCHIVE): $(OFILES)
ifeq ($(UNAME), Darwin)
	clang -dynamiclib -compatibility_version $(COMPAT) -current_version $(VERSION) \
	-install_name $(DESTDIR)$(PREFIX)/lib/$(ARCHIVE) -headerpad_max_install_names \
	-o $(ARCHIVE) $(OSCF) $(OFILES) $(LIBS)
	ln -s -f $(ARCHIVE) $(THELIB).dylib
else
	$(CC) -shared -o $(ARCHIVE) -Wl,-soname,$(THELIB).so.$(MAJOR) $(OFILES) $(LIBS)
	ln -s -f $(ARCHIVE) $(THELIB).so
	ln -s -f $(ARCHIVE) $(THELIB).so.$(MAJOR)
endif

UTIL = coda-list

$(UTIL): $(ARCHIVE)
	$(CC) $(CFLAGS) coda-list.c -o $(UTIL) -L. $(ARCHIVE) -lm $(LDL) $(LIBS)

MANFILE = coda-list.1coda-p

all: $(ARCHIVE) $(UTIL)

clean:
	rm -f $(OFILES) $(ARCHIVE) $(THELIB).dylib coda-list.o
	rm -f $(THELIB).so $(THELIB).so.$(MAJOR)
	rm -f $(UTIL) $(UTIL).o $(UTIL).exe

.PHONY:  all clean install

HEADER = coda-p.h

INSTALL ?= cp -a

MKDIR   ?= mkdir -p

# lib or lib64 -- linux
ifeq ($(LIB),)
LIB := lib
endif

install: $(ARCHIVE) $(HEADER) $(UTIL) $(MANFILE)
	$(MKDIR)              $(DESTDIR)$(PREFIX)/$(LIB)/
	$(INSTALL) $(ARCHIVE) $(DESTDIR)$(PREFIX)/$(LIB)/
	$(MKDIR)              $(DESTDIR)$(PREFIX)/include/
	$(INSTALL) $(HEADER)  $(DESTDIR)$(PREFIX)/include/
	$(MKDIR)              $(DESTDIR)$(PREFIX)/bin/
	$(INSTALL) $(UTIL)    $(DESTDIR)$(PREFIX)/bin/
	$(MKDIR)              $(DESTDIR)$(PREFIX)/share/man/man1/
	$(INSTALL) $(MANFILE) $(DESTDIR)$(PREFIX)/share/man/man1/
ifeq ($(UNAME), Darwin)
	$(INSTALL) $(THELIB).dylib       $(DESTDIR)$(PREFIX)/$(LIB)/
else
	$(INSTALL) $(THELIB).so          $(DESTDIR)$(PREFIX)/$(LIB)/
	$(INSTALL) $(THELIB).so.$(MAJOR) $(DESTDIR)$(PREFIX)/$(LIB)/
	# sudo ldconfig
endif

