#include "fmath.h"

#include <math.h>

/*******************************************************************************
 *
 *   Function: fmath_clamp()
 *
 ******************************************************************************/
float fmath_clamp(
  float in_value,
  float in_min,
  float in_max)
{
  const float clamped_value =
    fminf(
      fmaxf(
        in_value,
        in_min),
      in_max);

  return clamped_value;
}

/*******************************************************************************
 *
 *   Function: fmath_degrees_to_radians()
 *
 ******************************************************************************/
float fmath_degrees_to_radians(
  float in_angle_degrees)
{
  return (in_angle_degrees * (FMATH_PI / 180.0f));
}

/*******************************************************************************
 *
 *   Function: fmath_radians_to_degrees()
 *
 ******************************************************************************/
float fmath_radians_to_degrees(
  float in_angle_radians)
{
  return (in_angle_radians * (180.0f / FMATH_PI));
}
