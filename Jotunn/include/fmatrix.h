#pragma once

// Square float matrices, stored row major: mat[row][col], with buf holding
//  the same elements row after row. Points are row vectors multiplied on the
//  left of a transform, so a 4x4 transform keeps its translation in the last
//  row, and the product of A and B transforms by A, then by B. Stored this
//  way, a transform can be handed to OpenGL untransposed

#include "fvector.h"

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// A 2x2 matrix
union fmatrix_2x2_t
{
  // Buffer Form
  float buf[4];
  // Matrix form - row major - mat[row][col]
  float mat[2][2];

  /*
     --------------------------------------------
     | mat[0][0] / buf[0] || mat[0][1] / buf[1] |
     --------------------------------------------
     | mat[1][0] / buf[2] || mat[1][1] / buf[3] |
     --------------------------------------------
  */
};

// A 3x3 matrix
union fmatrix_3x3_t
{
  // Buffer Form
  float buf[9];
  // Matrix form - row major - mat[row][col]
  float mat[3][3];
  /*
     ------------------------------------------------------------------
     | mat[0][0] / buf[0] || mat[0][1] / buf[1] || mat[0][2] / buf[2] |
     ------------------------------------------------------------------
     | mat[1][0] / buf[3] || mat[1][1] / buf[4] || mat[1][2] / buf[5] |
     ------------------------------------------------------------------
     | mat[2][0] / buf[6] || mat[2][1] / buf[7] || mat[2][2] / buf[8] |
     ------------------------------------------------------------------
  */
};

// A 4x4 matrix, most often a transform
union fmatrix_4x4_t
{
  // Buffer Form
  float buf[16];
  // Matrix form - row major - mat[row][col]
  float mat[4][4];
  /*
     The buf index of each mat[row][col]:

              col 0   col 1   col 2   col 3
       row 0    0       1       2       3
       row 1    4       5       6       7
       row 2    8       9      10      11
       row 3   12      13      14      15
  */
};

/*******************************************************************************
 *
 *   DEBUGGING
 *
 ******************************************************************************/

#ifdef DEBUG

// Compares two 2x2 matrices element by element, to within 0.001
//
//  in:     in_matrix1, in_matrix2
//  ret:    1 when every element matches; otherwise 0
int fmatrix_2x2_are_equal(
  const union fmatrix_2x2_t* in_matrix1,
  const union fmatrix_2x2_t* in_matrix2);

// Compares two 3x3 matrices element by element, to within 0.001
//
//  in:     in_matrix1, in_matrix2
//  ret:    1 when every element matches; otherwise 0
int fmatrix_3x3_are_equal(
  const union fmatrix_3x3_t* in_matrix1,
  const union fmatrix_3x3_t* in_matrix2);

// Compares two 4x4 matrices element by element, to within 0.001
//
//  in:     in_matrix1, in_matrix2
//  ret:    1 when every element matches; otherwise 0
int fmatrix_4x4_are_equal(
  const union fmatrix_4x4_t* in_matrix1,
  const union fmatrix_4x4_t* in_matrix2);

// Checks whether a 2x2 matrix is the identity, to within 0.0001 per element
//
//  in:     in_matrix
//  ret:    1 when in_matrix is the identity; otherwise 0
int fmatrix_2x2_is_identity(
  const union fmatrix_2x2_t* in_matrix);

// Checks whether a 3x3 matrix is the identity, to within 0.0001 per element
//
//  in:     in_matrix
//  ret:    1 when in_matrix is the identity; otherwise 0
int fmatrix_3x3_is_identity(
  const union fmatrix_3x3_t* in_matrix);

// Checks whether a 4x4 matrix is the identity, to within 0.0001 per element
//
//  in:     in_matrix
//  ret:    1 when in_matrix is the identity; otherwise 0
int fmatrix_4x4_is_identity(
  const union fmatrix_4x4_t* in_matrix);

// Prints each element of a 2x2 matrix to stdout, with its row and column
//
//  in:     in_matrix
void fmatrix_2x2_print(
  const union fmatrix_2x2_t* in_matrix);

// Prints each element of a 3x3 matrix to stdout, with its row and column
//
//  in:     in_matrix
void fmatrix_3x3_print(
  const union fmatrix_3x3_t* in_matrix);

// Prints each element of a 4x4 matrix to stdout, with its row and column
//
//  in:     in_matrix
void fmatrix_4x4_print(
  const union fmatrix_4x4_t* in_matrix);

#endif

/*******************************************************************************
 *
 *   INITIALIZATION
 *
 ******************************************************************************/

// Sets a 2x2 matrix to the identity
//
//  out:    out_matrix
void fmatrix_2x2_identity(
  union fmatrix_2x2_t* out_matrix);

// Sets a 3x3 matrix to the identity
//
//  out:    out_matrix
void fmatrix_3x3_identity(
  union fmatrix_3x3_t* out_matrix);

// Sets a 4x4 matrix to the identity
//
//  out:    out_matrix
void fmatrix_4x4_identity(
  union fmatrix_4x4_t* out_matrix);

// Initializes a 2x2 matrix to the identity
//
//  out:    out_matrix
void fmatrix_2x2_init(
  union fmatrix_2x2_t* out_matrix);

// Initializes a 3x3 matrix to the identity
//
//  out:    out_matrix
void fmatrix_3x3_init(
  union fmatrix_3x3_t* out_matrix);

// Initializes a 4x4 matrix to the identity
//
//  out:    out_matrix
void fmatrix_4x4_init(
  union fmatrix_4x4_t* out_matrix);

// Sets a 2x2 matrix's elements from a row-major array, in_data[row][col]
//
//  in:     in_data
//  out:    out_matrix
void fmatrix_2x2_set(
  union fmatrix_2x2_t* out_matrix,
  const float in_data[2][2]);

// Sets a 3x3 matrix's elements from a row-major array, in_data[row][col]
//
//  in:     in_data
//  out:    out_matrix
void fmatrix_3x3_set(
  union fmatrix_3x3_t* out_matrix,
  const float in_data[3][3]);

// Sets a 4x4 matrix's elements from a row-major array, in_data[row][col]
//
//  in:     in_data
//  out:    out_matrix
void fmatrix_4x4_set(
  union fmatrix_4x4_t* out_matrix,
  const float in_data[4][4]);

/*******************************************************************************
 *
 *   ARITHMETIC
 *
 ******************************************************************************/

// Transposes a 2x2 matrix in place
//
//  in/out: inout_matrix
void fmatrix_2x2_transpose(
  union fmatrix_2x2_t* inout_matrix);

// Transposes a 3x3 matrix in place
//
//  in/out: inout_matrix
void fmatrix_3x3_transpose(
  union fmatrix_3x3_t* inout_matrix);

// Transposes a 4x4 matrix in place
//
//  in/out: inout_matrix
void fmatrix_4x4_transpose(
  union fmatrix_4x4_t* inout_matrix);

// Adds two 2x2 matrices
//
//  in:     in_matrix1, in_matrix2
//  ret:    in_matrix1 + in_matrix2
union fmatrix_2x2_t fmatrix_2x2_add(
  const union fmatrix_2x2_t* in_matrix1,
  const union fmatrix_2x2_t* in_matrix2);

// Adds two 3x3 matrices
//
//  in:     in_matrix1, in_matrix2
//  ret:    in_matrix1 + in_matrix2
union fmatrix_3x3_t fmatrix_3x3_add(
  const union fmatrix_3x3_t* in_matrix1,
  const union fmatrix_3x3_t* in_matrix2);

// Adds two 4x4 matrices
//
//  in:     in_matrix1, in_matrix2
//  ret:    in_matrix1 + in_matrix2
union fmatrix_4x4_t fmatrix_4x4_add(
  const union fmatrix_4x4_t* in_matrix1,
  const union fmatrix_4x4_t* in_matrix2);

// Subtracts one 2x2 matrix from another
//
//  in:     in_matrix1, in_matrix2
//  ret:    in_matrix1 - in_matrix2
union fmatrix_2x2_t fmatrix_2x2_subtract(
  const union fmatrix_2x2_t* in_matrix1,
  const union fmatrix_2x2_t* in_matrix2);

// Subtracts one 3x3 matrix from another
//
//  in:     in_matrix1, in_matrix2
//  ret:    in_matrix1 - in_matrix2
union fmatrix_3x3_t fmatrix_3x3_subtract(
  const union fmatrix_3x3_t* in_matrix1,
  const union fmatrix_3x3_t* in_matrix2);

// Subtracts one 4x4 matrix from another
//
//  in:     in_matrix1, in_matrix2
//  ret:    in_matrix1 - in_matrix2
union fmatrix_4x4_t fmatrix_4x4_subtract(
  const union fmatrix_4x4_t* in_matrix1,
  const union fmatrix_4x4_t* in_matrix2);

// Multiplies every element of a 2x2 matrix by a scalar
//
//  in:     in_matrix, in_scalar
//  ret:    The scaled matrix
union fmatrix_2x2_t fmatrix_2x2_scale(
  const union fmatrix_2x2_t* in_matrix,
  float in_scalar);

// Multiplies every element of a 3x3 matrix by a scalar
//
//  in:     in_matrix, in_scalar
//  ret:    The scaled matrix
union fmatrix_3x3_t fmatrix_3x3_scale(
  const union fmatrix_3x3_t* in_matrix,
  float in_scalar);

// Multiplies every element of a 4x4 matrix by a scalar
//
//  in:     in_matrix, in_scalar
//  ret:    The scaled matrix
union fmatrix_4x4_t fmatrix_4x4_scale(
  const union fmatrix_4x4_t* in_matrix,
  float in_scalar);

// Multiplies two 2x2 matrices; as transforms, the product applies
//  in_matrix1, then in_matrix2
//
//  in:     in_matrix1, in_matrix2
//  ret:    in_matrix1 * in_matrix2
union fmatrix_2x2_t fmatrix_2x2_multiply(
  const union fmatrix_2x2_t* in_matrix1,
  const union fmatrix_2x2_t* in_matrix2);

// Multiplies two 3x3 matrices; as transforms, the product applies
//  in_matrix1, then in_matrix2
//
//  in:     in_matrix1, in_matrix2
//  ret:    in_matrix1 * in_matrix2
union fmatrix_3x3_t fmatrix_3x3_multiply(
  const union fmatrix_3x3_t* in_matrix1,
  const union fmatrix_3x3_t* in_matrix2);

// Multiplies two 4x4 matrices; as transforms, the product applies
//  in_matrix1, then in_matrix2
//
//  in:     in_matrix1, in_matrix2
//  ret:    in_matrix1 * in_matrix2
union fmatrix_4x4_t fmatrix_4x4_multiply(
  const union fmatrix_4x4_t* in_matrix1,
  const union fmatrix_4x4_t* in_matrix2);

/*******************************************************************************
 *
 *   TRANSFORMS
 *
 ******************************************************************************/

// Follows a transform with a translation
//
//  in:     in_matrix, in_translation
//  ret:    A transform applying in_matrix, then the translation
union fmatrix_4x4_t fmatrix_4x4_transform_translate(
  const union fmatrix_4x4_t* in_matrix,
  union fvector3_t in_translation);

// Follows a transform with a scaling along each axis
//
//  in:     in_matrix, in_scale_factors
//  ret:    A transform applying in_matrix, then the scaling
union fmatrix_4x4_t fmatrix_4x4_transform_scale(
  const union fmatrix_4x4_t* in_matrix,
  union fvector3_t in_scale_factors);

// Follows a transform with a rotation about an axis through the origin,
//  counter-clockwise when looking down the axis towards the origin. The axis
//  needn't be of unit length
//
//  in:     in_matrix, in_axis, in_angle_radians
//  ret:    A transform applying in_matrix, then the rotation; in_matrix
//            unchanged when in_axis is the zero vector
union fmatrix_4x4_t fmatrix_4x4_transform_rotate(
  const union fmatrix_4x4_t* in_matrix,
  union fvector3_t in_axis,
  float in_angle_radians);

// Builds the transform that places a primitive drawn about the origin: a
//  scaling along each axis, then a rotation about an axis through the origin
//  as fmatrix_4x4_transform_rotate() makes, then a translation
//
//  in:     in_scale_factors, in_rotation_axis, in_rotation_radians,
//            in_translation
//  ret:    The transform
union fmatrix_4x4_t fmatrix_4x4_transform_compose(
  union fvector3_t in_scale_factors,
  union fvector3_t in_rotation_axis,
  float in_rotation_radians,
  union fvector3_t in_translation);

// Builds the transform that places a primitive in the XY plane: a
//  translation taking in_pivot to the origin, a scaling, a counter-clockwise
//  rotation about Z, then a translation to in_position. in_pivot is the
//  point of the primitive, before any transform, that it's scaled and turned
//  about and that ends up at in_position: (0.5, 0.5) for the middle of
//  renderer_2d's unit square, or (0, 0) for its corner or the center of its
//  unit circle. Z is left as it is
//
//  in:     in_pivot, in_scale_factors, in_rotation_radians, in_position
//  ret:    The transform
union fmatrix_4x4_t fmatrix_4x4_transform_compose_2d(
  union fvector2_t in_pivot,
  union fvector2_t in_scale_factors,
  float in_rotation_radians,
  union fvector2_t in_position);

// Transforms a point, as the row vector (x, y, z, 1). The result isn't
//  divided by its fourth component, so this suits affine transforms rather
//  than projections
//
//  in:     in_matrix, in_point
//  ret:    The transformed point
union fvector3_t fmatrix_4x4_transform_point(
  const union fmatrix_4x4_t* in_matrix,
  union fvector3_t in_point);
