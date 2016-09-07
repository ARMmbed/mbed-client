#
# Generic rules for fetching and building sources as submodules.
#
# This is used from Makefile.standalone
#

# macro for fetching the file content to a variable
LIB_CONTENT = $(shell cat $1)

GIT_URL = $(word 1, $(subst \#, , $1))
GIT_HASH = $(word 2, $(subst \#, , $1))

# Rule for processing the mbed-cli's submodule reference files (.lib).
# The lib files contain a <repository-git-url>#<commit-hash> as a string
# which is extracted and used for "git clone" + "git checkout" commands
$(SUBMODULE_BASE_PATH)/% : %.lib
	cd $(SUBMODULE_BASE_PATH) && \
	if [ -d $@ ]; then \
		echo Just update the repository && \
		pwd &&\
		cd $@ && \
		git fetch; \
	else \
		echo Clone the repository && \
		pwd && \
		git clone $(call GIT_URL, $(call LIB_CONTENT, $<)) && \
		cd $@; \
	fi; \
	git checkout $(call GIT_HASH, $(call LIB_CONTENT, $<))

BUILDDIRS := $(LIBS:%=build-%)
.PHONY: $(BUILDDIRS)
$(BUILDDIRS): 
	echo $(@:build-%=%)
	make -C $(SUBMODULE_BASE_PATH)/$(@:build-%=%) -f Makefile.standalone SUBMODULE_BASE_PATH=$(SUBMODULE_BASE_PATH) CC=$(CC) CPU=$(CPU)

# Note: the correct CC is needed as parameter as the library names are also constructed uring it,
# eg. libnsdl.a is actually libnsdl_gcc.a on GCC compilation.
CLEANDIRS := $(LIBS:%=clean-%)
.PHONY: $(CLEANDIRS)
$(CLEANDIRS): 
	echo $(@:clean-%=%)
	make -C $(SUBMODULE_BASE_PATH)/$(@:clean-%=%) -f Makefile.standalone clean SUBMODULE_BASE_PATH=$(SUBMODULE_BASE_PATH) CC=$(CC) CPU=$(CPU)

CLEAN_SPECIAL_DIRS := $(SPECIAL_LIBS:%=clean-%)
.PHONY: $(CLEAN_SPECIAL_DIRS)
$(CLEAN_SPECIAL_DIRS): 
	echo $(@:clean-%=%)
	make -C $(SUBMODULE_BASE_PATH)/$(@:clean-%=%) -f Makefile clean SUBMODULE_BASE_PATH=$(SUBMODULE_BASE_PATH) CC=$(CC) CPU=$(CPU)

FETCH_SUB_MODULES := $(LIBS:%=fetch-%)
.PHONY: $(FETCH_SUB_MODULES)
$(FETCH_SUB_MODULES): $(SUBMODULE_BASE_PATH)
	@echo $@
	make -C $(SUBMODULE_BASE_PATH)/$(@:fetch-%=%) -f Makefile.standalone fetch_source SUBMODULE_BASE_PATH=$(SUBMODULE_BASE_PATH) CC=$(CC) CPU=$(CPU)
