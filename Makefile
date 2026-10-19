# Contract J.A.C.K., built from source.
#
#   make          build the game into out/cjack-legacy-release/ (and its -private / -public .tar.gz)
#   make modern   the game with CJACK_MODERN, in out/cjack-modern-release/
#   make progress how much of the build is byte-identical to the retail game;
#                 fails if a function recorded as identical stops matching
#   make help     everything else
#
# Needs only docker on the host and the three submodules (`git submodule
# update --init`): the compilers, toolchains/ (toolchains/README.md); rez/,
# which fetches the redump images of the European discs from archive.org
# and installs the game from them with the English 1.1 update (rez/README.md;
# `make retail`, which every build runs first, does it once); and tools/, the
# reconstruction tools. Every step runs in a throwaway container of the image
# docker/Dockerfile describes, with this checkout mounted at /repo, as you.

IMAGE     ?= cjacksrc-build
CONTAINER ?= cjacksrc-build
# Parallel compile jobs inside the build (build/build.mk); empty: one per CPU
JOBS ?=
# The game directory `make test` plays: the release build's, or a variant's
# (make test TEST_GAME=out/cjack-modern-release; build/variant.sh), and the
# input script (tools/playtest-*.txt; empty: no input, the main menu)
TEST_GAME ?= out/cjack-legacy-release
TEST_SCRIPT ?=
TEST_WORLD ?= -
TEST_NAME = $(if $(filter out/cjack-legacy-release,$(TEST_GAME:%/=%)),test,test-$(notdir $(TEST_GAME:%/=%)))

DOCKER_RUN = docker run --rm --init --name $(CONTAINER) \
	-u $(shell id -u):$(shell id -g) -e HOME=/tmp/home $(if $(JOBS),-e CJACKSRC_JOBS=$(JOBS)) \
	$(if $(CJACK_ARCHIVES),-e CJACK_ARCHIVES="$(CJACK_ARCHIVES)") \
	-v $(CURDIR):/repo -w /repo $(IMAGE)

TOOLCHAIN_FILES = toolchains/msvc-7.10-3077/bin/cl.exe toolchains/dxsdk-8.1/include/d3d8.h

.PHONY: all game debug modern modern-debug image retail check-rez check-tools check-toolchain \
	test mptest campaign control reference progress progress-update shell clean distclean help

all: game

help:
	@echo "make            build everything into out/cjack-legacy-release/ and its .tar.gz archives"
	@echo "make debug      the game with symbols and CJACK_DEBUG logging, in out/cjack-legacy-debug/"
	@echo "make modern     the game with CJACK_MODERN, in out/cjack-modern-release/"
	@echo "make modern-debug  both, in out/cjack-modern-debug/"
	@echo "                (none of these is ever scored)"
	@echo "make test       run out/cjack-legacy-release/ under Xvfb and Wine to its main menu"
	@echo "                (build/playtest.sh; TEST_WORLD=<world>, TEST_SCRIPT=<script>,"
	@echo "                TEST_GAME=out/cjack-modern-release; results in out/play/test*/)"
	@echo "make campaign   every single-player world in turn, played a minute, saved and loaded"
	@echo "                (build/campaign.sh; CAMPAIGN_WORLDS=, CAMPAIGN_CONTROL=cjack-control)"
	@echo "make control    out/cjack-control/: retail's game with our engine, the control"
	@echo "make mptest     the dedicated server hosts a deathmatch game and a client joins it,"
	@echo "                under Xvfb and Wine (build/mptest.sh; out/play/*-mp/)"
	@echo "make progress   score the build against the retail binaries; fails if a"
	@echo "                function recorded in progress/identical.txt stops matching"
	@echo "make progress-update  record the score in progress/ and README.md"
	@echo "make reference  extract the retail binaries and their function tables into out/reference/"
	@echo "make retail     fetch the two disc images and install the retail game and its"
	@echo "                1.1 update from them, and fetch the USA release's two too, for"
	@echo "                the public archive (rez/, once; every build runs it); check it"
	@echo "make shell      a shell in the build container (build/*.sh, tools/ work there)"
	@echo "make image      (re)build the docker image $(IMAGE)"
	@echo "make clean      remove out/ and every build output in the source tree"
	@echo "make distclean  clean, and remove the docker image too"
	@echo
	@echo "Variables: JOBS=<n> (parallel compiles; default one per CPU)"
	@echo "           CJACK_ARCHIVES=\"private public\" (the archives build/package.sh writes)"
	@echo "           IMAGE_BUILD=0 (use the docker image as it is, as CI does)"
	@echo "           COMPLETE=1, COVERED=1, ACCEPT_LOSSES=1 (progress/README.md)"

game: check-toolchain retail
	$(DOCKER_RUN) bash build/all.sh

debug modern modern-debug: check-toolchain retail
	$(DOCKER_RUN) bash build/variant.sh $@

# IMAGE_BUILD=0: use $(IMAGE) as it is (CI builds it with a layer cache
# first, .github/workflows/public-archives.yml).
IMAGE_BUILD ?= 1
image:
ifeq ($(IMAGE_BUILD),0)
	@docker image inspect $(IMAGE) >/dev/null 2>&1 || \
	  { echo "error: IMAGE_BUILD=0, but there is no docker image $(IMAGE)" >&2; exit 1; }
else
	@docker build -q -t $(IMAGE) docker/ >/dev/null || \
	  { echo "error: could not build the docker image (docker/Dockerfile)" >&2; exit 1; }
endif
	@echo "== docker image $(IMAGE) ready"

# The retail game the build takes its data from (build/retail.sh): installed
# by rez/ the first time, checked against rez/games.sha256 every time; and
# the USA release's disc images, which the public archive's install scripts
# read too.
retail: check-rez check-tools image
	@$(DOCKER_RUN) bash -c 'source build/retail.sh && retail_install && retail_discs && retail_verify'

reference: retail
	$(DOCKER_RUN) bash build/reference.sh

check-rez:
	@[ -f rez/install-games.sh ] || { echo "error: rez/ is empty -- the disc images and their installer are the rez/ submodule: run git submodule update --init (see rez/README.md)" >&2; exit 1; }

check-tools:
	@[ -f tools/progress.py ] || { echo "error: tools/ is empty -- the reconstruction tools are the tools/ submodule: run git submodule update --init" >&2; exit 1; }

check-toolchain:
	@for f in $(TOOLCHAIN_FILES); do \
	  [ -f "$$f" ] || { echo "error: $$f is missing -- the compilers are the toolchains/ submodule: run git submodule update --init (see toolchains/README.md)" >&2; exit 1; }; \
	done

test: image
	@[ -f $(TEST_GAME)/Lithtech.exe ] || { echo "error: no $(TEST_GAME)/ -- build it first (make, or make debug/modern/modern-debug)" >&2; exit 1; }
	$(DOCKER_RUN) env PLAYTEST_NAME=$(TEST_NAME) $(if $(TEST_SCRIPT),PLAYTEST_SCRIPT=$(TEST_SCRIPT)) bash build/playtest.sh $(TEST_GAME) '$(TEST_WORLD)' $(if $(TEST_SCRIPT),5,60)

mptest: image
	@[ -f $(TEST_GAME)/ContractJackServer.exe ] || { echo "error: no $(TEST_GAME)/ -- build it first" >&2; exit 1; }
	$(DOCKER_RUN) env MPTEST_TYPE=$(or $(MPTEST_TYPE),dm) $(if $(MPTEST_CDKEY),MPTEST_CDKEY=$(MPTEST_CDKEY)) bash build/mptest.sh $(TEST_GAME)

# The single-player campaign level by level (build/campaign.sh): every world,
# or CAMPAIGN_WORLDS="M01S01 M04S02 ..."; CAMPAIGN_CONTROL=cjack-control
# compares each world's shots with the control's run of it.
campaign: image
	@[ -f $(TEST_GAME)/Lithtech.exe ] || { echo "error: no $(TEST_GAME)/ -- build it first" >&2; exit 1; }
	$(DOCKER_RUN) env CAMPAIGN_CONTROL=$(CAMPAIGN_CONTROL) bash build/campaign.sh $(TEST_GAME) $(CAMPAIGN_WORLDS)

# The control for the runtime tests (build/control.sh): retail's game with
# our engine, in out/cjack-control/ (TEST_GAME=out/cjack-control).
control:
	@bash build/control.sh

progress: retail
	$(DOCKER_RUN) env COMPLETE=$(COMPLETE) COVERED=$(COVERED) bash build/progress.sh check

progress-update: retail
	$(DOCKER_RUN) env ACCEPT_LOSSES=$(ACCEPT_LOSSES) bash build/progress.sh update

shell: image
	docker run --rm -it --init --name $(CONTAINER)-shell \
	  -u $(shell id -u):$(shell id -g) -e HOME=/tmp/home \
	  -v $(CURDIR):/repo -w /repo $(IMAGE) bash

clean:
	rm -rf out .wine .build
	find src -depth -type d \( -name built -o -name Built \) -exec rm -rf {} +

distclean: clean
	-docker rmi $(IMAGE)
