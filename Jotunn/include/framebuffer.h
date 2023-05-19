#pragma once

#include "texture.h"

#include <stdint.h>

// Components of a framebuffer

struct color_buffer_t
{
    // Underlying texture storage
    // struct texture_2d_t texture;
    unsigned int texture_id;
};

struct depth_buffer_t
{
    // Underlying texture storage
    // struct texture_2d_t texture;
    unsigned int texture_id;
};

struct stencil_buffer_t
{
    // Underlying texture storage
    // struct texture_2d_t texture;
    unsigned int texture_id;
};

//

struct framebuffer_t
{
   unsigned int frame_buffer;
   unsigned int rbo;
   struct color_buffer_t color_buffer;
   struct depth_buffer_t depth_buffer;
   struct stencil_buffer_t stencil_buffer;
};

//

void framebuffer_init(struct framebuffer_t* frame_buffer, const int width, const int height);

// Probably doesn't work tbh
void framebuffer_resize(struct framebuffer_t* frame_buffer, const int new_width, const int new_height);

void framebuffer_attach_color_buffer(struct framebuffer_t* frame_buffer, struct color_buffer_t* color_buffer);
void framebuffer_attach_depth_buffer(struct framebuffer_t* frame_buffer, struct depth_buffer_t* depth_buffer);
void framebuffer_attach_stencil_buffer(struct framebuffer_t* frame_buffer, struct stencil_buffer_t* stencil_buffer);

void framebuffer_bind(struct framebuffer_t* frame_buffer);
void framebuffer_unbind();

void framebuffer_cleanup(struct framebuffer_t* frame_buffer);

//

void color_buffer_init(struct color_buffer_t* color_buffer, const int width, const int height);
void depth_buffer_init(struct depth_buffer_t* depth_buffer, const int width, const int height);
void stencil_buffer_init(struct stencil_buffer_t* stencil_buffer, const int width, const int height);

void color_buffer_cleanup(struct color_buffer_t* color_buffer);
void depth_buffer_cleanup(struct depth_buffer_t* depth_buffer);
void stencil_buffer_cleanup(struct stencil_buffer_t* stencil_buffer);