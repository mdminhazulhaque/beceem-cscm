# ==========================================================
# 
# Beceem Cal API 
#
# Copyright 2009 Beceem Communications, Inc.
#
# ==========================================================

#
# Makefile for compiling CSCM Client
#

TARGET_OBJ_DIR=.
TARGET_BIN_DIR=../build

ifeq ($(PLATFORM),mac)
CC = gcc
OS = OSX
CFLAGS += -arch i386 -arch x86_64
else
CFLAGS += -fPIC
CC = $(CROSS_COMPILE)gcc
OS = LINUX
endif

CFLAGS += -Wall -std=gnu99 -g

ifeq ($(DEBUG),0)
CFLAGS += -O2
endif

SOURCE_DIR = ./
COMMON_DIR = ../common

ifeq ($(PLATFORM),bcs5350)
CFLAGS += -fPIC
endif

include $(COMMON_DIR)/Makefile

INCLUDE_DIRS = -I $(SOURCE_DIR) -I $(COMMON_DIR)
TARGET = wimaxc
LIB_FLAGS = 

OBJECTS = BeceemCscmClient.o \
		BeceemCscmCommon.o \
		SocketFunctions.o \
		Utility.o

$(TARGET) : $(OBJECTS)
	@echo -e "\n***** Building CM Client application *****"
	$(CC) $(CFLAGS) -o $(TARGET_BIN_DIR)/$(TARGET) $(patsubst %.o,$(TARGET_OBJ_DIR)/%.o,$(OBJECTS)) $(LIB_FLAGS)

%.o:$(SOURCE_DIR)/%.c
	@echo -e "\nCompiling $^ ..."
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $^ -o $(TARGET_OBJ_DIR)/$@ -D$(OS)

clean : 
	@echo -e "\n***** Cleaning CM Client application *****"
	rm -f -r $(patsubst %.o,$(TARGET_OBJ_DIR)/%.o,$(OBJECTS))
	rm -f -r $(TARGET_BIN_DIR)/$(TARGET)

install :
	@echo -e "\n***** Install not implemented *****"

