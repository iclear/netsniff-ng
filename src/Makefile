# netsniff-ng build system, part of netsniff-ng.
# Copyright 2012 Daniel Borkmann <borkmann@gnumaniacs.org>
# Subject to the GNU GPL, version 2.

VERSION = 0
PATCHLEVEL = 5
SUBLEVEL = 8
EXTRAVERSION = -rc0

TOOLS = netsniff-ng trafgen astraceroute flowtop ifpps mausezahn bpfc curvetun

# For packaging purposes, prefix can define a different path.
PREFIX ?=

# Debugging option
ifeq ("$(origin DEBUG)", "command line")
  DEBUG := 1
else
  DEBUG := 0
endif

# Disable if you don't want it
CCACHE = ccache

# Location of installation paths.
BINDIR = $(PREFIX)/usr/bin
SBINDIR = $(PREFIX)/usr/sbin
INCDIR = $(PREFIX)/usr/include
ETCDIR = $(PREFIX)/etc
ETCDIRE = $(ETCDIR)/netsniff-ng
DOCDIR = $(PREFIX)/usr/share/doc
DOCDIRE = $(DOCDIR)/netsniff-ng

# Shut up make, helper warnings, parallel compilation!
MAKEFLAGS += --no-print-directory
MAKEFLAGS += -rR
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --jobs=$(shell grep "^processor" /proc/cpuinfo | wc -l)

# For packaging purposes, you might want to disable O3+arch tuning
CFLAGS = -fstack-protector
ifeq ($(DEBUG), 1)
  CFLAGS += -g
  CFLAGS += -O2
else
  CFLAGS += -march=native
  CFLAGS += -mtune=native
  CFLAGS += -O3
  CFLAGS += -fpie
  CFLAGS += -pipe
  CFLAGS += -fomit-frame-pointer
endif
CFLAGS += --param=ssp-buffer-size=4
CFLAGS += -fno-strict-aliasing
CFLAGS += -fexceptions
CFLAGS += -fasynchronous-unwind-tables
CFLAGS += -fno-delete-null-pointer-checks
CFLAGS += -D_FORTIFY_SOURCE=2
CFLAGS += -D_REENTRANT
CFLAGS += -D_FILE_OFFSET_BITS=64
CFLAGS += -D_LARGEFILE_SOURCE
CFLAGS += -D_LARGEFILE64_SOURCE
CFLAGS += -D__WITH_HARDWARE_TIMESTAMPING=1
CFLAGS += -DVERSION_STRING=\"$(VERSION_STRING)\"
CFLAGS += -std=gnu99

# Should be commented in after the mausezahn mess is cleaned
WFLAGS  = -Wall
#WFLAGS += -Wmissing-prototypes
#WFLAGS += -Wdeclaration-after-statement
#WFLAGS += -Werror-implicit-function-declaration
#WFLAGS += -Wundef
#WFLAGS += -Wstrict-prototypes
#WFLAGS += -Wunused-but-set-variable
#WFLAGS += -Werror
WFLAGS += -Wformat=2

CFLAGS += $(WFLAGS) -I.
CPPFLAGS =
ifeq ("$(origin CROSS_LD_LIBRARY_PATH)", "command line")
  LDFLAGS = -L$(CROSS_LD_LIBRARY_PATH)
else
  LDFLAGS =
endif

ALL_CFLAGS = $(CFLAGS)
ALL_LDFLAGS = $(LDFLAGS)
TARGET_ARCH =
LEX_FLAGS =
YAAC_FLAGS =

Q = @

LD = $(Q)echo -e "  LD\t$@" && $(CCACHE) $(CROSS_COMPILE)gcc
CCNQ = $(CCACHE) $(CROSS_COMPILE)gcc
CC = $(Q)echo -e "  CC\t$<" && $(CCNQ)
MKDIR = $(Q)echo -e "  MKDIR\t$@" && mkdir
ifeq ($(DEBUG), 1)
  STRIP = $(Q)true
else
  STRIP = $(Q)echo -e "  STRIP\t$@" && $(CROSS_COMPILE)strip
endif
LEX = $(Q)echo -e "  LEX\t$<" && flex
YAAC = $(Q)echo -e "  YAAC\t$<" && bison
INST = echo -e "  INST\t$(1)" && install -d $(2) && \
	install --mode=644 -DC $(1) $(2)/$(shell basename $(1))
ifeq ("$(origin PREFIX)", "command line")
  INSTX = echo -e "  INST\t$(1)" && install -d $(2) && \
	install -C $(1) $(2)/$(shell basename $(1))
else
  INSTX = echo -e "  INST\t$(1)" && install -C $(1) $(2)/$(shell basename $(1))
endif
RM = echo -e "  RM\t$(1)" && rm -rf $(1)
RMDIR = echo -e "  RM\t$(1)" && rmdir --ignore-fail-on-non-empty $(1) 2> /dev/null || true

export VERSION PATCHLEVEL SUBLEVEL EXTRAVERSION
export CROSS_COMPILE

VERSION_STRING = $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)
VERSION_SHORT = $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)

bold = $(shell tput bold)
normal = $(shell tput sgr0)

# Build NaCl if no environment variables are set!
ifndef NACL_LIB_DIR
ifndef NACL_INC_DIR
   $(info $(bold)NACL_LIB_DIR/NACL_INC_DIR is undefined, building libnacl with curvetun!$(normal))
   curvetun: nacl
endif
endif

ifeq ("$(origin CROSS_COMPILE)", "command line")
  WHAT := Cross compiling
else
  WHAT := Building
endif

build_showinfo:
	$(Q)echo "$(bold)$(WHAT) netsniff-ng toolkit ($(VERSION_STRING)) for" \
	      $(shell $(CCNQ) -dumpmachine)":$(normal)"
clean_showinfo:
	$(Q)echo "$(bold)Cleaning netsniff-ng toolkit ($(VERSION_STRING)):$(normal)"

%.yy.o: %.l
	$(LEX) -P $(shell perl -wlne 'print $$1 if /lex-func-prefix:\s([a-z]+)/' $<) \
	       -o $(BUILD_DIR)/$(shell basename $< .l).yy.c $(LEX_FLAGS) $<
%.tab.o: %.y
	$(YAAC) -p $(shell perl -wlne 'print $$1 if /yaac-func-prefix:\s([a-z]+)/' $<) \
		-o $(BUILD_DIR)/$(shell basename $< .y).tab.c $(YAAC_FLAGS) -d $<

.PHONY: all toolkit $(TOOLS) clean %_prehook %_distclean %_clean %_install
.FORCE:
.DEFAULT_GOAL := all
.DEFAULT:
.IGNORE: %_clean_custom %_install_custom
.NOTPARALLEL: $(TOOLS)

DOC_FILES = Summary RelatedWork Performance KnownIssues IPv6Notes Sponsors \
	    SubmittingPatches CodingStyle logo.png RelNotes/Notes-$(VERSION_SHORT)

NCONF_FILES = ether.conf tcp.conf udp.conf oui.conf

all: build_showinfo toolkit
toolkit: $(TOOLS)
allbutcurvetun: $(filter-out curvetun,$(TOOLS))
install: $(foreach tool,$(TOOLS),$(tool)_install)
	$(Q)$(foreach file,$(DOC_FILES),$(call INST,../Documentation/$(file),$(DOCDIRE));)
install_allbutcurvetun: $(foreach tool,$(filter-out curvetun,$(TOOLS)),$(tool)_install)
	$(Q)$(foreach file,$(DOC_FILES),$(call INST,../Documentation/$(file),$(DOCDIRE));)
clean mostlyclean: $(foreach tool,$(TOOLS),$(tool)_clean)
realclean distclean clobber: $(foreach tool,$(TOOLS),$(tool)_distclean)
	$(Q)$(foreach file,$(DOC_FILES),$(call RM,$(DOCDIRE)/$(file));)
	$(Q)$(call RMDIR,$(DOCDIRE))
	$(Q)$(call RMDIR,$(ETCDIRE))
mrproper: clean distclean

define TOOL_templ
  include $(1)/Makefile
  $(1) $(1)%: BUILD_DIR := $(1)
  $(1)_prehook:
	$(Q)echo "$(bold)$(WHAT) $(1):$(normal)"
  $(1): $(1)_prehook $$($(1)-lex) $$($(1)-yaac) $$(patsubst %.o,$(1)/%.o,$$($(1)-objs))
  $(1)_clean: $(1)_clean_custom
	$(Q)$$(call RM,$(1)/*.o $(1)/$(1))
  $(1)_install: $(1)_install_custom
	$(Q)$$(call INSTX,$(1)/$(1),$$(SBINDIR))
	$(Q)$$(call INST,../Documentation/$$(shell echo $(1) | sed 's/\([a-z]\)\(.*\)/\u\1\2/g'),$$(DOCDIRE))
  $(1)_distclean: $(1)_distclean_custom
	$(Q)$$(call RM,$$(SBINDIR)/$(1))
	$(Q)$$(call RM,$$(DOCDIRE)/$$(shell echo $(1) | sed 's/\([a-z]\)\(.*\)/\u\1\2/g'))
  $(1)/%.yy.o: $(1)/%.yy.c
	$$(CC) $$(ALL_CFLAGS) -o $$@ -c $$<
  $(1)/%.tab.o: $(1)/%.tab.c
	$$(CC) $$(ALL_CFLAGS) -o $$@ -c $$<
  $(1)/%.o: %.c
	$$(CC) $$(ALL_CFLAGS) -o $$@ -c $$<
endef

$(foreach tool,$(TOOLS),$(eval $(call TOOL_templ,$(tool))))

%:: ;

netsniff-ng: ALL_CFLAGS += -I$(INCDIR)/libnl3/ -D__WITH_PROTOS
trafgen: ALL_CFLAGS += -I.. -I$(INCDIR)/libnl3/ -D__WITH_PROTOS
bpfc: ALL_CFLAGS += -I..
curvetun: ALL_CFLAGS += -I NACL_INC_DIR
curvetun: ALL_LDFLAGS += -L NACL_LIB_DIR

bpfc_clean_custom:
	$(Q)$(call RM,$(BUILD_DIR)/*.h $(BUILD_DIR)/*.c)
trafgen_clean_custom:
	$(Q)$(call RM,$(BUILD_DIR)/*.h $(BUILD_DIR)/*.c)

netsniff-ng_distclean_custom flowtop_distclean_custom:
	$(Q)$(foreach file,$(NCONF_FILES),$(call RM,$(ETCDIRE)/$(file));)
	$(Q)$(call RMDIR,$(ETCDIRE))
astraceroute_distclean_custom:
	$(Q)$(call RM,$(ETCDIRE)/whois.conf)
	$(Q)$(call RM,$(SBINDIR)/astraceroute6)
	$(Q)$(call RMDIR,$(ETCDIRE))

netsniff-ng_install_custom flowtop_install_custom:
	$(Q)$(foreach file,$(NCONF_FILES),$(call INST,conf/$(file),$(ETCDIRE));)
astraceroute_install_custom:
	$(Q)$(call INST,conf/whois.conf,$(ETCDIRE))
	$(Q)$(call INSTX,../scripts/astraceroute6,$(SBINDIR))

$(TOOLS):
	$(LD) $(ALL_LDFLAGS) -o $@/$@ $@/*.o $($@-libs)
	$(STRIP) $@/$@

update:
	$(Q)../scripts/geoip-database-update
nacl:
	$(Q)echo "$(bold)$(WHAT) $@:$(normal)"
	$(Q)cd curvetun/ && ./build_nacl.sh ~/nacl
	$(Q)source ~/.bashrc
geoip:
	$(Q)echo "$(bold)$(WHAT) $@:$(normal)"
	$(Q)cd astraceroute/ && ./build_geoip.sh

help:
	$(Q)echo "$(bold)Available tools from the toolkit:$(normal)"
	$(Q)echo " <toolnames>:={$(TOOLS)}"
	$(Q)echo "$(bold)Targets for building the toolkit:$(normal)"
	$(Q)echo " all|toolkit                  - Build the whole toolkit"
	$(Q)echo " allbutcurvetun               - Build all except curvetun"
	$(Q)echo " <toolname>                   - Build only one of the tools"
	$(Q)echo "$(bold)Targets for cleaning the toolkit's build files:$(normal)"
	$(Q)echo " clean|mostlyclean            - Remove all build files"
	$(Q)echo " <toolname>_clean             - Remove only one of the tool's files"
	$(Q)echo "$(bold)Targets for installing the toolkit:$(normal)"
	$(Q)echo " install                      - Install the whole toolkit"
	$(Q)echo " <toolname>_install           - Install only one of the tools"
	$(Q)echo "$(bold)Targets for removing the toolkit:$(normal)"
	$(Q)echo " realclean|distclean|clobber  - Remove the whole toolkit from the system"
	$(Q)echo " <toolname>_distclean         - Remove only one of the tools"
	$(Q)echo "$(bold)Misc targets:$(normal)"
	$(Q)echo " mrproper                     - Remove build and install files"
	$(Q)echo " update                       - Update to the latest GeoIP database"
	$(Q)echo " nacl                         - Execute the build_nacl script"
	$(Q)echo " geoip                        - Execute the build_geoip script"
	$(Q)echo " help                         - Show this help"
	$(Q)echo "$(bold)Available parameters:$(normal)"
	$(Q)echo " DEBUG=1                      - Enable debugging"
	$(Q)echo " PREFIX=/path                 - Install path prefix"
	$(Q)echo " CROSS_COMPILE=/path-prefix   - Kernel-like cross-compiling prefix"
	$(Q)echo " CROSS_LD_LIBRARY_PATH=/path  - Library search path for cross-compiling"
