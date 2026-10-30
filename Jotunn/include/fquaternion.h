#pragma once

// Float quaternions

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// A quaternion, with its components ordered w, x, y, z
union fquaternion_t
{
  float quat[4];

  struct fquaternion_components_t
  {
    float w, x, y, z;
  } comp;
};

/*******************************************************************************
 *
 *   INITIALIZATION
 *
 ******************************************************************************/

// Zeroes a quaternion
//
//  out:    out_quaternion
void fquaternion_init(union fquaternion_t* out_quaternion);
