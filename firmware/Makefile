OPENCM3_DIR ?= $(CURDIR)/libopencm3
DEVICE      = stm32f030f4
BUILD_DIR   = build
PROJECT     = firmware

include $(OPENCM3_DIR)/mk/genlink-config.mk
include $(OPENCM3_DIR)/mk/gcc-config.mk

LDSCRIPT = $(BUILD_DIR)/generated.$(DEVICE).ld

CFLAGS  += -Os -Wall -Wextra -Werror -std=c99 -ffunction-sections -fdata-sections
CPPFLAGS += -Iinclude -I$(OPENCM3_DIR)/include
LDFLAGS  += -nostartfiles -Wl,--gc-sections

OBJS = $(BUILD_DIR)/src/main.o \
       $(BUILD_DIR)/src/buttons.o \
       $(BUILD_DIR)/src/welder.o \
       $(BUILD_DIR)/src/i2c.o \
       $(BUILD_DIR)/src/ssd1306.o

all: $(BUILD_DIR)/$(PROJECT).bin $(BUILD_DIR)/$(PROJECT).hex

$(BUILD_DIR)/$(PROJECT).elf: $(OBJS) $(LDSCRIPT) $(LIBDEPS)
	@mkdir -p $(BUILD_DIR)
	$(LD) $(OBJS) $(LDLIBS) $(LDFLAGS) -T$(LDSCRIPT) $(ARCH_FLAGS) -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) -o $@ -c $<

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -Obinary $< $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -Oihex $< $@

$(LDSCRIPT): $(OPENCM3_DIR)/ld/linker.ld.S $(DEVICES_DATA)
	@mkdir -p $(BUILD_DIR)
	$(CPP) $(ARCH_FLAGS) $(shell $(OPENCM3_DIR)/scripts/genlink.py $(DEVICES_DATA) $(DEVICE) DEFS) -P -E $< -o $@

clean:
	rm -rf $(BUILD_DIR)

flash: $(BUILD_DIR)/$(PROJECT).bin
	st-flash write $< 0x8000000

.PHONY: all clean flash