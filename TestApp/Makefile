C      := -gcc
# CFLAGS := -g -pedantic-errors -std=c17 -Wall -Wextra
CFLAGS := -g -Wall -std=c17 -Wextra

RPATH_LINKAGE := -Wl,-rpath,$(CURDIR)/../Jotunn/.build/lib

LDFLAGS  := -L/usr/lib -lm -lpthread $(RPATH_LINKAGE) -L$(CURDIR)/../Jotunn/.build/lib -ljotunn

FREETYPE2_INCLUDE := -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sysprof-4

BUILD    := ./.build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := TestApp
INCLUDE  := -I$(CURDIR)/../Jotunn/include -I$(CURDIR)/../Jotunn/vendor/clay $(FREETYPE2_INCLUDE)

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