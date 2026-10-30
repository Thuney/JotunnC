#pragma once

#include "fvector.h"

#include <stdint.h>

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// Four unsigned bytes, most often a normalized RGBA8 color. The components
//  mirror fvector4_t's ordering, so for a color w is red, x is green, y is blue
//  and z is alpha. Laid out byte-for-byte in that order, which also matches
//  a packed 32-bit RGBA color on little-endian targets
union u8vector4_t
{
  uint8_t vec[4];

  struct
  {
    uint8_t w, x, y, z;
  } comp;

  /*
   *  < w, x, y, z >
   */
};

/*******************************************************************************
 *
 *   CONVERSIONS
 *
 ******************************************************************************/

// Converts a normalized float vector to bytes, clamping each component to
//  [0, 1] and rounding to the nearest step of 1/255
//
//  in:     in_vector
//  ret:    The byte vector
union u8vector4_t u8vector4_from_fvector4(
  const union fvector4_t in_vector);

// Converts a byte vector to a normalized float vector in [0, 1]
//
//  in:     in_vector
//  ret:    The float vector
union fvector4_t u8vector4_to_fvector4(
  const union u8vector4_t in_vector);
