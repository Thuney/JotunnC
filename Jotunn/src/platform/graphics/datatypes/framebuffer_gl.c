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

extern void platform_color_buffer_init(struct color_buffer_t* color_buffer, const int width, const int height)
{
    glGenTextures(1, &color_buffer->texture_id);
    glBindTexture(GL_TEXTURE_2D, color_buffer->texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    #ifdef DEBUG
        fprintf(stdout, "Initialized color buffer texture with ID: %d\n", color_buffer->texture_id);
    #endif
}

extern void platform_depth_buffer_init(struct depth_buffer_t* depth_buffer, const int width, const int height)
{
    glGenTextures(1, &depth_buffer->texture_id);
    glBindTexture(GL_TEXTURE_2D, depth_buffer->texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    #ifdef DEBUG
        fprintf(stdout, "Initialized depth buffer texture with ID: %d\n", depth_buffer->texture_id);
    #endif
}

extern void platform_stencil_buffer_init(struct stencil_buffer_t* stencil_buffer, const int width, const int height)
{
    glGenTextures(1, &stencil_buffer->texture_id);
    glBindTexture(GL_TEXTURE_2D, stencil_buffer->texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX, width, height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    #ifdef DEBUG
        fprintf(stdout, "Initialized stencil buffer texture with ID: %d\n", stencil_buffer->texture_id);
    #endif
}


void platform_framebuffer_attach_color_buffer(struct framebuffer_t* frame_buffer, struct color_buffer_t* color_buffer)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer->texture_id, 0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        #ifdef DEBUG
            fprintf(stdout, "Error occurred during color buffer attachment\n");
        #endif
    }

    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer->texture.texture_id, 0);
}

void platform_framebuffer_attach_depth_buffer(struct framebuffer_t* frame_buffer, struct depth_buffer_t* depth_buffer)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_buffer->texture_id, 0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        #ifdef DEBUG
            fprintf(stdout, "Error occurred during depth buffer attachment\n");
        #endif
    }
    
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_buffer->texture.texture_id, 0);
}

void platform_framebuffer_attach_stencil_buffer(struct framebuffer_t* frame_buffer, struct stencil_buffer_t* stencil_buffer)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencil_buffer->texture_id, 0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        #ifdef DEBUG
            fprintf(stdout, "Error occurred during stencil buffer attachment\n");
        #endif
    }

    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencil_buffer->texture.texture_id, 0);
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
    glDeleteRenderbuffers(1, &(frame_buffer->rbo));
    glDeleteFramebuffers(1, &(frame_buffer->frame_buffer));
}