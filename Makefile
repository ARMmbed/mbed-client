#
# Makefile for LWM2M C++ Library
#

# Define compiler toolchain with CC or PLATFORM variables
# Example (GCC toolchains, default $CC and $AR are used)
# make
#
# OR (Cross-compile GCC toolchain)
# make PLATFORM=arm-linux-gnueabi-
#
# OR (ArmCC/Keil)
# make CC=ArmCC AR=ArmAR
#
# OR (IAR-ARM)
# make CC=iccarm

LIB = liblwm2m.a

# If configuration is not specified, use linux
ifeq (,$(CONFIG))
CONFIG := linux
endif

include sources.mk
include include_dirs.mk
include config/$(CONFIG).mk

SERVLIB_DIR := ../libService
NSDL_C_DIR := ../nsdl-c
override CFLAGS += -I$(NSDL_C_DIR)/nsdl-c
override CFLAGS += $(addprefix -I,$(INCLUDE_DIRS))
override CFLAGS += $(addprefix -D,$(FLAGS))
ifeq ($(DEBUG),1)
override CFLAGS += -DHAVE_DEBUG
endif

#
# Define compiler toolchain
#
include ../libService/toolchain_rules.mk

$(eval $(call generate_rules,$(LIB),$(SRCS)))

.PHONY: release
release:
	7z a nanostack_$(VERSION).zip *.a *.lib include
