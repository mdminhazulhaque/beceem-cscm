

LINUX_SOURCE_DIR = ./src/linux
TARGET_OBJ_DIR = .

LINUX_OBJECTS =  Thread.o \
		Sem.o \
		SyncObjects.o \
		DeviceIO.o \
		DebugPrint.o \
		SharedMemory.o

%.o:$(LINUX_SOURCE_DIR)/%.c
	@echo CC $@
	@$(CC) $(INCLUDE_DIR) -c $^ $(NP_BUILD_FLAGS) -o $(TARGET_OBJ_DIR)/$@ -DLINUX $(DEBUG_FLAG) $(LIB_THREAD_FLAG)

