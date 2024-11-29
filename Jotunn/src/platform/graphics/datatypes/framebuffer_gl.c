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

void platform_color_buffer_init(struct color_buffer_t* color_buffer, const int width, const int height)
{
    texture_2d_init(&color_buffer->texture, width, height, TEXTURE_2D_INTERNAL_FORMAT_RGBA8, false);
}

void platform_render_buffer_init(struct render_buffer_t* render_buffer, const int width, const int height)
{
    glGenRenderbuffers(1, &(render_buffer->rbo));
    glBindRenderbuffer(GL_RENDERBUFFER, (render_buffer->rbo));

    // Use a single renderbuffer object for both a depth AND stencil buffer
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}

extern void platform_color_buffer_cleanup(struct color_buffer_t* color_buffer)
{
    texture_2d_cleanup(&color_buffer->texture);
}

extern void platform_render_buffer_cleanup(struct render_buffer_t* render_buffer)
{
    glDeleteRenderbuffers(1, &(render_buffer->rbo));
}


void platform_color_buffer_resize(struct color_buffer_t* color_buffer, const int new_width, const int new_height)
{
    texture_2d_bind(&color_buffer->texture, 0);
    texture_2d_resize(&color_buffer->texture, new_width, new_height);
}

void platform_render_buffer_resize(struct render_buffer_t* render_buffer, const int new_width, const int new_height)
{
    glBindRenderbuffer(GL_RENDERBUFFER, (render_buffer->rbo));
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, new_width, new_height);
}

void platform_framebuffer_attach_color_buffer(struct framebuffer_t* frame_buffer, struct color_buffer_t* color_buffer)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer->texture.texture_id, 0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        #ifdef DEBUG
            fprintf(stdout, "Error occurred during color buffer attachment - %d\n", error);
        #endif
    }
}

void platform_framebuffer_attach_render_buffer(struct framebuffer_t* frame_buffer, struct render_buffer_t* render_buffer)
{
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, (render_buffer->rbo));

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        #ifdef DEBUG
            fprintf(stdout, "Error occurred during render buffer attachment\n");
        #endif
    }
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