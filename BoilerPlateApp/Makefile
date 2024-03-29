C      := -gcc
CFLAGS := -pedantic-errors -Wall -Wextra

RPATH_LINKAGE := -Wl,-rpath,$(CURDIR)/../Jotunn/.build/lib/

LDFLAGS  := -L/usr/lib/ -lstdc++ -lm -lpthread $(RPATH_LINKAGE) -L$(CURDIR)/../Jotunn/.build/lib/ -ljotunn

FREETYPE2_INCLUDE := -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sysprof-4

BUILD    := ./.build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := BoilerPlateApp
INCLUDE  := -I$(CURDIR)/../Jotunn/include/ $(FREETYPE2_INCLUDE)

HDRS     := ball_window.h \

SRC      := ball_window.c \
			main.c 		  \

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