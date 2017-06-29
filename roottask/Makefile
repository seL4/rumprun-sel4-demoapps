#
# Copyright 2017, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
# @TAG(DATA61_BSD)
#

LIBDIRS = $(PWD)/$(CONFIG_RUMP_DIR)/lib
INCLUDE_DIRS := $(PWD)/$(CONFIG_RUMP_DIR)/include $(PWD)/$(CONFIG_RUMP_DIR)/../src/sys
# Targets
TARGETS := $(notdir $(SOURCE_DIR)).bin

# Set custom entry point as we are a rootserver and will not be
# started in a standard way
ENTRY_POINT := _sel4_start

# Source files required to build the target
CFILES   := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/*.c))
CFILES   += $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/plat/${PLAT}/*.c))
CFILES   += $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/arch/${ARCH}/*.c))

# CPIO archive
OFILES := archive.o
ifdef $(RUMPAPPPATH)
COMPONENTS2 := $(RUMPAPPPATH)
else
COMPONENTS2 := $(COMPONENTS)
endif

CMDLINE := cmdline.h
HFILES += $(CMDLINE)
# Need to foce main.c to recompile incase -DRUMPAPPNAME below is different.
CFLAGS += -I$(BUILD_DIR)
ASMFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/arch/${ARCH}/*.S))
ASMFILES += $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/plat/${PLAT}/*.S))

# Libraries required to build the target
LIBS = c sel4 cpio elf sel4muslcsys sel4vka sel4allocman \
       platsupport sel4platsupport sel4vspace \
       sel4utils sel4simple utils sel4debug

LIBS += sel4simple-default


include $(SEL4_COMMON)/common.mk

# targets to generate CPIO archive of elf files
${COMPONENTS2}:
	false


$(CMDLINE): $(PROJECT_BASE)/apps/$(notdir $(COMPONENTS2))/$(CMDLINE)
	@echo " [$(CMDLINE)] installing $(CMDLINE)"
	$(Q)mkdir -p $(dir $@)
	rsync --checksum $(PROJECT_BASE)/apps/$(notdir $(COMPONENTS2))/$(CMDLINE) $@


archive.o: ${COMPONENTS2}
	@echo " [archive.o] $(COMPONENTS2)"
	$(Q)mkdir -p $(dir $@)
	${COMMON_PATH}/files_to_obj.sh archive2.o _cpio_archive $^
	rsync --checksum archive2.o archive.o
