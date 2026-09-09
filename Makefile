ARCH ?= x86_64
CROSS_COMPILE ?=

CC ?= $(CROSS_COMPILE)gcc
AR ?= $(CROSS_COMPILE)ar
CP ?= cp
MKDIR ?= mkdir -p
RM = rm -rf

BUILD_DIR ?= $(CURDIR)/build
TEMP_DIR ?= $(CURDIR)/temp
SRC_DIR = $(CURDIR)/src
INC_DIR = $(CURDIR)/include
AGFX_DIR = $(CURDIR)/agfx

ifeq ($(V),1)
    Q :=
    ECHO := @true
else
    Q := @
    ECHO := @printf
endif

CYAN   := \033[0;36m
YELLOW := \033[1;33m
GREEN  := \033[0;32m
RED    := \033[1;31m
PURPLE := \033[0;35m
LCYAN  := \033[1;36m
DRED   := \033[0;31m
BROWN  := \033[0;33m
GRAY   := \033[0;37m
NC     := \033[0m

ifeq ($(ARCH),x86_64)
    ARCH_CFLAGS ?= -m64 -mno-red-zone
endif

COMMON_CFLAGS = -Wall -Wextra -std=gnu11 -fno-omit-frame-pointer -ffreestanding -fno-pic -fno-pie -fstack-protector
USER_COMMON_CFLAGS = $(COMMON_CFLAGS) -fno-asynchronous-unwind-tables $(ARCH_CFLAGS)
LIB_CFLAGS = $(USER_COMMON_CFLAGS) -nostdinc -I$(INC_DIR) -I$(AGFX_DIR)

GENERIC_SRCS = $(filter-out $(SRC_DIR)/aos_start.c, $(wildcard $(SRC_DIR)/*.c))
GENERIC_OBJS = $(patsubst $(SRC_DIR)/%.c, $(TEMP_DIR)/%.o, $(GENERIC_SRCS))

ARCH_DIR  = $(SRC_DIR)/arch/$(ARCH)
ARCH_SRCS = $(wildcard $(ARCH_DIR)/*.c)
ARCH_OBJS = $(patsubst $(ARCH_DIR)/%.c, $(TEMP_DIR)/arch_%.o, $(ARCH_SRCS))

AGFX_SRCS = $(wildcard $(AGFX_DIR)/*.c)
AGFX_OBJS = $(patsubst $(AGFX_DIR)/%.c, $(TEMP_DIR)/agfx_%.o, $(AGFX_SRCS))

LIB_OBJS = $(GENERIC_OBJS) $(ARCH_OBJS) $(AGFX_OBJS)

START_SRC = $(SRC_DIR)/aos_start.c
START_OBJ = $(TEMP_DIR)/aos_start.o

TARGET_LIB = $(BUILD_DIR)/libaos.a
TARGET_START = $(BUILD_DIR)/aos_start.o

.PHONY: all prepare clean

all: prepare $(TARGET_LIB) $(TARGET_START)
	$(Q)$(RM) $(INC_DIR)/stb_truetype.h
	@echo "AOSLIB Build Successful for $(ARCH)!"

prepare:
	$(ECHO) "${RED}[  MKDIR  ]${NC} ${BUILD_DIR}\n"
	$(Q)$(MKDIR) $(BUILD_DIR)
	$(ECHO) "${RED}[  MKDIR  ]${NC} ${TEMP_DIR}\n"
	$(Q)$(MKDIR) $(TEMP_DIR)
	@if [ -d "$(AGFX_DIR)" ]; then \
		$(CP) -f $(AGFX_DIR)/*.h $(INC_DIR)/; \
	fi

$(TARGET_LIB): $(LIB_OBJS)
	$(ECHO) "${LCYAN}[   AR    ]${NC} $@\n"
	$(Q)$(AR) rcs $@ $^

$(TARGET_START): $(START_OBJ)
	$(ECHO) "${BROWN}[   CP    ]${NC} $< ${GREEN}->${NC} $@\n"
	$(Q)$(CP) $< $@

$(TEMP_DIR)/%.o: $(SRC_DIR)/%.c | prepare
	$(ECHO) "${CYAN}[   CC    ]${NC} $<\n"
	$(Q)$(CC) $(LIB_CFLAGS) -c $< -o $@

$(TEMP_DIR)/arch_%.o: $(ARCH_DIR)/%.c | prepare
	$(ECHO) "${CYAN}[   CC    ]${NC} (arch: $(ARCH)) $<\n"
	$(Q)$(CC) $(LIB_CFLAGS) -c $< -o $@

$(TEMP_DIR)/agfx_%.o: $(AGFX_DIR)/%.c | prepare
	$(ECHO) "${CYAN}[   CC    ]${NC} (AGFX) $<\n"
	$(Q)$(CC) $(LIB_CFLAGS) -c $< -o $@

clean:
	$(ECHO) "${DRED}[   RM    ]${NC} ${TEMP_DIR}\n"
	$(Q)$(RM) $(TEMP_DIR)
	$(ECHO) "${DRED}[   RM    ]${NC} ${BUILD_DIR}\n"
	$(Q)$(RM) $(BUILD_DIR)
	$(Q)$(RM) $(INC_DIR)/agfx*.h $(INC_DIR)/stb_truetype.h