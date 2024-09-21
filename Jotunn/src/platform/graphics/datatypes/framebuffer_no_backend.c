#include "framebuffer.h"

void platform_framebuffer_init(struct framebuffer_t* frame_buffer)
{
    return;
}

void platform_color_buffer_init(struct color_buffer_t* color_buffer, const int width, const int height)
{
    return;
}

void platform_render_buffer_init(struct render_buffer_t* render_buffer, const int width, const int height)
{
    return;
}

void platform_color_buffer_resize(struct color_buffer_t* color_buffer, const int new_width, const int new_height)
{
    return;
}

void platform_render_buffer_resize(struct render_buffer_t* render_buffer, const int new_width, const int new_height)
{
    return;
}

void platform_framebuffer_attach_color_buffer(struct framebuffer_t* frame_buffer, struct color_buffer_t* color_buffer)
{
    return;
}

void platform_framebuffer_attach_render_buffer(struct framebuffer_t* frame_buffer, struct render_buffer_t* render_buffer)
{
    return;
}

void platform_framebuffer_bind(struct framebuffer_t* frame_buffer)
{
    return;
}

void platform_framebuffer_unbind()
{
    return;
}

void platform_framebuffer_cleanup(struct framebuffer_t* frame_buffer)
{
    return;
}