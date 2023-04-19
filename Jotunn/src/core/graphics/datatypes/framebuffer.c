#include "framebuffer.h"

#include <memory.h>

#ifdef DEBUG
    #include <stdio.h>
#endif

#include <stdlib.h>

extern void platform_framebuffer_init(struct framebuffer_t* frame_buffer);
extern void platform_framebuffer_attach_color_buffer(struct framebuffer_t* frame_buffer, struct color_buffer_t* color_buffer);
extern void platform_framebuffer_attach_depth_buffer(struct framebuffer_t* frame_buffer, struct depth_buffer_t* depth_buffer);
extern void platform_framebuffer_attach_stencil_buffer(struct framebuffer_t* frame_buffer, struct stencil_buffer_t* stencil_buffer);
extern void platform_framebuffer_bind(struct framebuffer_t* frame_buffer);
extern void platform_framebuffer_unbind();
extern void platform_framebuffer_cleanup(struct framebuffer_t* frame_buffer);

void framebuffer_init(struct framebuffer_t* frame_buffer, const int width, const int height)
{
    #ifdef DEBUG
        fprintf(stdout, "Initializing frame buffer\n");
    #endif

    platform_framebuffer_init(frame_buffer);

    #ifdef DEBUG
        fprintf(stdout, "Binding frame buffer\n");
    #endif

    framebuffer_bind(frame_buffer);

    color_buffer_init(&(frame_buffer->color_buffer), width, height);
    depth_buffer_init(&(frame_buffer->depth_buffer), width, height);
    stencil_buffer_init(&(frame_buffer->stencil_buffer), width, height);

    framebuffer_attach_color_buffer(frame_buffer, &(frame_buffer->color_buffer));
    framebuffer_attach_depth_buffer(frame_buffer, &(frame_buffer->depth_buffer));
    framebuffer_attach_stencil_buffer(frame_buffer, &(frame_buffer->stencil_buffer));

    framebuffer_unbind();
}

void framebuffer_resize(struct framebuffer_t* frame_buffer, const int new_width, const int new_height)
{
    texture_2d_resize(&(frame_buffer->color_buffer.texture), new_width, new_height);
    texture_2d_resize(&(frame_buffer->depth_buffer.texture), new_width, new_height);
    texture_2d_resize(&(frame_buffer->stencil_buffer.texture), new_width, new_height);
}

void framebuffer_attach_color_buffer(struct framebuffer_t* frame_buffer, struct color_buffer_t* color_buffer)
{
    platform_framebuffer_attach_color_buffer(frame_buffer, color_buffer);
}

void framebuffer_attach_depth_buffer(struct framebuffer_t* frame_buffer, struct depth_buffer_t* depth_buffer)
{
    platform_framebuffer_attach_depth_buffer(frame_buffer, depth_buffer);
}

void framebuffer_attach_stencil_buffer(struct framebuffer_t* frame_buffer, struct stencil_buffer_t* stencil_buffer)
{
    platform_framebuffer_attach_stencil_buffer(frame_buffer, stencil_buffer);
}

void framebuffer_bind(struct framebuffer_t* frame_buffer)
{
    platform_framebuffer_bind(frame_buffer);
}

void framebuffer_unbind()
{
    platform_framebuffer_unbind();
}

void framebuffer_cleanup(struct framebuffer_t* frame_buffer)
{
    platform_framebuffer_cleanup(frame_buffer);

    color_buffer_cleanup(&(frame_buffer->color_buffer));
    depth_buffer_cleanup(&(frame_buffer->depth_buffer));
    stencil_buffer_cleanup(&(frame_buffer->stencil_buffer));
}

// 

void color_buffer_init(struct color_buffer_t* color_buffer, const int width, const int height)
{
    #ifdef DEBUG
        fprintf(stdout, "Initializing color buffer\n");
    #endif

    texture_2d_init(&color_buffer->texture, width, height, TEXTURE_2D_INTERNAL_FORMAT_RGB8, false);
    uint32_t buffer_size_bytes = (height*width*color_buffer->texture.channels);
    color_buffer->texture.data = (unsigned char*) calloc(buffer_size_bytes, sizeof(unsigned char));
}

void depth_buffer_init(struct depth_buffer_t* depth_buffer, const int width, const int height)
{
    #ifdef DEBUG
        fprintf(stdout, "Initializing depth buffer\n");
    #endif

    texture_2d_init(&depth_buffer->texture, width, height, TEXTURE_2D_FORMAT_DEPTH_COMPONENT, false);
    uint32_t buffer_size_bytes = (height*width*depth_buffer->texture.channels);
    depth_buffer->texture.data = (unsigned char*) calloc(buffer_size_bytes, sizeof(unsigned char));
}

void stencil_buffer_init(struct stencil_buffer_t* stencil_buffer, const int width, const int height)
{
    #ifdef DEBUG
        fprintf(stdout, "Initializing stencil buffer\n");
    #endif

    texture_2d_init(&stencil_buffer->texture, width, height, TEXTURE_2D_FORMAT_STENCIL_INDEX, false);
    uint32_t buffer_size_bytes = (height*width*stencil_buffer->texture.channels);
    stencil_buffer->texture.data = (unsigned char*) calloc(buffer_size_bytes, sizeof(unsigned char));
}

void color_buffer_cleanup(struct color_buffer_t* color_buffer)
{
    #ifdef DEBUG
        fprintf(stdout, "Cleaning up color buffer\n");
    #endif

    free(color_buffer->texture.data);
    color_buffer->texture.data = 0;
    texture_2d_cleanup(&(color_buffer->texture));
}

void depth_buffer_cleanup(struct depth_buffer_t* depth_buffer)
{
    #ifdef DEBUG
        fprintf(stdout, "Cleaning up depth buffer\n");
    #endif

    free(depth_buffer->texture.data);
    depth_buffer->texture.data = 0;
    texture_2d_cleanup(&(depth_buffer->texture));
}

void stencil_buffer_cleanup(struct stencil_buffer_t* stencil_buffer)
{
    #ifdef DEBUG
        fprintf(stdout, "Cleaning up stencil buffer\n");
    #endif

    free(stencil_buffer->texture.data);
    stencil_buffer->texture.data = 0;
    texture_2d_cleanup(&(stencil_buffer->texture));
}
