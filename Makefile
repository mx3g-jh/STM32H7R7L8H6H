
MAKE = make
MAKEFLAGS += --no-print-directory
MAKE_ARGS = -j14
PNA ?= TEST
CURRENT_DIR := $(shell pwd)
OPENOCD		?= JLinkExe

ifeq ($(PNA),BOOT)
COMMUNICATION_PROTOCOL = MAVLINK2
USE_MCU_NAME = STM32H7R7L8
endif

ifeq ($(PNA),APP)
COMMUNICATION_PROTOCOL = MAVLINK2
USE_MCU_NAME = STM32H7R7L8
endif

define cmake-build
	$(eval BUILD_DIR := build)

	@if [ ! -e $(BUILD_DIR)/CMakeCache.txt ]; then \
		mkdir -p $(BUILD_DIR) && \
		cd $(BUILD_DIR) && \
		cmake .. \
		-DCMAKE_PROJECT_NAME=$(PNA) \
		-DCMAKE_COMMUNICATION_PROTOCOL=$(COMMUNICATION_PROTOCOL) \
		-DUSE_MCU_NAME=$(USE_MCU_NAME) \
		-DBUILD_CONTEXT=$(PNA) || rm -rf $(BUILD_DIR); \
	fi

	@echo "Build dir: $(BUILD_DIR)" && \
	cd $(BUILD_DIR) && $(MAKE) $(MAKE_ARGS) $(ARGS)

	@if [ "$(PNA)" = "APP" ]; then \
		cp Appli/build/APP.bin $(BUILD_DIR); \
		cp Appli/build/APP.hex $(BUILD_DIR); \
		cp Appli/build/APP.elf $(BUILD_DIR); \
	fi

	@if [ "$(PNA)" = "BOOT" ]; then \
		cp Boot/build/BOOT.bin $(BUILD_DIR); \
		cp Boot/build/BOOT.hex $(BUILD_DIR); \
		cp Boot/build/BOOT.elf $(BUILD_DIR); \
	fi
endef


# rebuild jlink-flash-fw-standalone.jlink
define generate-jlink-script
	@rm -f cmake/jlink-flash-fw-standalone.jlink
	@echo "Creating jlink-flash-fw-standalone.jlink"
	@echo "h" >> cmake/jlink-flash-fw-standalone.jlink
	@echo "loadfile ./build/${PNA}.bin, 0x08000000" >> cmake/jlink-flash-fw-standalone.jlink
	@echo "r" >> cmake/jlink-flash-fw-standalone.jlink
	@echo "q" >> cmake/jlink-flash-fw-standalone.jlink
endef

all:
	@make clean
	$(call generate-jlink-script)
	@echo "jlink-script build complete"
	$(call cmake-build, 	${CMAKE_PROJECT_NAME}.elf)

upload: all jlink-flash-bootloader
	@echo "Upload have done!"

jlink_flash:
	@$(MAKE) all
	@if [ "$(PNA)" = "BOOT" ]; then \
		$(OPENOCD) -device $(USE_MCU_NAME) -speed 4000 -if SWD -CommanderScript ./cmake/jlink-flash-fw-standalone_boot.jlink; \
	else \
		$(OPENOCD) -device $(USE_MCU_NAME) -speed 4000 -if SWD -CommanderScript ./cmake/jlink-flash-fw-standalone_app.jlink; \
	fi
	@echo "$(OPENOCD) flash done !"

clean:
	@rm -rf build/
	@rm -rf Appli/build/
	@rm -rf Boot/build/

# Makefile for Astyle
# -------------------------
# use dos2unix fix style
# brew install dos2unix
# dos2unix xxx.xx

# Define check_format target
check_format:
	@echo "Checking formatting with astyle"
	@./Tools/astyle/check_code_style_all.sh
	@echo "Running git diff to check for changes"
	@git diff --check

# Define format target
format:
	@echo "Formatting with astyle"
	@./Tools/astyle/check_code_style_all.sh --fix

.PHONY:
	clean upload check_format format
