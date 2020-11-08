ARCH := $(shell uname -m)

ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
BUILD_DIR ?= .

LIBUCONTEXT = $(BUILD_DIR)/libucontext.a

all: $(LIBUCONTEXT)

CSRCS := $(wildcard arch/${ARCH}/*.c)
SSRCS := $(wildcard arch/${ARCH}/*.S)
OBJS := $(CSRCS:%.c=$(BUILD_DIR)/%.o) $(SSRCS:%.S=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:%.o=%.d)

INC_DIRS := include $(BUILD_DIR)/arch/${ARCH}
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

override CFLAGS +=					\
	-DCUSTOM_IMPL					\
	-DG_LOG_DOMAIN='"ucontext"'			\
	$(INC_FLAGS)					\
	-MMD -MP

$(LIBUCONTEXT): $(OBJS)

.PHONY: clean

clean:
	rm -r $(OBJS) $(DEPS) $(LIBUCONTEXT)

$(BUILD_DIR)/arch/${ARCH}/%.o: $(ROOT_DIR)/arch/${ARCH}/%.c
	@$(MKDIR_P) $(dir $@)
	$(call quiet-command,$(CC) $(CFLAGS) -c -o $@ $<,"CC","$@")

$(BUILD_DIR)/arch/${ARCH}/%.o: $(ROOT_DIR)/arch/${ARCH}/%.S
	@$(MKDIR_P) $(dir $@)
	$(call quiet-command,$(CC) $(CFLAGS) -c -o $@ $<,"CC","$@")

%.a:
	$(call quiet-command,rm -f $@ && $(AR) rcs $@ $^,"AR","$@")

MKDIR_P ?= mkdir -p
quiet-command-run = $(if $(V),,$(if $2,printf "  %-7s %s\n" $2 $3 && ))$1
quiet-@ = $(if $(V),,@)
quiet-command = $(quiet-@)$(call quiet-command-run,$1,$2,$3)

print-%:
	@echo '$*=$($*)'

.SUFFIXES:

-include $(DEPS)
