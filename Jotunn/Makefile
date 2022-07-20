C      := -gcc
CFLAGS := -pedantic-errors -Wall -Wextra -fPIC

LDFLAGS  := -L/usr/lib/ -lstdc++ -lm -lpthread -lGL -lglfw -lGLEW -shared

BUILD    := ./.build
OBJ_DIR  := $(BUILD)/objects
LIB_DIR  := $(BUILD)/lib
TARGET   := libjotunn.so
INCLUDE  := -I./include \

HDRS     := \
			$(wildcard ./include/*.h)  \

SRC      := \
			$(wildcard ./src/Core/Application/*.c)  \

OBJECTS := $(SRC:%.c=$(OBJ_DIR)/%.o)

all: build $(LIB_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.c $(HDRS)
	@mkdir -p $(@D)
	$(C) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(LIB_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(C) $(CFLAGS) $(INCLUDE) -o $(LIB_DIR)/$(TARGET) $(OBJECTS) $(LDFLAGS)

.PHONY: all build clean debug release

build:
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CFLAGS += -DDEBUG -g
debug: all

release: CFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(LIB_DIR)/*