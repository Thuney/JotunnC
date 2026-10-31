#include "color.h"

#include <math.h>

/*******************************************************************************
 *
 *   Function: color_from_rgb()
 *
 ******************************************************************************/
union fvector4_t color_from_rgb(
  union fvector3_t in_rgb,
  float in_alpha)
{
  const union fvector4_t color = {{in_rgb.vec[0], in_rgb.vec[1], in_rgb.vec[2], in_alpha}};

  return color;
}

/*******************************************************************************
 *
 *   Function: color_from_hsv()
 *
 ******************************************************************************/
union fvector4_t color_from_hsv(
  float in_hue,
  float in_saturation,
  float in_value,
  float in_alpha)
{
  // Which sixth of the color wheel the hue falls in, and how far into it
  const float sector = ((in_hue - floorf(in_hue)) * 6.0f);
  const float chroma = (in_value * in_saturation);
  const float second_largest = (chroma * (1.0f - fabsf(fmodf(sector, 2.0f) - 1.0f)));
  const float lightness_offset = (in_value - chroma);

  float red = 0.0f;
  float green = 0.0f;
  float blue = 0.0f;

  switch ((int)sector)
  {
    case 0:
    {
      red = chroma;
      green = second_largest;
    }
    break;

    case 1:
    {
      red = second_largest;
      green = chroma;
    }
    break;

    case 2:
    {
      green = chroma;
      blue = second_largest;
    }
    break;

    case 3:
    {
      green = second_largest;
      blue = chroma;
    }
    break;

    case 4:
    {
      red = second_largest;
      blue = chroma;
    }
    break;

    // The last sixth, and a hue rounded up to a whole turn
    default:
    {
      red = chroma;
      blue = second_largest;
    }
    break;
  }

  const union fvector4_t color =
    {{(red + lightness_offset), (green + lightness_offset), (blue + lightness_offset), in_alpha}};

  return color;
}
