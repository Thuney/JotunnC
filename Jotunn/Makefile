C        := -gcc
CFLAGS   := -pedantic-errors -Wall -Wextra -fPIC
CDEFINES := 

GRAPHICS_BACKEND := OPENGL

ifeq ($(GRAPHICS_BACKEND), OPENGL)

LD_GRAPHICS_FLAGS := -lGL -lglfw -lGLEW
CDEFINES := $(CDEFINES) -DBACKEND_OPENGL 

endif

LDFLAGS  := -L/usr/lib/ -lstdc++ -lm -lpthread $(LD_GRAPHICS_FLAGS) -shared

BUILD    := ./.build
OBJ_DIR  := $(BUILD)/objects
LIB_DIR  := $(BUILD)/lib
TARGET   := libjotunn.so
INCLUDE  := -I./include \

HDRS     := \
			./include/application.h      \
			./include/window.h           \
											     \
			./include/renderer_2d.h      \
			./include/renderable_2d.h    \
			./include/camera.h           \
											     \
			./include/shader.h           \
			./include/vertexarray.h      \
			./include/vertexattribute.h  \
			./include/vertexbuffer.h     \
											     \
			./include/fvector.h          \
			./include/fmatrix.h          \
			./include/fquaternion.h      \

# Platform independent application sources
CORE_SRC := \
			./src/core/application/application.c             \
			./src/core/application/window.c                  \
															             \
			./src/core/graphics/renderer_2d.c                \
			./src/core/graphics/renderable_2d.c              \
			./src/core/graphics/camera.c		                \
															             \
			./src/core/graphics/datatypes/shader.c           \
			./src/core/graphics/datatypes/vertexarray.c      \
			./src/core/graphics/datatypes/vertexattribute.c  \
			./src/core/graphics/datatypes/vertexbuffer.c     \
																          \
			./src/math/fvector.c    				             \
			./src/math/fmatrix.c						             \
			./src/math/fquaternion.c				             \

# Alternate source linking depending on graphical backend
ifeq ($(GRAPHICS_BACKEND), OPENGL)

PLATFORM_SRC := ./src/platform/application/window_gl.c 		            \
					 ./src/platform/graphics/datatypes/shader_gl.c           \
					 ./src/platform/graphics/datatypes/vertexarray_gl.c      \
					 ./src/platform/graphics/datatypes/vertexattribute_gl.c  \
					 ./src/platform/graphics/datatypes/vertexbuffer_gl.c     \

else

PLATFORM_SRC := ./src/platform/application/window_no_backend.c 		           \
					 ./src/platform/graphics/datatypes/shader_no_backend.c           \
					 ./src/platform/graphics/datatypes/vertexarray_no_backend.c      \
					 ./src/platform/graphics/datatypes/vertexattribute_no_backend.c  \
					 ./src/platform/graphics/datatypes/vertexbuffer_no_backend.c     \

endif

# Combine sources
SRC := $(CORE_SRC) $(PLATFORM_SRC) 

#

OBJECTS := $(SRC:%.c=$(OBJ_DIR)/%.o)

all: build $(LIB_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.c $(HDRS)
	@mkdir -p $(@D)
	$(C) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(LIB_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(C) $(CFLAGS) $(CDEFINES) $(INCLUDE) -o $(LIB_DIR)/$(TARGET) $(OBJECTS) $(LDFLAGS)

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