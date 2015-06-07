#
# Makefile for compiling BeceemWiMAX shared object
#

VERSION := $(subst ",,$(word 3, $(shell echo `grep "define DLL_VERSION" libraryversion.h`)))

TARGET_LIB := $(shell if [ "$(TARGET_LIB)" = "" ]; then echo "libxvi020.so"; else echo $(TARGET_LIB); fi)
ENABLE_STATIC_LINKING := $(shell if [ "$(ENABLE_STATIC_LINKING)" = "" ]; then echo "n"; else echo $(ENABLE_STATIC_LINKING); fi)

CC := $(shell if [ "$(CC)" = "" ]; then echo "gcc -Wall"; else echo $(CC); fi)
AR := $(shell if [ "$(AR)" = "" ]; then echo "ar"; else echo $(AR); fi) 

#below flag needs to be enabled if pthread_cancel is not supported on the ported platform.
#LIB_THREAD_FLAG=-DBCM_PTHREAD_CANCEL_NOT_SUPPORTED


SOURCE_DIR = ./src
LINUX_SOURCE_DIR = $(SOURCE_DIR)/linux

TARGET_DIR = ../build
TARGET_OBJ_DIR = .
TARGET=$(TARGET_LIB)
LIB_FLAGS = -lpthread -lrt
INCLUDE_DIR = -I../include -I./src/ -I./ -I./src/linux/

include $(LINUX_SOURCE_DIR)/Makefile

OBJECTS = $(LINUX_OBJECTS) \
		BeceemWiMAX.o \
		ProxyEndianHandler.o\
		BeceemWiMAXExport.o \
		WiMAXMessage.o \
		ProcessResponse.o \
		DriverPacket.o \
		SendMessage.o \
		Logger.o \
		MIBInitializations.o \
		MIBGetSetAPI.o \
		uti_BeceemEEPROMCfg.o \
		uti_BeceemEEPROMCfgStruct.o \
		OMADMGetSetAPI.o\
		OMADMNodeInfo.o \
		WiMAXCommon.o \
		WiMAXCommonAPI.o \
		vendorspecificextension.o\
		flashhandler.o \
		dynamiclogging.o \
		socketipc.o \
		CAPIOMAinterface.o


$(TARGET) : $(OBJECTS)
ifeq ($(ENABLE_STATIC_LINKING),n)	
	@echo "LINK LIBRARIES =" $(TARGET_DIR)/$(TARGET)
	@$(CC) -shared -fPIC    -o $(TARGET_DIR)/$(TARGET) $(patsubst %.o,$(TARGET_OBJ_DIR)/%.o,$(OBJECTS)) $(LIB_FLAGS) -Wl,-rpath,$(TARGET_DIR)/
# orginal
# @$(CC) -shared -fPIC    -o $(TARGET_DIR)/$(TARGET).$(VERSION) $(patsubst %.o,$(TARGET_OBJ_DIR)/%.o,$(OBJECTS)) $(LIB_FLAGS) -Wl,-rpath,$(TARGET_DIR)/
# ln -fs $(TARGET).$(VERSION) $(TARGET_DIR)/$(TARGET)
	
else
	@echo AR $(TARGET)
	@$(AR) -q ${TARGET_DIR}/$(TARGET) $(TARGET_OBJ_DIR)/*.o
endif		


%.o:$(SOURCE_DIR)/%.c
	@echo CC $@
	@$(CC) $(INCLUDE_DIR) $(CPE_BUILD_CFLAGS) $(NP_BUILD_FLAGS) $(LINK_LIBRARIES)   -c $^ -o $(TARGET_OBJ_DIR)/$@ -DLINUX -DPROXY_ENABLED_BUILD 



clean :
	@echo "Cleaning BeceemWiMax Lib"
	@rm -f -r $(patsubst %.o,$(TARGET_OBJ_DIR)/%.o,$(OBJECTS))
	@rm -f -r $(TARGET_DIR)/$(TARGET).$(VERSION)
	@rm -f -r $(TARGET_DIR)/$(TARGET)

install :
ifeq ($(ENABLE_STATIC_LINKING),n)
	@cp $(TARGET_DIR)/$(TARGET).$(VERSION) /lib/$(TARGET).$(VERSION)
	@ln -fs /lib/$(TARGET).$(VERSION) /lib/$(TARGET)
endif
