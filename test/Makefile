#!/usr/bin/make -f
# SPDX-License-Identifier: MIT
# -*- makefile -*-
#
# Minimal AVL-tree helper functions test
#
# SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>

TESTS_CXX_COMPATIBLE = \
 containerof \
 avl_entry \
 avl_init-explicit \
 avl_init-local \
 avl_init-global \
 avl_link_node \
 avl_erase_node \
 avl_first \
 avl_last \
 avl_next \
 avl_prev \
 avl_link_node-prioqueue \
 avl_erase_node-prioqueue \
 avl_insert_balance \
 avl_erase \
 avl_insert-prioqueue \
 avl_erase-prioqueue \

TESTS_C_ONLY = \

TESTS_ALL = $(TESTS_CXX_COMPATIBLE) $(TESTS_C_ONLY)

# tests flags and options
CFLAGS += -g3 -pedantic -Wall -W -Werror -MD -MP
ifeq ("$(BUILD_CXX)", "1")
	CFLAGS += -std=c++98
	TESTS = $(TESTS_CXX_COMPATIBLE)
	COMPILER_NAME=$(CXX)
else
	CFLAGS += -std=c99
	TESTS += $(TESTS_ALL)
	COMPILER_NAME=$(CC)
endif

# disable verbose output
ifneq ($(findstring $(MAKEFLAGS),s),s)
ifndef V
	Q_CC = @echo '    $(COMPILER_NAME)' $@;
	Q_LD = @echo '    $(COMPILER_NAME)' $@;
	export Q_CC
	export Q_LD
endif
endif

# standard build tools
CC ?= gcc
RM ?= rm -f
ifeq ("$(BUILD_CXX)", "1")
COMPILE.c = $(Q_CC)$(CXX) -x c++ $(CFLAGS) $(CXXLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
LINK.o = $(Q_LD)$(CXX) $(CFLAGS) $(CXXLAGS) $(LDFLAGS) $(TARGET_ARCH)
else
COMPILE.c = $(Q_CC)$(CC) -x c $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
LINK.o = $(Q_LD)$(CC) $(CFLAGS) $(LDFLAGS) $(TARGET_ARCH)
endif

TESTS_OK = $(TESTS:=.ok)

# default target
all: $(TESTS_OK)

$(TESTS_OK): %.ok: %
	@echo "T:  $(COMPILER_NAME) $(TESTRUN_NAME) $<"
	@$(TESTRUN_WRAPPER) ./$<
	@touch $@

# standard build rules
.SUFFIXES: .o .c
.c.o:
	$(COMPILE.c) -o $@ $<

avltree.o: ../avltree.c
	$(COMPILE.c) -o $@ $<

$(TESTS): %: %.o avltree.o
	$(LINK.o) $^ $(LDLIBS) -o $@

clean:
	@$(RM) $(TESTS_ALL) $(DEP) $(TESTS_OK) $(TESTS:=.o) $(TESTS:=.d) avltree.o avltree.d

# load dependencies
DEP = $(TESTS:=.d) avltree.d
-include $(DEP)

.PHONY: all clean
