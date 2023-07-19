C        := -gcc
CFLAGS   := -g -pedantic-errors -Wall -std=gnu99 -D_POSIX_SOURCE -D_GNU_SOURCE -Wextra -fPIC
CDEFINES := 

GRAPHICS_BACKEND := OPENGL

ifeq ($(GRAPHICS_BACKEND), OPENGL)

LD_GRAPHICS_FLAGS := -lGL -lglfw -lGLEW
CDEFINES := $(CDEFINES) -DBACKEND_OPENGL 

endif

LDFLAGS  := -L/usr/lib/ -lm -lfreetype -lpthread -lrt $(LD_GRAPHICS_FLAGS) -shared

FREETYPE2_INCLUDE := -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sysprof-4

BUILD    := ./.build
OBJ_DIR  := $(BUILD)/objects
LIB_DIR  := $(BUILD)/lib
TARGET   := libjotunn.so
INCLUDE  := -I./include -I./res $(FREETYPE2_INCLUDE)

HDRS := \
./include/application.h               \
./include/event.h                     \
./include/timestep.h                  \
./include/window.h                    \
./include/window_layer.h              \
./include/ui_layer.h              	  \
\
./include/util.h                      \
\
./include/render_api.h                \
./include/renderer.h                  \
./include/renderer_2d.h               \
./include/renderer_window_layer.h	  \
./include/renderable_2d_primitive.h   \
./include/camera.h                    \
\
./include/font.h                      \
./include/framebuffer.h               \
./include/shader.h                    \
./include/texture.h                   \
./include/vertexarray.h               \
./include/vertexattribute.h           \
./include/vertexbuffer.h              \
\
./include/fvector.h                   \
./include/fmatrix.h                   \
./include/fquaternion.h               \

# Platform independent application sources
CORE_SRC := \
./src/core/application/application.c             \
./src/core/application/timestep.c                \
./src/core/application/window.c                  \
./src/core/application/window_layer.c            \
./src/core/application/ui_layer.c           	 \
\
./src/core/utils/util.c                          \
\
./src/core/graphics/api/render_api.c             \
./src/core/graphics/renderer.c                   \
./src/core/graphics/renderer_2d.c                \
./src/core/graphics/renderer_window_layer.c		 \
./src/core/graphics/renderable_2d_primitive.c    \
./src/core/graphics/camera.c                     \
\
./src/core/graphics/datatypes/font.c             \
./src/core/graphics/datatypes/framebuffer.c      \
./src/core/graphics/datatypes/shader.c           \
./src/core/graphics/datatypes/texture.c          \
./src/core/graphics/datatypes/vertexarray.c      \
./src/core/graphics/datatypes/vertexattribute.c  \
./src/core/graphics/datatypes/vertexbuffer.c     \
\
./src/math/fvector.c                             \
./src/math/fmatrix.c                             \
./src/math/fquaternion.c                         \

# Alternate source linking depending on graphical backend
ifeq ($(GRAPHICS_BACKEND), OPENGL)

PLATFORM_SRC := \
./src/platform/application/window_gl.c                 \
./src/platform/graphics/api/render_api_opengl.c        \
./src/platform/graphics/datatypes/framebuffer_gl.c     \
./src/platform/graphics/datatypes/shader_gl.c          \
./src/platform/graphics/datatypes/texture_gl.c         \
./src/platform/graphics/datatypes/vertexarray_gl.c     \
./src/platform/graphics/datatypes/vertexattribute_gl.c \
./src/platform/graphics/datatypes/vertexbuffer_gl.c    \

else

PLATFORM_SRC := \
./src/platform/application/window_no_backend.c                  \
./src/platform/graphics/api/render_api_no_backend.c             \
./src/platform/graphics/datatypes/framebuffer_no_backend.c      \
./src/platform/graphics/datatypes/shader_no_backend.c           \
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