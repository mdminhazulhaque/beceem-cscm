#
# Makefile for Common files
#

%.o:$(COMMON_DIR)/%.c
	@echo -e "\nCompiling $^..."
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $^ -o $(TARGET_OBJ_DIR)/$@ -D$(OS)
