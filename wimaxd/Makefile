# ==========================================================
# 
# Beceem CSCM
#
# Copyright 2009 Beceem Communications, Inc.
#
# ==========================================================

#
# Makefile for compiling CSCM Server
#

# Define DEBUG_MALLOC to get a memory allocation trace
# CFLAGS += -DDEBUG_MALLOC

ifeq ($(PLATFORM),mac)
CC = gcc
OS = OSX
else
CC = $(CROSS_COMPILE)gcc
OS = LINUX
endif

ifeq ($(PLATFORM),bcs5350) 
CFLAGS += -DPLATFORM_BCS5350
LIB_FLAGS += -Wl,-rpath-link,$(OPENSSL_LIB_PATH)
else ifeq ($(PLATFORM),bcs5200)
CFLAGS += -DPLATFORM_BCS5200
endif

LIB_FLAGS += -L$(TARGET_BIN_DIR)
ifdef CROSS_TOOLCHAIN_DIR
LIB_FLAGS += -Wl,-rpath-link,$(CROSS_TOOLCHAIN_DIR)/usr/lib
LIB_FLAGS += -Wl,-rpath,$(CROSS_TOOLCHAIN_DIR)/lib -Wl,-rpath,$(CROSS_TOOLCHAIN_DIR)/usr/lib 
LIB_FLAGS += -L$(CROSS_TOOLCHAIN_DIR)/lib -L$(CROSS_TOOLCHAIN_DIR)/usr/lib
else
LIB_FLAGS += -Wl,-rpath,/lib
endif

CFLAGS += -Wall -std=gnu99 -g
ifeq ($(DEBUG),0)
CFLAGS += -O2
endif

SOURCE_DIR = ./
COMMON_DIR = ../common
API_INC_DIR = ../include
EAP_SUP_DIR = ../libeap_supplicant
TARGET_BIN_DIR=../build
TARGET_OBJ_DIR=.

include $(COMMON_DIR)/Makefile

INCLUDE_DIRS = -I $(SOURCE_DIR) -I $(COMMON_DIR) -I $(API_INC_DIR) -I $(EAP_SUP_DIR)
TARGET = wimaxd

ifeq ($(PLATFORM),mac)
LIB_FLAGS += -framework BeceemWiMaxAPI 
CFLAGS += -arch i386 -arch x86_64
else
CFLAGS += -fPIC
LIB_FLAGS += -lrt
LIB_FLAGS += -L../build -lxvi020
endif

ifeq ($(PLATFORM),bcs5350)
CFLAGS += -fPIC
endif
LIB_FLAGS += -L../build -leap_supplicant -lpthread

OBJECTS = AuxStruct.o \
		BeceemAPI.o \
		BeceemAPICallback.o \
		BeceemAPIEap.o \
		BeceemAPIMisc.o \
		BeceemAPIRequest.o \
		BeceemAPIResponse.o \
		BeceemCscmLogin.o \
		BeceemCscmServer.o \
		BeceemAPIStrings.o \
		BeceemCscmCommon.o \
		BeceemCscmTop.o \
		BeceemCscmOEM.o \
		LogFile.o \
		Options.o \
		PrintContext.o \
		SocketFunctions.o \
		SysLogFunctions.o \
		ThreadFunctions.o \
		Utility.o

ifneq ($(PLATFORM),mac)
OBJECTS += ProcessFunctions.o
endif

$(TARGET) : $(OBJECTS)
	@echo -e "\n***** Building CM Server application *****"
	$(CC) $(CFLAGS) -o $(TARGET_BIN_DIR)/$(TARGET) $(patsubst %.o,$(TARGET_OBJ_DIR)/%.o,$(OBJECTS)) $(LIB_FLAGS)

%.o:$(SOURCE_DIR)/%.c
	@echo -e "\nCompiling $^ ..."
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $^ -o $(TARGET_OBJ_DIR)/$@ -D$(OS)

clean : 
	@echo -e "\n***** Cleaning CM Server application *****"
	rm -f -r $(patsubst %.o,$(TARGET_OBJ_DIR)/%.o,$(OBJECTS))
	rm -f -r $(TARGET_BIN_DIR)/$(TARGET)

install :
	@echo -e "\n***** Install not implemented *****"
