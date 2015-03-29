############################################################################
# apps/system/Makefile
#
#   Copyright (C) 2011-2014 Gregory Nutt. All rights reserved.
#   Author: Gregory Nutt <gnutt@nuttx.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 3. Neither the name NuttX nor the names of its contributors may be
#    used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
############################################################################

-include $(TOPDIR)/.config	# Current configuration
-include $(TOPDIR)/Make.defs

UTIL_APPS = init

ifeq ($(CONFIG_UTIL_CLE),y)
UTIL_APPS += cle
endif

ifeq ($(CONFIG_UTIL_INIFILE),y)
UTIL_APPS += inifile
endif

ifeq ($(CONFIG_UTIL_READLINE),y)
UTIL_APPS += readline
endif

ifeq ($(CONFIG_UTIL_NETLIB),y)
UTIL_APPS += netlib
endif

ifeq ($(CONFIG_UTIL_CODECS),y)
UTIL_APPS += codecs
endif

ifeq ($(CONFIG_UTIL_JSON),y)
UTIL_APPS += json
endif


# Sub-directories containing system tasks/libraries

SUBDIRS = cle inifile readline netlib init
BIN = libutil$(LIBEXT)
# Create the list of installed runtime modules (INSTALLED_DIRS)

ifeq ($(CONFIG_WINDOWS_NATIVE),y)
define ADD_DIRECTORY
  INSTALLED_DIRS  += $(if $(wildcard .\$1\Makefile),$1,)
endef
else
define ADD_DIRECTORY
  INSTALLED_DIRS  += $(if $(wildcard ./$1/Makefile),$1,)
endef
endif

$(foreach DIR, $(SUBDIRS), $(eval $(call ADD_DIRECTORY,$(DIR))))

all: $(BIN) 


.PHONY: nothing context depend clean distclean build

define SDIR_template
$(1)_$(2):
	$(Q) $(MAKE) -C $(1) $(2) TOPDIR="$(TOPDIR)" BIN="$(TOPDIR)$(DELIM)util$(DELIM)$(BIN)"
endef

$(foreach SDIR, $(INSTALLED_DIRS), $(eval $(call SDIR_template,$(SDIR),depend)))
$(foreach SDIR, $(INSTALLED_DIRS), $(eval $(call SDIR_template,$(SDIR),clean)))
$(foreach SDIR, $(INSTALLED_DIRS), $(eval $(call SDIR_template,$(SDIR),distclean)))
$(foreach SDIR, $(UTIL_APPS), $(eval $(call SDIR_template,$(SDIR),all)))

$(BIN): depend build

build: $(foreach SDIR, $(UTIL_APPS),$(SDIR)_all)

nothing:

install:

context:

depend: $(foreach SDIR, $(INSTALLED_DIRS), $(SDIR)_depend)

clean: $(foreach SDIR, $(INSTALLED_DIRS), $(SDIR)_clean)

distclean: $(foreach SDIR, $(INSTALLED_DIRS), $(SDIR)_distclean)
