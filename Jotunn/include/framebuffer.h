#pragma once

#include "texture.h"

#include <stdint.h>

// Components of a framebuffer

struct color_buffer_t
{
    struct texture_2d_t texture;
    // unsigned int texture_id;
};

struct render_buffer_t
{
    unsigned int rbo;
};

//

struct framebuffer_t
{
   unsigned int frame_buffer;
   struct color_buffer_t color_buffer;
   struct render_buffer_t depth_stencil_buffer;
};

//

void framebuffer_init(struct framebuffer_t* frame_buffer, const int width, const int height);

void framebuffer_cleanup(struct framebuffer_t* frame_buffer);

void framebuffer_bind(struct framebuffer_t* frame_buffer);
void framebuffer_unbind();

void framebuffer_resize(struct framebuffer_t* frame_buffer, const int new_width, const int new_height);

void framebuffer_attach_color_buffer(struct framebuffer_t* frame_buffer, struct color_buffer_t* color_buffer);
void framebuffer_attach_render_buffer(struct framebuffer_t* frame_buffer, struct render_buffer_t* render_buffer);

//

void color_buffer_init(struct color_buffer_t* color_buffer, const int width, const int height);
void render_buffer_init(struct render_buffer_t* render_buffer, const int width, const int height);

void color_buffer_resize(struct color_buffer_t* color_buffer, const int new_width, const int new_height);
void render_buffer_resize(struct render_buffer_t* render_buffer, const int new_width, const int new_height);

void color_buffer_cleanup(struct color_buffer_t* color_buffer);
void render_buffer_cleanup(struct render_buffer_t* render_buffer);