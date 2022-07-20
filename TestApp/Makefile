C      := -gcc
CFLAGS := -pedantic-errors -Wall -Wextra

RPATH_LINKAGE := -Wl,-rpath,$(CURDIR)/../Jotunn/.build/lib/

LDFLAGS  := $(RPATH_LINKAGE) -L$(CURDIR)/../Jotunn/.build/lib/ -ljotunn

BUILD    := ./.build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := TestApp
INCLUDE  := -I$(CURDIR)/../Jotunn/include/ \

HDRS     := \

SRC      := main.c \

OBJECTS := $(SRC:%.c=$(OBJ_DIR)/%.o)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.c $(HDRS)
	@mkdir -p $(@D)
	$(C) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(C) $(CFLAGS) $(INCLUDE) -o $(APP_DIR)/$(TARGET) $(OBJECTS) $(LDFLAGS)

.PHONY: all build clean debug release

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CFLAGS += -DDEBUG -g
debug: all

release: CFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*