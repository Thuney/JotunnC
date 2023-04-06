#include "framebuffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef DEBUG
   #include <stdio.h>
#endif

void platform_framebuffer_init(struct framebuffer_t* frame_buffer)
{
    glGenFramebuffers(1, &(frame_buffer->frame_buffer));
}

void platform_framebuffer_attach_color_buffer(struct framebuffer_t* frame_buffer, struct color_buffer_t* color_buffer)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer->texture.texture_id, 0);
}

void platform_framebuffer_attach_depth_buffer(struct framebuffer_t* frame_buffer, struct depth_buffer_t* depth_buffer)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_buffer->texture.texture_id, 0);
}

void platform_framebuffer_attach_stencil_buffer(struct framebuffer_t* frame_buffer, struct stencil_buffer_t* stencil_buffer)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencil_buffer->texture.texture_id, 0);
}

void platform_framebuffer_bind(struct framebuffer_t* frame_buffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->frame_buffer);
}

void platform_framebuffer_unbind()
{
    // Binds back to what should be the default framebuffer (the framebuffer of the first window spawned)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void platform_framebuffer_cleanup(struct framebuffer_t* frame_buffer)
{
    glDeleteFramebuffers(1, &(frame_buffer->frame_buffer));
}