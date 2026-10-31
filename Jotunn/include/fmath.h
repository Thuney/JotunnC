#pragma once

// Float scalar maths shared across Jotunn and the applications built on it:
//  the constant pi, limiting a value to a range, and converting angles

#define FMATH_PI 3.14159265358979f

/*******************************************************************************
 *
 *   RANGES
 *
 ******************************************************************************/

// Limits a value to a range
//
//  in:     in_value, in_min, in_max
//  ret:    in_value, or the nearer limit when it's outside them
float fmath_clamp(
  float in_value,
  float in_min,
  float in_max);

/*******************************************************************************
 *
 *   ANGLES
 *
 ******************************************************************************/

// Converts an angle from degrees to radians
//
//  in:     in_angle_degrees
//  ret:    The angle in radians
float fmath_degrees_to_radians(
  float in_angle_degrees);

// Converts an angle from radians to degrees
//
//  in:     in_angle_radians
//  ret:    The angle in degrees
float fmath_radians_to_degrees(
  float in_angle_radians);
