#pragma once

#include "framebuffer.h"
#include "renderer.h"

struct window_layer_t
{
    struct framebuffer_t framebuffer;
    //
    struct camera_base_t* camera;
    struct renderer_base_t* renderer;
};