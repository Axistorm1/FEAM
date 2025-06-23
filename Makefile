BUILD_DIR ?= build

BUILD_TYPE ?= Release

all: configure
	@cmake --build $(BUILD_DIR) --config $(BUILD_TYPE) -j12
	@mv $(BUILD_DIR)/file_manager .

debug: BUILD_TYPE := Debug
debug: configure
	@cmake --build $(BUILD_DIR) --config Debug -j12
	@mv $(BUILD_DIR)/file_manager .

profile: BUILD_TYPE := Profile
profile: configure
	@cmake --build $(BUILD_DIR) --config Profile -j12
	@mv $(BUILD_DIR)/file_manager .

configure:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

clean:
	@cmake --build $(BUILD_DIR) --target clean

fclean:
	@rm -rf $(BUILD_DIR)
	@rm -rf file_manager

re: fclean all

.PHONY: all debug profile clean fclean re
