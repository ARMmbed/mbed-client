#
# Makefile for LWM2M C++ Library
#
# List of subdirectories to build
TEST_FOLDER := ./test/

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

# List of unit test directories for libraries
UNITTESTS := $(sort $(dir $(wildcard $(TEST_FOLDER)*/utest/*)))

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

TESTDIRS := $(UNITTESTS:%=build-%)
CLEANTESTDIRS := $(UNITTESTS:%=clean-%)
COVERAGEFILE := ./lcov/coverage.info

#
# Define compiler toolchain
#
include ../libService/toolchain_rules.mk

$(eval $(call generate_rules,$(LIB),$(SRCS)))

.PHONY: release
release:
	7z a nanostack_$(VERSION).zip *.a *.lib include

$(TESTDIRS):
	@make -C $(@:build-%=%)

$(CLEANDIRS):
	@make -C $(@:clean-%=%) clean

$(CLEANTESTDIRS):
	@make -C $(@:clean-%=%) clean

.PHONY: test
test: $(TESTDIRS)
	@mkdir -p lcov
	@mkdir -p lcov/results
	@rm -f lcov/results/*
	@find ./test -name '*.xml' | xargs cp -t ./lcov/results/
	@rm -f lcov/index.xml
	@./xsl_script.sh
	@cp junit_xsl.xslt lcov/.
	@xsltproc -o lcov/testresults.html lcov/junit_xsl.xslt lcov/index.xml
	@rm -f lcov/junit_xsl.xslt
	@rm -f lcov/index.xml
	@rm -rf lcov/results
	@lcov -d test/. -c -o $(COVERAGEFILE)
	@lcov -q -r $(COVERAGEFILE) "/usr*" -o $(COVERAGEFILE)
	@lcov -q -r $(COVERAGEFILE) "/test*" -o $(COVERAGEFILE)
	@genhtml -q $(COVERAGEFILE) --show-details --prefix $(CURDIR:%/applications/libService=%) --output-directory lcov/html
	@echo LibService unit tests built

clean: $(CLEANDIRS) \
	$(CLEANTESTDIRS)
