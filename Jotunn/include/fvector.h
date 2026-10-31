#pragma once

// Small float vectors, passed by const pointer to the arithmetic, which
//  returns new vectors by value

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// A 2D vector
union fvector2_t
{
  float vec[2];

  struct
  {
    float x, y;
  } comp;

  /*
   *  < x, y >
   */
};

// A 3D vector
union fvector3_t
{
  float vec[3];

  struct
  {
    float x, y, z;
  } comp;

  /*
   *  < x, y, z >
   */
};

// A 4D vector, with its components ordered w, x, y, z
union fvector4_t
{
  float vec[4];

  struct
  {
    float w, x, y, z;
  } comp;

  /*
   *  < w, x, y, z >
   */
};

/*******************************************************************************
 *
 *   DEBUGGING
 *
 ******************************************************************************/

#ifdef DEBUG

  #include <stdio.h>

// Prints a 2D vector's components to stdout
//
//  in:     in_vector
void fvector2_print(
  const union fvector2_t* in_vector);

// Prints a 3D vector's components to stdout
//
//  in:     in_vector
void fvector3_print(
  const union fvector3_t* in_vector);

// Prints a 4D vector's components to stdout
//
//  in:     in_vector
void fvector4_print(
  const union fvector4_t* in_vector);

#endif

/*******************************************************************************
 *
 *   INITIALIZATION
 *
 ******************************************************************************/

// Zeroes a 2D vector
//
//  out:    out_vector
void fvector2_init(
  union fvector2_t* out_vector);

// Zeroes a 3D vector
//
//  out:    out_vector
void fvector3_init(
  union fvector3_t* out_vector);

// Zeroes a 4D vector
//
//  out:    out_vector
void fvector4_init(
  union fvector4_t* out_vector);

// Sets a 2D vector's components
//
//  in:     in_x, in_y
//  out:    out_vector
void fvector2_set(
  union fvector2_t* out_vector,
  const float in_x,
  const float in_y);

// Sets a 3D vector's components
//
//  in:     in_x, in_y, in_z
//  out:    out_vector
void fvector3_set(
  union fvector3_t* out_vector,
  const float in_x,
  const float in_y,
  const float in_z);

// Sets a 4D vector's components
//
//  in:     in_w, in_x, in_y, in_z
//  out:    out_vector
void fvector4_set(
  union fvector4_t* out_vector,
  const float in_w,
  const float in_x,
  const float in_y,
  const float in_z);

// Copies a 2D vector
//
//  in:     in_vector
//  out:    out_vector
void fvector2_setv(
  union fvector2_t* out_vector,
  const union fvector2_t in_vector);

// Copies a 3D vector
//
//  in:     in_vector
//  out:    out_vector
void fvector3_setv(
  union fvector3_t* out_vector,
  const union fvector3_t in_vector);

// Copies a 4D vector
//
//  in:     in_vector
//  out:    out_vector
void fvector4_setv(
  union fvector4_t* out_vector,
  const union fvector4_t in_vector);

/*******************************************************************************
 *
 *   ARITHMETIC
 *
 ******************************************************************************/

// Measures a 2D vector's length
//
//  in:     in_vector
//  ret:    The vector's Euclidean length
float fvector2_magnitude(
  const union fvector2_t* in_vector);

// Measures a 3D vector's length
//
//  in:     in_vector
//  ret:    The vector's Euclidean length
float fvector3_magnitude(
  const union fvector3_t* in_vector);

// Measures a 4D vector's length
//
//  in:     in_vector
//  ret:    The vector's Euclidean length
float fvector4_magnitude(
  const union fvector4_t* in_vector);

// Scales a 2D vector to unit length. A zero vector gives non-finite
//  components
//
//  in:     in_vector
//  ret:    The unit vector
union fvector2_t fvector2_normalize(
  const union fvector2_t* in_vector);

// Scales a 3D vector to unit length. A zero vector gives non-finite
//  components
//
//  in:     in_vector
//  ret:    The unit vector
union fvector3_t fvector3_normalize(
  const union fvector3_t* in_vector);

// Scales a 4D vector to unit length. A zero vector gives non-finite
//  components
//
//  in:     in_vector
//  ret:    The unit vector
union fvector4_t fvector4_normalize(
  const union fvector4_t* in_vector);

// Adds two 2D vectors
//
//  in:     in_vector1, in_vector2
//  ret:    in_vector1 + in_vector2
union fvector2_t fvector2_add(
  const union fvector2_t* in_vector1,
  const union fvector2_t* in_vector2);

// Adds two 3D vectors
//
//  in:     in_vector1, in_vector2
//  ret:    in_vector1 + in_vector2
union fvector3_t fvector3_add(
  const union fvector3_t* in_vector1,
  const union fvector3_t* in_vector2);

// Adds two 4D vectors
//
//  in:     in_vector1, in_vector2
//  ret:    in_vector1 + in_vector2
union fvector4_t fvector4_add(
  const union fvector4_t* in_vector1,
  const union fvector4_t* in_vector2);

// Subtracts one 2D vector from another
//
//  in:     in_vector1, in_vector2
//  ret:    in_vector1 - in_vector2
union fvector2_t fvector2_subtract(
  const union fvector2_t* in_vector1,
  const union fvector2_t* in_vector2);

// Subtracts one 3D vector from another
//
//  in:     in_vector1, in_vector2
//  ret:    in_vector1 - in_vector2
union fvector3_t fvector3_subtract(
  const union fvector3_t* in_vector1,
  const union fvector3_t* in_vector2);

// Subtracts one 4D vector from another
//
//  in:     in_vector1, in_vector2
//  ret:    in_vector1 - in_vector2
union fvector4_t fvector4_subtract(
  const union fvector4_t* in_vector1,
  const union fvector4_t* in_vector2);

// Multiplies each component of a 2D vector by a scalar
//
//  in:     in_vector, in_scalar
//  ret:    The scaled vector
union fvector2_t fvector2_scale(
  const union fvector2_t* in_vector,
  const float in_scalar);

// Multiplies each component of a 3D vector by a scalar
//
//  in:     in_vector, in_scalar
//  ret:    The scaled vector
union fvector3_t fvector3_scale(
  const union fvector3_t* in_vector,
  const float in_scalar);

// Multiplies each component of a 4D vector by a scalar
//
//  in:     in_vector, in_scalar
//  ret:    The scaled vector
union fvector4_t fvector4_scale(
  const union fvector4_t* in_vector,
  const float in_scalar);

// Takes the dot product of two 2D vectors
//
//  in:     in_vector1, in_vector2
//  ret:    in_vector1 . in_vector2
float fvector2_dot(
  const union fvector2_t* in_vector1,
  const union fvector2_t* in_vector2);

// Takes the dot product of two 3D vectors
//
//  in:     in_vector1, in_vector2
//  ret:    in_vector1 . in_vector2
float fvector3_dot(
  const union fvector3_t* in_vector1,
  const union fvector3_t* in_vector2);

// Takes the cross product of two 3D vectors
//
//  in:     in_vector1, in_vector2
//  ret:    in_vector1 x in_vector2
union fvector3_t fvector3_cross(
  const union fvector3_t* in_vector1,
  const union fvector3_t* in_vector2);
