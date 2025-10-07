#
# Makefile for GuppyScreen
#

# Configuration target - interactive menu to select build target
.PHONY: config
config:
	@bash scripts/config.sh

# Load build configuration
-include .config

# Ensure configuration exists for build targets
.config:
	@echo "============================================================"
	@echo "  No build configuration found!"
	@echo "  Please run 'make config' to select your target hardware."
	@echo "============================================================"
	@exit 1

# Default target if no configuration exists
.DEFAULT_GOAL := help

help:
	@echo "GuppyScreen Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  config    - Configure build target (interactive menu)"
	@echo "  build     - Build GuppyScreen for configured target"
	@echo "  clean     - Remove build artifacts"
	@echo "  install   - Install to system (requires root)"
	@echo ""
	@echo "Run 'make config' first to select your hardware target."

#===============================================================================
# Target Validation
#===============================================================================

# All build targets require .config
build default all install: .config

# Validate TARGET is set and valid
ifdef TARGET
  VALID_TARGETS := simulator pi k1 flashforge
  ifeq ($(filter $(TARGET),$(VALID_TARGETS)),)
    $(error Invalid TARGET: $(TARGET). Valid options: $(VALID_TARGETS))
  endif
endif

#===============================================================================
# Target-Specific Configuration
#===============================================================================

ifeq ($(TARGET),simulator)
  # ===========================================================================
  # SIMULATOR - macOS/Linux Development Build
  # ===========================================================================
  DEFINES += -D LV_BUILD_TEST=0 -D SIMULATOR

  # Platform-specific linking
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Darwin)
    # macOS: Static linking to avoid runtime dyld issues
    # Detect SDL2 location (Homebrew or system)
    SDL2_PREFIX := $(shell brew --prefix sdl2 2>/dev/null || echo "/usr/local")
    LDFLAGS ?= -lm libhv/lib/libhv.a spdlog/build/libspdlog.a wpa_supplicant/wpa_supplicant/libwpa_client.a \
               -lpthread -L$(SDL2_PREFIX)/lib -lSDL2 -framework Security -framework CoreFoundation
  else
    # Linux: Dynamic linking
    LDFLAGS ?= -lm -Llibhv/lib -Lspdlog/build -lhv -latomic -lpthread -Lwpa_supplicant/wpa_supplicant/ \
               -lwpa_client -lstdc++fs -lspdlog -lSDL2
  endif

else ifeq ($(TARGET),pi)
  # ===========================================================================
  # RASPBERRY PI / BTT PAD - Native ARM Build
  # ===========================================================================
  DEFINES += -D TARGET_PI
  # Native build on device (no cross-compilation)
  # Dynamic linking
  LDFLAGS ?= -lm -Llibhv/lib -Lspdlog/build -lhv -latomic -lpthread -Lwpa_supplicant/wpa_supplicant/ -lwpa_client -lstdc++fs -lspdlog

else ifeq ($(TARGET),k1)
  # ===========================================================================
  # CREALITY K1/MAX - MIPS Embedded Build
  # ===========================================================================
  CROSS_COMPILE = mips-linux-gnu-
  DEFINES += -D TARGET_K1
  LDFLAGS ?= -static -lm -Llibhv/lib -Lspdlog/build -l:libhv.a -latomic -lpthread -Lwpa_supplicant/wpa_supplicant/ -l:libwpa_client.a -lstdc++fs -l:libspdlog.a

else ifeq ($(TARGET),flashforge)
  # ===========================================================================
  # FLASHFORGE FF5M/PRO - ARM Embedded Build
  # ===========================================================================
  CROSS_COMPILE = arm-unknown-linux-gnueabihf-
  DEFINES += -D TARGET_FLASHFORGE
  # FlashForge uses dynamic linking (per Alexander's configuration)
  LDFLAGS ?= -lm -Llibhv/lib -Lspdlog/build -l:libhv.a -latomic -lpthread -Lwpa_supplicant/wpa_supplicant/ -l:libwpa_client.a -lstdc++fs -l:libspdlog.a
  RANLIB = $(CROSS_COMPILE)ranlib

endif

#===============================================================================
# Cross-Compilation Toolchain Setup
#===============================================================================

ifdef CROSS_COMPILE
  CC     = $(CROSS_COMPILE)gcc
  CXX    = $(CROSS_COMPILE)g++
  CPP    = $(CC) -E
  AS     = $(CROSS_COMPILE)as
  LD     = $(CROSS_COMPILE)ld
  AR     = $(CROSS_COMPILE)ar
  NM     = $(CROSS_COMPILE)nm
  STRIP  = $(CROSS_COMPILE)strip
  ifndef RANLIB
    RANLIB = $(CROSS_COMPILE)ranlib
  endif
endif

#===============================================================================
# Build Configuration
#===============================================================================

# Detect number of processors for parallel builds
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  # macOS
  NPROC := $(shell sysctl -n hw.ncpu 2>/dev/null || echo 4)
else ifeq ($(UNAME_S),Linux)
  # Linux
  NPROC := $(shell nproc 2>/dev/null || echo 4)
else
  # Fallback for other systems
  NPROC := 4
endif

LVGL_DIR_NAME  ?= lvgl
LVGL_DIR       ?= .
BIN             = guppyscreen
BUILD_DIR       = ./build
BUILD_OBJ_DIR   = $(BUILD_DIR)/obj
BUILD_BIN_DIR   = $(BUILD_DIR)/bin
SPDLOG_DIR      = spdlog

prefix ?= /usr
bindir ?= $(prefix)/bin

# Detect compiler type for appropriate warning flags
CC_VERSION := $(shell $(CC) --version 2>/dev/null)
CXX_VERSION := $(shell $(CXX) --version 2>/dev/null)

# Common warnings supported by both GCC and Clang
WARNINGS_COMMON := -Wall -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith \
                   -fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wno-unused-parameter \
                   -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess \
                   -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default \
                   -Wreturn-type -Wmultichar -Wformat-security -Wno-error=pedantic -Wno-sign-compare \
                   -Wdouble-promotion -Wempty-body -Wshift-negative-value -Wno-unused-value

# GCC-specific warnings (not supported by Clang)
WARNINGS_GCC := -Wmaybe-uninitialized -Wclobbered

# Detect if we're using Clang or GCC
ifneq (,$(findstring clang,$(CC_VERSION)))
  # Clang: use only common warnings
  WARNINGS := $(WARNINGS_COMMON)
else ifneq (,$(findstring gcc,$(CC_VERSION)))
  # GCC: use common + GCC-specific warnings
  WARNINGS := $(WARNINGS_COMMON) $(WARNINGS_GCC)
else
  # Unknown compiler: use only common warnings to be safe
  WARNINGS := $(WARNINGS_COMMON)
endif

CFLAGS  ?= -O3 -g0 -MD -MP -I$(LVGL_DIR)/ $(WARNINGS)
INC      := -I./ -I./lvgl/ -I./lv_touch_calibration -I./spdlog/include -Ilibhv/include -Iwpa_supplicant/src/common
LDLIBS   := -lm

DEFINES += -D _GNU_SOURCE -DSPDLOG_COMPILED_LIB

#===============================================================================
# Version Management
#===============================================================================

# Read version from VERSION file if it exists
ifneq ($(wildcard VERSION),)
  GUPPYSCREEN_VERSION := $(shell cat VERSION | tr -d '\n')
endif

#===============================================================================
# Optional Build Flags (legacy support)
#===============================================================================

ifdef EVDEV_CALIBRATE
  DEFINES += -D EVDEV_CALIBRATE
endif

ifdef GUPPYSCREEN_VERSION
  DEFINES += -D GUPPYSCREEN_VERSION="\"${GUPPYSCREEN_VERSION}\""
endif

# Theme selection
ASSET_DIR = material
ifeq ($(GUPPY_THEME),zbolt)
  ASSET_DIR = zbolt
  DEFINES += -D ZBOLT
endif

# Small screen support
ifdef GUPPY_SMALL_SCREEN
  ASSET_DIR = material_46
  DEFINES += -D GUPPY_SMALL_SCREEN
endif

# Screen rotation
ifdef GUPPY_ROTATE
  DEFINES += -D GUPPY_ROTATE
endif

#===============================================================================
# Source Files
#===============================================================================

MAINSRC  = $(filter-out $(LVGL_DIR)/src/kd_graphic_mode.cpp, $(wildcard $(LVGL_DIR)/src/*.cpp))

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk

CSRCS   += $(wildcard $(LVGL_DIR)/assets/*.c)
CSRCS   += $(wildcard $(LVGL_DIR)/lv_touch_calibration/*.c)

# Add theme assets
ifeq ($(GUPPY_THEME),zbolt)
  CSRCS += $(wildcard $(LVGL_DIR)/assets/zbolt/*.c)
else
  CSRCS += $(wildcard $(LVGL_DIR)/assets/$(ASSET_DIR)/*.c)
endif

#===============================================================================
# Object Files
#===============================================================================

OBJEXT  ?= .o
AOBJS    = $(ASRCS:.S=$(OBJEXT))
COBJS    = $(CSRCS:.c=$(OBJEXT))
MAINOBJ  = $(MAINSRC:.cpp=$(OBJEXT))
DEPS     = $(addprefix $(BUILD_OBJ_DIR)/, $(patsubst %.o, %.d, $(MAINOBJ)))
OBJS     = $(AOBJS) $(COBJS) $(MAINOBJ)
TARGETS  = $(addprefix $(BUILD_OBJ_DIR)/, $(patsubst ./%, %, $(OBJS)))

COMPILE_CC  = $(CC) $(CFLAGS) $(INC) $(DEFINES)
COMPILE_CXX = $(CXX) $(CFLAGS) $(INC) $(DEFINES)

#===============================================================================
# Build Rules
#===============================================================================

all: default

libhv.a:
	$(MAKE) -C libhv -j$(NPROC) libhv

libspdlog.a:
	@mkdir -p $(SPDLOG_DIR)/build
	@cmake -B $(SPDLOG_DIR)/build -S $(SPDLOG_DIR)/ -DCMAKE_CXX_COMPILER=$(CXX)
	$(MAKE) -C $(SPDLOG_DIR)/build -j$(NPROC)

wpaclient:
ifeq ($(TARGET),flashforge)
	$(MAKE) -C wpa_supplicant/wpa_supplicant CC=$(CC) RANLIB=$(RANLIB) -j$(NPROC) libwpa_client.a
else
	$(MAKE) -C wpa_supplicant/wpa_supplicant -j$(NPROC) libwpa_client.a
endif

$(BUILD_OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(COMPILE_CXX) -std=c++17 $(CFLAGS) -c $< -o $@
	@echo "CXX $<"

$(BUILD_OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(COMPILE_CC) $(CFLAGS) -c $< -o $@
	@echo "CC $<"

$(BUILD_OBJ_DIR)/kd_graphic_mode.o: src/kd_graphic_mode.cpp
	@mkdir -p $(dir $@)
	@$(COMPILE_CC) $(CFLAGS) -c $< -o $@
	@echo "CC $<"

kd_graphic_mode: $(BUILD_OBJ_DIR)/kd_graphic_mode.o
	$(CC) -o $(BUILD_BIN_DIR)/kd_graphic_mode $(BUILD_OBJ_DIR)/kd_graphic_mode.o

default: $(TARGETS)
	@mkdir -p $(dir $(BUILD_BIN_DIR)/)
	$(CXX) -o $(BUILD_BIN_DIR)/$(BIN) $(TARGETS) $(LDFLAGS) $(LDLIBS)
	@echo "CXX $<"

#===============================================================================
# Clean Targets
#===============================================================================

spdlogclean:
	rm -rf $(SPDLOG_DIR)/build

libhvclean:
	$(MAKE) -C libhv clean

wpaclean:
	$(MAKE) -C wpa_supplicant/wpa_supplicant clean

clean:
	rm -rf $(BUILD_DIR)

#===============================================================================
# Install/Uninstall
#===============================================================================

install:
	install -d $(DESTDIR)$(bindir)
	install $(BUILD_BIN_DIR)/$(BIN) $(DESTDIR)$(bindir)

uninstall:
	$(RM) -r $(addprefix $(DESTDIR)$(bindir)/,$(BIN))

#===============================================================================
# Full Build Process
#===============================================================================

build:
	@echo "Building with $(NPROC) parallel jobs..."
	$(MAKE) wpaclean
	$(MAKE) wpaclient
	$(MAKE) libhvclean
	$(MAKE) libhv.a
	$(MAKE) spdlogclean
	$(MAKE) libspdlog.a
	$(MAKE) clean
	$(MAKE) -j$(NPROC) default
	@echo "Setting up configuration..."
ifeq ($(TARGET),simulator)
	@echo "Copying simulator config and creating directories..."
	@mkdir -p $(BUILD_DIR)/logs $(BUILD_DIR)/thumbnails
	@cp -n guppyconfig-simulator.json $(BUILD_BIN_DIR)/guppyconfig.json 2>/dev/null || true
else
	@echo "Copying production config..."
	@cp -n debian/guppyconfig.json $(BUILD_BIN_DIR)/guppyconfig.json 2>/dev/null || true
endif

-include $(DEPS)
