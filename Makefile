#makefile for BUGS I - SRN 7/12/91 
# modified for Linux on 6/15/92 Ken Corey

PROG = bugs

CC       ?= gcc
CFLAGS   ?= -O3
CFLAGS	 += -std=c99 -Wall -Wextra
# ncurses through pkg-config when it is there, plain -lncurses otherwise
CURSES_CFLAGS ?= $(shell pkg-config --cflags ncurses 2>/dev/null)
CURSES_LIBS   ?= $(shell pkg-config --libs ncurses 2>/dev/null || echo -lncurses)
CFLAGS   += $(CURSES_CFLAGS)
CPPFLAGS += -DLINUX
# the same values ncurses' pkg-config file sets, so the two agree
CPPFLAGS += -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600
LIBS     += $(CURSES_LIBS)

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
DEPS = info.h

INSTALL      = install
INSTALL_BIN  = $(INSTALL) -D -m 755

PREFIX    = /usr/local
BIN_DIR   = $(PREFIX)/bin

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LIBS)

%.o: %.c $(DEPS)

bugs.o: bugs.h

install: all
	$(INSTALL_BIN) $(PROG) $(BIN_DIR)/$(PROG)
.PHONY: install

uninstall:
	$(RM)    $(BIN_DIR)/$(PROG)
.PHONY: uninstall
clean: 
	$(RM) $(PROG) $(OBJS)
.PHONY: clean
