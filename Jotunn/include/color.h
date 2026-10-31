#pragma once

// Making colors as the renderers take them: a union fvector4_t holding red,
//  green, blue and alpha, each from 0 to 1, in vec[0] to vec[3]. Its
//  components are named w, x, y and z, so for a color w is red, x is green,
//  y is blue and z is alpha, as with union u8vector4_t

#include "fvector.h"

/*******************************************************************************
 *
 *   CONVERSIONS
 *
 ******************************************************************************/

// Makes a color from its red, green and blue, in in_rgb's vec[0] to vec[2],
//  and its alpha. A color edited as three floats, as with ImGui's
//  igColorEdit3(), can be kept in a union fvector3_t and edited through vec
//
//  in:     in_rgb, in_alpha
//  ret:    The color
union fvector4_t color_from_rgb(
  union fvector3_t in_rgb,
  float in_alpha);

// Makes a color from its hue, saturation and value, and its alpha. The hue
//  goes once around the color wheel from 0 to 1, from red through green and
//  blue back to red, and wraps outside that range
//
//  in:     in_hue, in_saturation, in_value, in_alpha
//  ret:    The color
union fvector4_t color_from_hsv(
  float in_hue,
  float in_saturation,
  float in_value,
  float in_alpha);
