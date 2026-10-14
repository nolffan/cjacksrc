# build/build.mk -- every compile and link of the game, as one Makefile.
#
# The workspaces (VC6-style .dsw/.dsp, written for this project: Monolith's
# own Visual Studio .NET 2003 solutions are lost) as Developer Studio would
# build them: tools/dsp2mk.py reads each workspace and its project files and
# writes the rules (.build/*.mk) -- every compile with the flags its
# project's configuration gives it ("Final"; a library's "Release" where it
# has no "Final"), one source per compile, every library and link, the
# custom build steps. `make -j` then runs the compiles of every binary at
# once, and a rebuild redoes only what changed: an object is rebuilt when its
# source, a header it includes (tools/mkdeps.py finds them), or its command
# line changes (the line is kept in <object>.cmd), a binary when any of its
# inputs does.
#
# It is run from the root of the tree being built, never from a checkout:
# build/compile.sh runs it in out/release-tree/ (src/ without its variant
# blocks, build/env.sh) and build/variant.sh in out/build-<variant>/, as
#
#   make -f build/build.mk -j<N> [target...]
#
# Targets: all (the default: every workspace's wanted projects, below); a
# project by its workspace name; any single output.
#
# The compilers see the tree's src/ as drive S: and toolchains/ as T:, so
# they see the same paths wherever the tree is. (Monolith's own build ran
# in C:\proj\to2\source\, as the retail images' strings show.)
#
# From the environment (build/variant.sh sets them; empty for the release
# build, whose command lines are then exactly the project files' ones):
#   CJACKSRC_CFLAGS   appended to every compile (/D CJACK_MODERN, /Z7 ...)
#   CJACKSRC_LDFLAGS  put first on every link
#   CJACKSRC_RCFLAGS  put first on every resource compile

ROOT := $(CURDIR)
SHELL := /bin/bash
.SUFFIXES:
.DELETE_ON_ERROR:
MAKEFLAGS += --no-builtin-rules --no-builtin-variables
.DEFAULT_GOAL := all

ifeq ($(wildcard $(ROOT)/src/Engine),)
$(error run build/build.mk from the root of a tree, as build/compile.sh does)
endif

empty :=
space := $(empty) $(empty)
comma := ,
tab := $(empty)	$(empty)
define nl


endef
# one shell word per make word, quoted: Windows paths keep their backslashes
q = $(foreach w,$(1),'$(subst ','\'',$(w))')

CJACKSRC_CFLAGS  ?=
CJACKSRC_LDFLAGS ?=
CJACKSRC_RCFLAGS ?=

# The workspaces, each with the projects of it the game needs (and,
# through the workspace, every library they depend on). A workspace that is
# not in the tree yet is skipped: the game is built up one binary at a time
# (docs/PLAN.md), and each binary's target in progress/targets.py is
# optional until its workspace is here.
#   src/Engine/ltmsg/ltmsg.dsw         ltmsg.dll, the engine's messages
#   src/Engine/snddrv/snddrv.dsw       SndDrv.dll, the sound driver, with
#                                      EAX's GUIDs (eaxguid), which the
#                                      retail Rich header says VC6's cl
#                                      12.00.8168 compiled: CL_eaxguid
#   src/Game/Libs/GameSpy/GameSpy.dsw  GameSpy.dll, Monolith's wrapper
#                                      around GameSpy's SDK, which the game
#                                      DLLs import (so it comes first)
#   src/Game/TO2.dsw                   the game DLLs, the launcher and the
#                                      dedicated server
#   src/Engine/runtime/winbuild/Engine.dsw   the engine
WORKSPACES := \
    src/Engine/ltmsg/ltmsg.dsw:ltmsg \
    src/Engine/snddrv/snddrv.dsw:snddrv \
    src/Game/Libs/GameSpy/GameSpy.dsw:GameSpy \
    src/Game/TO2.dsw:ClientShellDLL,Object,ClientFxDLL,ClientRes,ServerRes,ServerApp,Launcher \
    src/Engine/runtime/winbuild/Engine.dsw:Lithtech,server

# ---------------------------------------------------------------------------
# Wine and the toolchains
export WINEDEBUG := -all
export WINEDLLOVERRIDES := mscoree,mshtml=
WINE := wine
PFX := $(ROOT)/.wine/drives

# Every retail binary's Rich header names cl 13.10.3077 and link 7.10.3077
# (progress/COMPILERS.md): msvc-7.10-3077 is that compiler, with its CRT,
# ATL/MFC 7.1 and Platform SDK. The DirectX 8.1 SDK comes before VC's own
# directories, where its setup puts it.
TC      := $(ROOT)/toolchains
VC71    := $(TC)/msvc-7.10-3077
DXSDK   := $(TC)/dxsdk-8.1
CL      := $(VC71)/bin/cl.exe
LINK    := $(VC71)/bin/link.exe
LIBEXE  := $(VC71)/bin/lib.exe
RC      := $(VC71)/bin/rc.exe
CL_eaxguid := $(TC)/msvc-6.00-8168/BIN/cl.exe

# Windows paths: src/ is S:, toolchains/ T:, the rest Z:
wz = z:$(subst /,\,$(1))
ws = $(if $(filter $(ROOT)/src $(ROOT)/src/%,$(1)),s:$(subst /,\,$(patsubst $(ROOT)/src%,%,$(1))),$(if $(filter $(TC) $(TC)/%,$(1)),t:$(subst /,\,$(patsubst $(TC)%,%,$(1))),$(call wz,$(1))))

# The environment of a tool run. src/cjacksrc/ (cjacksrc_debug.h) is on
# every compiler's include path, last.
ENV_GAME := WINEPREFIX=$(PFX) WINEPATH='$(call ws,$(VC71))\bin' \
    INCLUDE='$(call ws,$(DXSDK))\include;$(call ws,$(VC71))\include;$(call ws,$(VC71))\atlmfc\include;$(call ws,$(VC71))\PlatformSDK\include;$(call ws,$(ROOT)/src/cjacksrc)' \
    LIB='$(call ws,$(DXSDK))\lib;$(call ws,$(VC71))\lib;$(call ws,$(VC71))\atlmfc\lib;$(call ws,$(VC71))\PlatformSDK\lib'

# The prefix: made the first time, then only checked; every tool run waits
# for it (an order-only prerequisite, so it causes no rebuild).
$(PFX)/.ready:
	@echo "   WINE prefix $(@D)"
	@mkdir -p $(@D) && export WINEPREFIX=$(@D) && { $(WINE) wineboot -i >/dev/null 2>&1; \
	  for v in TEMP TMP; do $(WINE) reg add 'HKCU\Environment' /v $$v /t REG_SZ /d 'C:\windows\temp' /f >/dev/null 2>&1; done; \
	  wineserver -w; } && test -f $(@D)/system.reg && touch $@
.PHONY: wine-drives FORCE
wine-drives: $(PFX)/.ready
	@ln -sfn $(ROOT)/src $(PFX)/dosdevices/s:
	@ln -sfn $(TC) $(PFX)/dosdevices/t:

# ---------------------------------------------------------------------------
# The machinery
#
# Every output's command is a variable, cmd_<output>, that the rule runs and
# keeps in <output>.cmd. An output whose command differs from the kept one
# is rebuilt (FORCE): a changed flag, a file added to a link.
cmd_changed = $(call str_ne,$(strip $(cmd_$(1))),$(strip $(file <$(1).cmd)),FORCE)
str_ne = $(if $(subst x$(1),,x$(2))$(subst x$(2),,x$(1)),$(3))

OBJECTS :=
DEPSPEC :=
TOOLS :=

# $(call compile,<object>,<working dir>,<source as spelled>,<env>,<exe>,<flags>,<prefix>)
# One compile: `<exe> <flags> <source> $(CJACKSRC_CFLAGS)` in <working dir>
# (the variant flags last: /Oy- must follow the /O2 that implies /Oy).
define compile
OBJECTS += $(1)
cmd_$(1) := cd $(2) && $(4) $(WINE) $(5) $(call q,$(6) $(3) $(CJACKSRC_CFLAGS))
DEPSPEC += $(subst $(space),|,$(1)$(tab)$(2)$(tab)$(3)$(tab)$(strip $(6)))
$(1): $$(call cmd_changed,$(1)) | $(7)
endef

# $(call tool,<output>,<prerequisites>,<command>): any other build step
define tool
cmd_$(1) := $(3)
$(1): $$(call cmd_changed,$(1)) $(2)
TOOLS += $(1)
endef

# $(call rcfile,<res>,<project dir>,<rc as spelled>,<rc in the tree>,<flags>)
# What the script can include -- the files in its directory and in each /i
# directory, what it #includes and rcincludes, the bitmaps, icons and other
# files its resources name, recursively -- tools/mkdeps.py finds, as it
# finds a compile's headers.
define rcfile
DEPSPEC += $(subst $(space),|,$(1)$(tab)$(2)$(tab)$(3)$(tab)$(strip $(CJACKSRC_RCFLAGS) $(5)))
$(call tool,$(1),$(4) | wine-drives,\
    cd $(2) && $(ENV_GAME) $(WINE) $(RC) $(CJACKSRC_RCFLAGS) $(call q,$(5) $(3)))
endef

# $(call library,<lib>,<project dir>,<flags>,<objects>,<objects as the project dir sees them>)
# $(call library,<lib>,<project dir>,<flags>,<objects>,<objects as the project dir sees them>)
define library
$(call tool,$(1),$(4) | wine-drives,\
    cd $(2) && { printf '%s\n' $(call q,$(3) $(subst /,\,$(5))) > $(1:%=$(ROOT)/%).rsp; } && \
    $(ENV_GAME) $(WINE) $(LIBEXE) '@$(call ws,$(ROOT)/$(1)).rsp')
endef

# $(call binary,<dll or exe>,<project dir>,<flags>,<prerequisites>,<inputs as the project dir sees them>,
#   <the libraries' precompiled-header objects, as the project dir sees them>)
# Through a response file, as Developer Studio links; with a map beside it.
# Compiled with /Z7 (the debug builds), the libraries' precompiled-header
# objects are linked too: the others refer to the type information in them
# (LNK1211 without).
Z7 := $(findstring /Z7,$(CJACKSRC_CFLAGS))
define binary
$(call tool,$(1),$(4) | wine-drives,\
    cd $(2) && { printf '%s\n' $(call q,$(CJACKSRC_LDFLAGS) $(3) /map:$(call ws,$(ROOT)/$(basename $(1))).map $(subst /,\,$(5) $(if $(Z7),$(6)))) > $(1:%=$(ROOT)/%).rsp; } && \
    $(ENV_GAME) $(WINE) $(LINK) '@$(call ws,$(ROOT)/$(1)).rsp')
endef

# $(call custom,<outputs>,<input>,<project dir>,<commands>): a source's
# custom build step (flex, bison), its commands run by cmd.exe
define custom
$(1) &: $(2) | wine-drives
	@echo "   GEN  $(1)"
	@cd $(3) && $(ENV_GAME) $(WINE) cmd /c $(call q,$(4)) >/dev/null
	@# (bison names its output in lower case, and the step's own
	@# `ren AnimationParse.o AnimationParse.o` that fixes it on Windows changes
	@# no case under Wine: give each output the name the step gives it)
	@for o in $(1); do [ -f "$$$$o" ] || { f=$$$$(find "$$$$(dirname $$$$o)" -maxdepth 1 -iname "$$$$(basename $$$$o)" | head -1); \
	  [ -n "$$$$f" ] && mv "$$$$f" "$$$$o"; }; test -f "$$$$o" || { echo "error: $$$$o was not made" >&2; exit 1; }; done
endef

# ---------------------------------------------------------------------------
# The projects (tools/dsp2mk.py)

DEPDIR := $(ROOT)/.build
$(shell mkdir -p $(DEPDIR))
PROJECTS :=
WANTED :=
# $(call workspace,<dsw>,<projects, comma-separated>): its rules, from
# tools/dsp2mk.py, which gets the projects earlier workspaces already build
# (--have), so that a library two workspaces share is built once.
define workspace
ifneq ($(wildcard $(ROOT)/$(1)),)
DSP_OK := $$(shell python3 tools/dsp2mk.py --have "$$(PROJECTS)" $(1) Final $(DEPDIR)/$(basename $(notdir $(1))).mk $(subst $(comma), ,$(2)) 2>$(DEPDIR)/dsp2mk-$(basename $(notdir $(1))).log && echo ok)
ifneq ($$(DSP_OK),ok)
$$(error tools/dsp2mk.py failed on $(1): $$(file <$(DEPDIR)/dsp2mk-$(basename $(notdir $(1))).log))
endif
include $(DEPDIR)/$(basename $(notdir $(1))).mk
WANTED += $(subst $(comma), ,$(2))
endif
endef
$(foreach w,$(WORKSPACES),$(eval $(call workspace,$(firstword $(subst :, ,$(w))),$(lastword $(subst :, ,$(w))))))

.PHONY: all game
all game: $(WANTED)

# ---------------------------------------------------------------------------
# The rules all compiles and tools share

dups := $(strip $(foreach o,$(sort $(OBJECTS)),$(if $(word 2,$(filter $(o),$(OBJECTS))),$(o))))
ifneq ($(dups),)
$(error two sources compile to one object: $(dups))
endif

# Under Wine, a compile that uses a precompiled header now and then dies
# with C1001 (an internal compiler error) or C1060 ("out of heap space")
# while other compiles run; the same compile run again succeeds, and gives
# the same object. Those two are retried, up to three tries in all (said so
# in the log); any other error fails the build at once.
$(OBJECTS):
	@echo "   CC   $@"
	@mkdir -p $(@D) && rm -f $@
	@for try in 1 2 3; do \
	  out=$$( { $(cmd_$@); } 2>&1 ); rc=$$?; \
	  if [ $$rc = 0 ] || [ $$try = 3 ] || ! grep -qE 'error C10(01|60)' <<< "$$out"; then printf '%s\n' "$$out"; break; fi; \
	  echo "   CC   $@: $$(grep -oE 'error C10(01|60)' <<< "$$out" | head -1), compiling it again"; rm -f $@; \
	done; exit $$rc
	@test -f $@ || { echo "error: $@ was not built" >&2; exit 1; }
	@printf '%s\n' '$(subst ','\'',$(cmd_$@))' > $@.cmd

$(TOOLS):
	@echo "   GEN  $@"
	@mkdir -p $(@D) && rm -f $@
	@$(cmd_$@)
	@test -f $@ || { echo "error: $@ was not built" >&2; exit 1; }
	@printf '%s\n' '$(subst ','\'',$(cmd_$@))' > $@.cmd

FORCE:

# Header dependencies (tools/mkdeps.py), brought up to date on every run
$(file >$(DEPDIR)/deps.spec,$(subst |, ,$(subst $(space),$(nl),$(DEPSPEC))))
DEPS_OK := $(shell python3 tools/mkdeps.py $(DEPDIR)/deps.spec $(DEPDIR)/deps.mk && echo ok)
ifneq ($(DEPS_OK),ok)
$(error tools/mkdeps.py failed)
endif
include $(DEPDIR)/deps.mk
