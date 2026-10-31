#pragma once

// What the 2D renderer example draws with besides shapes: a texture loaded
//  from an image file, a texture made in code, and a typeface for text.
//  Textures and a typeface's glyph atlas live in graphics memory, belonging
//  to the graphics context current as they're made, so they're loaded with
//  their window's context current and drawn only in that window

#include "font.h"
#include "texture.h"

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

struct renderer_2d_app_resources_t
{
  // Jotunn's placeholder texture when its file can't be read
  struct texture_2d_t image_texture;
  struct texture_2d_t checker_texture;
  struct font_typeface_t typeface;
};

/*******************************************************************************
 *
 *   LIFETIME
 *
 ******************************************************************************/

// Loads the image, makes the checkerboard texture and loads the typeface, in
//  the current graphics context. An image that can't be loaded is reported
//  on stderr, and replaced with Jotunn's placeholder texture
//
//  out:    out_resources
void renderer_2d_app_resources_load(
  struct renderer_2d_app_resources_t* out_resources);

// Releases what renderer_2d_app_resources_load() made. The graphics context
//  they were made in must be current
//
//  in/out: inout_resources
void renderer_2d_app_resources_release(
  struct renderer_2d_app_resources_t* inout_resources);
