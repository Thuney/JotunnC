#include "fmatrix.h"

#include <memory.h>

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Sets a square matrix, stored row major, to the identity
//
//  in:     in_dimension
//  out:    out_matrix
static void fmatrix_identity(
  float* out_matrix,
  const int in_dimension);

// Transposes a square matrix, stored row major, in place
//
//  in:     in_dimension
//  in/out: inout_matrix
static void fmatrix_transpose(
  float* inout_matrix,
  const int in_dimension);

// Copies the elements of one square matrix into another
//
//  in:     in_matrix, in_dimension
//  out:    out_matrix
static void fmatrix_set(
  float* out_matrix,
  const float* in_matrix,
  const int in_dimension);

// Adds two square matrices, element by element
//
//  in:     in_matrix1, in_matrix2, in_dimension
//  out:    out_matrix
static void fmatrix_add(
  float* out_matrix,
  const float* in_matrix1,
  const float* in_matrix2,
  const int in_dimension);

// Multiplies every element of a square matrix by a scalar
//
//  in:     in_matrix, in_dimension, in_scalar
//  out:    out_matrix
static void fmatrix_scale(
  float* out_matrix,
  const float* in_matrix,
  const int in_dimension,
  const float in_scalar);

// Multiplies two square matrices, stored row major. out_matrix must not
//  overlap either input
//
//  in:     in_matrix1, in_matrix2, in_dimension
//  out:    out_matrix
static void fmatrix_multiply(
  float* out_matrix,
  const float* in_matrix1,
  const float* in_matrix2,
  const int in_dimension);

#ifdef DEBUG

// Compares two square matrices element by element, to within 0.001
//
//  in:     in_matrix1, in_matrix2, in_dimension
//  ret:    1 when every element matches; otherwise 0
static int fmatrix_are_equal(
  const float* in_matrix1,
  const float* in_matrix2,
  const int in_dimension);

// Checks whether a square matrix, stored row major, is the identity, to
//  within 0.0001 per element
//
//  in:     in_matrix, in_dimension
//  ret:    1 when in_matrix is the identity; otherwise 0
static int fmatrix_is_identity(
  const float* in_matrix,
  const int in_dimension);

// Prints each element of a square matrix, stored row major, to stdout with
//  its row and column
//
//  in:     in_matrix, in_dimension
static void fmatrix_print_matrix_form(
  const float* in_matrix,
  const int in_dimension);

// Prints each element of a matrix's buffer to stdout with its index
//
//  in:     in_buffer, in_length
static void fmatrix_print_buffer_form(
  const float in_buffer[],
  const int in_length);

#endif

/*******************************************************************************
 *
 *   Function: fmatrix_identity()
 *
 ******************************************************************************/
static void fmatrix_identity(
  float* out_matrix,
  const int in_dimension)
{
  int r, c;
  float val;

  for (r = 0; r < in_dimension; r++)
  {
    for (c = 0; c < in_dimension; c++)
    {
      val = (r == c) ? (1.0f) : (0.0f);
      out_matrix[in_dimension * r + c] = val;
    }
  }
}

/*******************************************************************************
 *
 *   Function: fmatrix_2x2_identity()
 *
 ******************************************************************************/
void fmatrix_2x2_identity(
  union fmatrix_2x2_t* out_matrix)
{
  fmatrix_identity(&(out_matrix->mat[0][0]), 2);
}

/*******************************************************************************
 *
 *   Function: fmatrix_3x3_identity()
 *
 ******************************************************************************/
void fmatrix_3x3_identity(
  union fmatrix_3x3_t* out_matrix)
{
  fmatrix_identity(&(out_matrix->mat[0][0]), 3);
}

/*******************************************************************************
 *
 *   Function: fmatrix_4x4_identity()
 *
 ******************************************************************************/
void fmatrix_4x4_identity(
  union fmatrix_4x4_t* out_matrix)
{
  fmatrix_identity(&(out_matrix->mat[0][0]), 4);
}

/*******************************************************************************
 *
 *   Function: fmatrix_transpose()
 *
 ******************************************************************************/
static void fmatrix_transpose(
  float* inout_matrix,
  const int in_dimension)
{
  /*
 * |---|---|
 * | A | B |  Swap [0,1] with [1,0]
 * |---|---|
 * | C | D |   00 [01]
 * |---|---|  [10] 11
 *       |
 *       V
 * |---|---|
 * | A | C |
 * |---|---|
 * | B | D |
 * |---|---|
 */

/*
 * |---|---|---|
 * | A | B | C |  Swap [0,1] with [1,0]; Swap [0,2] with [2,0];
 * |---|---|---|  Swap [1,2] with [2,1]
 * | D | E | F |    00  [01] [02]
 * |---|---|---|   [10]  11  [12]
 * | G | H | I |   [20] [21]  22
 * |---|---|---|
 *       |
 *       V
 * |---|---|---|
 * | A | D | G |
 * |---|---|---|
 * | B | E | H |
 * |---|---|---|
 * | C | F | I |
 * |---|---|---|
 */

/*
 * |---|---|---|---|
 * | A | B | C | D |  00  [01] [02] [03]
 * |---|---|---|---| [10]  11  [12] [13]
 * | E | F | G | H | [20] [21]  22  [23]
 * |---|---|---|---| [30] [31] [32]  33
 * | I | J | K | L |
 * |---|---|---|---|
 * | M | N | O | P |
 * |---|---|---|---|
 *       |
 *       V
 * |---|---|---|---|
 * | A | E | I | M |
 * |---|---|---|---|
 * | B | F | J | N |
 * |---|---|---|---|
 * | C | G | K | O |
 * |---|---|---|---|
 * | D | H | L | P |
 * |---|---|---|---|
 */
  int r, c;
  float temp;

  for (r = 0; r < in_dimension; r++)
  {
    for (c = 0; c < in_dimension; c++)
    {
      if (c > r)
      {
        temp = inout_matrix[in_dimension * c + r];
        inout_matrix[in_dimension * c + r] = inout_matrix[in_dimension * r + c];
        inout_matrix[in_dimension * r + c] = temp;
      }
    }
  }
}

/*******************************************************************************
 *
 *   Function: fmatrix_2x2_transpose()
 *
 ******************************************************************************/
void fmatrix_2x2_transpose(
  union fmatrix_2x2_t* inout_matrix)
{
  fmatrix_transpose(inout_matrix->buf, 2);
}

/*******************************************************************************
 *
 *   Function: fmatrix_3x3_transpose()
 *
 ******************************************************************************/
void fmatrix_3x3_transpose(
  union fmatrix_3x3_t* inout_matrix)
{
  fmatrix_transpose(inout_matrix->buf, 3);
}

/*******************************************************************************
 *
 *   Function: fmatrix_4x4_transpose()
 *
 ******************************************************************************/
void fmatrix_4x4_transpose(
  union fmatrix_4x4_t* inout_matrix)
{
  fmatrix_transpose(inout_matrix->buf, 4);
}

/*******************************************************************************
 *
 *   Function: fmatrix_2x2_init()
 *
 ******************************************************************************/
void fmatrix_2x2_init(
  union fmatrix_2x2_t* out_matrix)
{
  memset(out_matrix, 0, sizeof(union fmatrix_2x2_t));
  fmatrix_2x2_identity(out_matrix);
}

/*******************************************************************************
 *
 *   Function: fmatrix_3x3_init()
 *
 ******************************************************************************/
void fmatrix_3x3_init(
  union fmatrix_3x3_t* out_matrix)
{
  memset(out_matrix, 0, sizeof(union fmatrix_3x3_t));
  fmatrix_3x3_identity(out_matrix);
}

/*******************************************************************************
 *
 *   Function: fmatrix_4x4_init()
 *
 ******************************************************************************/
void fmatrix_4x4_init(
  union fmatrix_4x4_t* out_matrix)
{
  memset(out_matrix, 0, sizeof(union fmatrix_4x4_t));
  fmatrix_4x4_identity(out_matrix);
}

/*******************************************************************************
 *
 *   Function: fmatrix_set()
 *
 ******************************************************************************/
static void fmatrix_set(
  float* out_matrix,
  const float* in_matrix,
  const int in_dimension)
{
  int r, c;

  for (r = 0; r < in_dimension; r++)
  {
    for (c = 0; c < in_dimension; c++)
    {
      out_matrix[in_dimension * r + c] = in_matrix[in_dimension * r + c];
    }
  }
}

/*******************************************************************************
 *
 *   Function: fmatrix_2x2_set()
 *
 ******************************************************************************/
void fmatrix_2x2_set(
  union fmatrix_2x2_t* out_matrix,
  const float in_data[2][2])
{
  fmatrix_set(&(out_matrix->buf[0]), &(in_data[0][0]), 2);
}

/*******************************************************************************
 *
 *   Function: fmatrix_3x3_set()
 *
 ******************************************************************************/
void fmatrix_3x3_set(
  union fmatrix_3x3_t* out_matrix,
  const float in_data[3][3])
{
  fmatrix_set(&(out_matrix->buf[0]), &(in_data[0][0]), 3);
}

/*******************************************************************************
 *
 *   Function: fmatrix_4x4_set()
 *
 ******************************************************************************/
void fmatrix_4x4_set(
  union fmatrix_4x4_t* out_matrix,
  const float in_data[4][4])
{
  fmatrix_set(&(out_matrix->buf[0]), &(in_data[0][0]), 4);
}

/*******************************************************************************
 *
 *   Function: fmatrix_add()
 *
 ******************************************************************************/
static void fmatrix_add(
  float* out_matrix,
  const float* in_matrix1,
  const float* in_matrix2,
  const int in_dimension)
{
  int r, c;
  float val1, val2;

  for (r = 0; r < in_dimension; r++)
  {
    for (c = 0; c < in_dimension; c++)
    {
      val1 = in_matrix1[in_dimension * r + c];
      val2 = in_matrix2[in_dimension * r + c];

      out_matrix[in_dimension * r + c] = (val1 + val2);
    }
  }
}

/*******************************************************************************
 *
 *   Function: fmatrix_scale()
 *
 ******************************************************************************/
static void fmatrix_scale(
  float* out_matrix,
  const float* in_matrix,
  const int in_dimension,
  const float in_scalar)
{
  int r, c;
  float val;

  for (r = 0; r < in_dimension; r++)
  {
    for (c = 0; c < in_dimension; c++)
    {
      val = in_matrix[in_dimension * r + c];
      out_matrix[in_dimension * r + c] = (val * in_scalar);
    }
  }
}

/*******************************************************************************
 *
 *   Function: fmatrix_2x2_add()
 *
 ******************************************************************************/
union fmatrix_2x2_t fmatrix_2x2_add(
  const union fmatrix_2x2_t* in_matrix1,
  const union fmatrix_2x2_t* in_matrix2)
{
  union fmatrix_2x2_t resultant;

  fmatrix_add(resultant.buf, in_matrix1->buf, in_matrix2->buf, 2);

  return resultant;
}

/*******************************************************************************
 *
 *   Function: fmatrix_3x3_add()
 *
 ******************************************************************************/
union fmatrix_3x3_t fmatrix_3x3_add(
  const union fmatrix_3x3_t* in_matrix1,
  const union fmatrix_3x3_t* in_matrix2)
{
  union fmatrix_3x3_t resultant;

  fmatrix_add(resultant.buf, in_matrix1->buf, in_matrix2->buf, 3);

  return resultant;
}

/*******************************************************************************
 *
 *   Function: fmatrix_4x4_add()
 *
 ******************************************************************************/
union fmatrix_4x4_t fmatrix_4x4_add(
  const union fmatrix_4x4_t* in_matrix1,
  const union fmatrix_4x4_t* in_matrix2)
{
  union fmatrix_4x4_t resultant;

  fmatrix_add(resultant.buf, in_matrix1->buf, in_matrix2->buf, 4);

  return resultant;
}

/*******************************************************************************
 *
 *   Function: fmatrix_2x2_subtract()
 *
 ******************************************************************************/
union fmatrix_2x2_t fmatrix_2x2_subtract(
  const union fmatrix_2x2_t* in_matrix1,
  const union fmatrix_2x2_t* in_matrix2)
{
  union fmatrix_2x2_t mat2_neg_scaled = fmatrix_2x2_scale(in_matrix2, -1.0f);
  return fmatrix_2x2_add(in_matrix1, &mat2_neg_scaled);
}

/*******************************************************************************
 *
 *   Function: fmatrix_3x3_subtract()
 *
 ******************************************************************************/
union fmatrix_3x3_t fmatrix_3x3_subtract(
  const union fmatrix_3x3_t* in_matrix1,
  const union fmatrix_3x3_t* in_matrix2)
{
  union fmatrix_3x3_t mat3_neg_scaled = fmatrix_3x3_scale(in_matrix2, -1.0f);
  return fmatrix_3x3_add(in_matrix1, &mat3_neg_scaled);
}

/*******************************************************************************
 *
 *   Function: fmatrix_4x4_subtract()
 *
 ******************************************************************************/
union fmatrix_4x4_t fmatrix_4x4_subtract(
  const union fmatrix_4x4_t* in_matrix1,
  const union fmatrix_4x4_t* in_matrix2)
{
  union fmatrix_4x4_t mat4_neg_scaled = fmatrix_4x4_scale(in_matrix2, -1.0f);
  return fmatrix_4x4_add(in_matrix1, &mat4_neg_scaled);
}

/*******************************************************************************
 *
 *   Function: fmatrix_2x2_scale()
 *
 ******************************************************************************/
union fmatrix_2x2_t fmatrix_2x2_scale(
  const union fmatrix_2x2_t* in_matrix,
  const float in_scalar)
{
  union fmatrix_2x2_t resultant;

  fmatrix_scale(resultant.buf, in_matrix->buf, 2, in_scalar);

  return resultant;
}

/*******************************************************************************
 *
 *   Function: fmatrix_3x3_scale()
 *
 ******************************************************************************/
union fmatrix_3x3_t fmatrix_3x3_scale(
  const union fmatrix_3x3_t* in_matrix,
  const float in_scalar)
{
  union fmatrix_3x3_t resultant;

  fmatrix_scale(resultant.buf, in_matrix->buf, 3, in_scalar);

  return resultant;
}

/*******************************************************************************
 *
 *   Function: fmatrix_4x4_scale()
 *
 ******************************************************************************/
union fmatrix_4x4_t fmatrix_4x4_scale(
  const union fmatrix_4x4_t* in_matrix,
  const float in_scalar)
{
  union fmatrix_4x4_t resultant;

  fmatrix_scale(resultant.buf, in_matrix->buf, 4, in_scalar);

  return resultant;
}

/*******************************************************************************
 *
 *   Function: fmatrix_multiply()
 *
 ******************************************************************************/
static void fmatrix_multiply(
  float* out_matrix,
  const float* in_matrix1,
  const float* in_matrix2,
  const int in_dimension)
{
  int r, c, i;
  float sum;

  for (r = 0; r < in_dimension; r++)
  {
    for (c = 0; c < in_dimension; c++)
    {
      sum = 0.0f;

      for (i = 0; i < in_dimension; i++)
      {
        int mat1_idx = (in_dimension * r + i);
        int mat2_idx = (c + in_dimension * i);

        sum += (in_matrix1[mat1_idx] * in_matrix2[mat2_idx]);
      }

      out_matrix[in_dimension * r + c] = sum;
    }
  }
}

/*******************************************************************************
 *
 *   Function: fmatrix_2x2_multiply()
 *
 ******************************************************************************/
union fmatrix_2x2_t fmatrix_2x2_multiply(
  const union fmatrix_2x2_t* in_matrix1,
  const union fmatrix_2x2_t* in_matrix2)
{
  union fmatrix_2x2_t resultant;

  fmatrix_multiply(
    &(resultant.buf[0]), &(in_matrix1->buf[0]), &(in_matrix2->buf[0]), 2);

  return resultant;
}

/*******************************************************************************
 *
 *   Function: fmatrix_3x3_multiply()
 *
 ******************************************************************************/
union fmatrix_3x3_t fmatrix_3x3_multiply(
  const union fmatrix_3x3_t* in_matrix1,
  const union fmatrix_3x3_t* in_matrix2)
{
  union fmatrix_3x3_t resultant;

  fmatrix_multiply(
    &(resultant.buf[0]), &(in_matrix1->buf[0]), &(in_matrix2->buf[0]), 3);

  return resultant;
}

/*******************************************************************************
 *
 *   Function: fmatrix_4x4_multiply()
 *
 ******************************************************************************/
union fmatrix_4x4_t fmatrix_4x4_multiply(
  const union fmatrix_4x4_t* in_matrix1,
  const union fmatrix_4x4_t* in_matrix2)
{
  union fmatrix_4x4_t resultant;

  fmatrix_multiply(
    &(resultant.buf[0]), &(in_matrix1->buf[0]), &(in_matrix2->buf[0]), 4);

  return resultant;
}

/*******************************************************************************
 *
 *   Function: fmatrix_4x4_transform_translate()
 *
 ******************************************************************************/
union fmatrix_4x4_t fmatrix_4x4_transform_translate(
  const union fmatrix_4x4_t* in_matrix,
  const union fvector3_t in_translation)
{
  union fmatrix_4x4_t translation_matrix;

  const float translation_matrix_data[4][4] = {
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {in_translation.comp.x, in_translation.comp.y, in_translation.comp.z, 1.0f}};

  fmatrix_4x4_set(&translation_matrix, translation_matrix_data);

  return fmatrix_4x4_multiply(in_matrix, &translation_matrix);
}

/*******************************************************************************
 *
 *   Function: fmatrix_4x4_transform_scale()
 *
 ******************************************************************************/
union fmatrix_4x4_t fmatrix_4x4_transform_scale(
  const union fmatrix_4x4_t* in_matrix,
  const union fvector3_t in_scale_factors)
{
  union fmatrix_4x4_t scale_matrix;

  const float scale_matrix_data[4][4] = {
    {in_scale_factors.comp.x, 0.0f, 0.0f, 0.0f},
    {0.0f, in_scale_factors.comp.y, 0.0f, 0.0f},
    {0.0f, 0.0f, in_scale_factors.comp.z, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}};

  fmatrix_4x4_set(&scale_matrix, scale_matrix_data);

  return fmatrix_4x4_multiply(in_matrix, &scale_matrix);
}

/*******************************************************************************
 *
 *   Function: fmatrix_4x4_transform_point()
 *
 ******************************************************************************/
union fvector3_t fmatrix_4x4_transform_point(
  const union fmatrix_4x4_t* in_matrix,
  const union fvector3_t in_point)
{
  union fvector4_t point_vec4 = {in_point.comp.x, in_point.comp.y, in_point.comp.z, 1.0f};
  union fvector4_t resultant = {0.0f, 0.0f, 0.0f, 0.0f};

  for (int p = 0; p < 4; p++)
  {
    for (int m_col = 0; m_col < 4; m_col++)
    {
      resultant.vec[p] += (in_matrix->mat[m_col][p]) * (point_vec4.vec[m_col]);
    }
  }

  return (union fvector3_t){{resultant.comp.w, resultant.comp.x, resultant.comp.y}};
}


#ifdef DEBUG

#include <math.h>
#include <stdio.h>

/*******************************************************************************
 *
 *   Function: fmatrix_are_equal()
 *
 ******************************************************************************/
static int fmatrix_are_equal(
  const float* in_matrix1,
  const float* in_matrix2,
  const int in_dimension)
{
  int are_equal = 1;

  int r, c;
  float a, b;

  for (r = 0; r < in_dimension; r++)
  {
    for (c = 0; c < in_dimension; c++)
    {
      a = in_matrix1[in_dimension * r + c];
      b = in_matrix2[in_dimension * r + c];

      // Row and Column are equal AND !(cur_val = 1)
      if (!(fabs(a - b) < 0.001))
      {
        are_equal = 0;
      }
    }
  }

  return are_equal;
}

/*******************************************************************************
 *
 *   Function: fmatrix_2x2_are_equal()
 *
 ******************************************************************************/
int fmatrix_2x2_are_equal(
  const union fmatrix_2x2_t* in_matrix1,
  const union fmatrix_2x2_t* in_matrix2)
{
  return fmatrix_are_equal(in_matrix1->buf, in_matrix2->buf, 2);
}

/*******************************************************************************
 *
 *   Function: fmatrix_3x3_are_equal()
 *
 ******************************************************************************/
int fmatrix_3x3_are_equal(
  const union fmatrix_3x3_t* in_matrix1,
  const union fmatrix_3x3_t* in_matrix2)
{
  return fmatrix_are_equal(in_matrix1->buf, in_matrix2->buf, 3);
}

/*******************************************************************************
 *
 *   Function: fmatrix_4x4_are_equal()
 *
 ******************************************************************************/
int fmatrix_4x4_are_equal(
  const union fmatrix_4x4_t* in_matrix1,
  const union fmatrix_4x4_t* in_matrix2)
{
  return fmatrix_are_equal(in_matrix1->buf, in_matrix2->buf, 4);
}

/*******************************************************************************
 *
 *   Function: fmatrix_is_identity()
 *
 ******************************************************************************/
static int fmatrix_is_identity(
  const float* in_matrix,
  const int in_dimension)
{
  int is_identity = 1;

  int r, c;
  float cur_val;

  for (r = 0; r < in_dimension; r++)
  {
    for (c = 0; c < in_dimension; c++)
    {
      cur_val = in_matrix[in_dimension * r + c];

      // Row and Column are equal AND !(cur_val = 1)
      if (((r == c) && !(fabs(cur_val - 1.0f) < 0.0001)) ||
          ((r != c) && !(fabs(cur_val) < 0.0001)))
      {
        is_identity = 0;
      }
    }
  }

  return is_identity;
}

/*******************************************************************************
 *
 *   Function: fmatrix_2x2_is_identity()
 *
 ******************************************************************************/
int fmatrix_2x2_is_identity(
  const union fmatrix_2x2_t* in_matrix)
{
  return fmatrix_is_identity(in_matrix->buf, 2);
}

/*******************************************************************************
 *
 *   Function: fmatrix_3x3_is_identity()
 *
 ******************************************************************************/
int fmatrix_3x3_is_identity(
  const union fmatrix_3x3_t* in_matrix)
{
  return fmatrix_is_identity(in_matrix->buf, 3);
}

/*******************************************************************************
 *
 *   Function: fmatrix_4x4_is_identity()
 *
 ******************************************************************************/
int fmatrix_4x4_is_identity(
  const union fmatrix_4x4_t* in_matrix)
{
  return fmatrix_is_identity(in_matrix->buf, 4);
}

/*******************************************************************************
 *
 *   Function: fmatrix_print_matrix_form()
 *
 ******************************************************************************/
static void fmatrix_print_matrix_form(
  const float* in_matrix,
  const int in_dimension)
{
  printf("Printing matrix representation\n");

  int r, c;
  for (r = 0; r < in_dimension; r++)
  {
    for (c = 0; c < in_dimension; c++)
    {
      float val = in_matrix[in_dimension * r + c];
      printf("Row = %d, Col = %d, Val = %f\n", r, c, val);
    }
  }
  printf("\n\n");
}

/*******************************************************************************
 *
 *   Function: fmatrix_print_buffer_form()
 *
 ******************************************************************************/
static void fmatrix_print_buffer_form(
  const float in_buffer[],
  const int in_length)
{
  printf("Printing buffer representation\n");

  int i;
  for (i = 0; i < in_length; i++)
  {
    float val = in_buffer[i];
    printf("Buffer Index = %d, Val = %f\n", i, val);
  }
  printf("\n\n");
}

/*******************************************************************************
 *
 *   Function: fmatrix_2x2_print()
 *
 ******************************************************************************/
void fmatrix_2x2_print(
  const union fmatrix_2x2_t* in_matrix)
{
  fmatrix_print_matrix_form(&(in_matrix->mat[0][0]), 2);
  // fmatrix_print_buffer_form(matrix->buf, 2*2);
}

/*******************************************************************************
 *
 *   Function: fmatrix_3x3_print()
 *
 ******************************************************************************/
void fmatrix_3x3_print(
  const union fmatrix_3x3_t* in_matrix)
{
  fmatrix_print_matrix_form(&(in_matrix->mat[0][0]), 3);
  // fmatrix_print_buffer_form(matrix->buf, 3*3);
}

/*******************************************************************************
 *
 *   Function: fmatrix_4x4_print()
 *
 ******************************************************************************/
void fmatrix_4x4_print(
  const union fmatrix_4x4_t* in_matrix)
{
  fmatrix_print_matrix_form(&(in_matrix->mat[0][0]), 4);
  // fmatrix_print_buffer_form(matrix->buf, 4*4);
}

#endif
