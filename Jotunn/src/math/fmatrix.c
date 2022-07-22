#include <memory.h>

#include "fmatrix.h"

//

static void fmatrix_identity(float* matrix, const int dim)
{
   int r, c;
   float val;

   for (r = 0; r < dim; r++)
   {
      for (c = 0; c < dim; c++)
      {
         val = (r == c) ? (1.0f) : (0.0f);
         matrix[dim*r + c] = val;
      }
   }
}

void fmatrix_2x2_identity(fmatrix_2x2* matrix)
{
   fmatrix_identity(&(matrix->mat[0][0]), 2);
}

void fmatrix_3x3_identity(fmatrix_3x3* matrix)
{
   fmatrix_identity(&(matrix->mat[0][0]), 3);
}

void fmatrix_4x4_identity(fmatrix_4x4* matrix)
{
   fmatrix_identity(&(matrix->mat[0][0]), 4);
}

// 

void fmatrix_2x2_init(fmatrix_2x2* matrix)
{
   memset(matrix, 0, sizeof(fmatrix_2x2));
   fmatrix_2x2_identity(matrix);
}

void fmatrix_3x3_init(fmatrix_3x3* matrix)
{
   memset(matrix, 0, sizeof(fmatrix_3x3));
   fmatrix_3x3_identity(matrix);
}

void fmatrix_4x4_init(fmatrix_4x4* matrix)
{
   memset(matrix, 0, sizeof(fmatrix_4x4));
   fmatrix_4x4_identity(matrix);
}

static void fmatrix_set(float* dest_buf, const float* src_buf, const int dim)
{
   int r, c;

   for (r = 0; r < dim; r++)
   {
      for (c = 0; c < dim; c++)
      {
         dest_buf[dim*r + c] = src_buf[dim*r + c];
      }
   }
} 

void fmatrix_2x2_set(fmatrix_2x2* matrix, const float mat_data[2][2])
{
   fmatrix_set(&(matrix->buf[0]), &(mat_data[0][0]), 2);
}

void fmatrix_3x3_set(fmatrix_3x3* matrix, const float mat_data[3][3])
{
   fmatrix_set(&(matrix->buf[0]), &(mat_data[0][0]), 3);
}

void fmatrix_4x4_set(fmatrix_4x4* matrix, const float mat_data[4][4])
{
   fmatrix_set(&(matrix->buf[0]), &(mat_data[0][0]), 4);
}

//

static void fmatrix_add(float* matrix_dest, const float* matrix1, const float* matrix2, const int dim)
{
   int r, c;
   float val1, val2;

   for (r = 0; r < dim; r++)
   {
      for (c = 0; c < dim; c++)
      {
         val1 = matrix1[dim*r + c];
         val2 = matrix2[dim*r + c];

         matrix_dest[dim*r + c] = (val1 + val2);
      }
   }
}

static void fmatrix_scale(float* matrix_dest, const float* matrix_src, const int dim, const float scalar)
{
   int r, c;
   float val;

   for (r = 0; r < dim; r++)
   {
      for (c = 0; c < dim; c++)
      {
         val = matrix_src[dim*r + c];
         matrix_dest[dim*r + c] = (val*scalar);
      }
   }
}

fmatrix_2x2 fmatrix_2x2_add(const fmatrix_2x2* matrix1, const fmatrix_2x2* matrix2)
{
   fmatrix_2x2 resultant;
   
   fmatrix_add(resultant.buf, matrix1->buf, matrix2->buf, 2);

   return resultant;
}

fmatrix_3x3 fmatrix_3x3_add(const fmatrix_3x3* matrix1, const fmatrix_3x3* matrix2)
{
   fmatrix_3x3 resultant;

   fmatrix_add(resultant.buf, matrix1->buf, matrix2->buf, 3);

   return resultant;
}

fmatrix_4x4 fmatrix_4x4_add(const fmatrix_4x4* matrix1, const fmatrix_4x4* matrix2)
{
   fmatrix_4x4 resultant;

   fmatrix_add(resultant.buf, matrix1->buf, matrix2->buf, 4);

   return resultant;
}

//

fmatrix_2x2 fmatrix_2x2_subtract(const fmatrix_2x2* matrix1, const fmatrix_2x2* matrix2)
{
   fmatrix_2x2 mat2_neg_scaled = fmatrix_2x2_scale(matrix2, -1.0f);
   return fmatrix_2x2_add(matrix1, &mat2_neg_scaled);
}

fmatrix_3x3 fmatrix_3x3_subtract(const fmatrix_3x3* matrix1, const fmatrix_3x3* matrix2)
{
   fmatrix_3x3 mat3_neg_scaled = fmatrix_3x3_scale(matrix2, -1.0f);
   return fmatrix_3x3_add(matrix1, &mat3_neg_scaled);
}

fmatrix_4x4 fmatrix_4x4_subtract(const fmatrix_4x4* matrix1, const fmatrix_4x4* matrix2)
{
   fmatrix_4x4 mat4_neg_scaled = fmatrix_4x4_scale(matrix2, -1.0f);
   return fmatrix_4x4_add(matrix1, &mat4_neg_scaled);
}

//

fmatrix_2x2 fmatrix_2x2_scale(const fmatrix_2x2* matrix, const float scalar)
{
   fmatrix_2x2 resultant;

   fmatrix_scale(resultant.buf, matrix->buf, 2, scalar);

   return resultant;
}

fmatrix_3x3 fmatrix_3x3_scale(const fmatrix_3x3* matrix, const float scalar)
{
   fmatrix_3x3 resultant;

   fmatrix_scale(resultant.buf, matrix->buf, 3, scalar);

   return resultant;
}

fmatrix_4x4 fmatrix_4x4_scale(const fmatrix_4x4* matrix, const float scalar)
{
   fmatrix_4x4 resultant;

   fmatrix_scale(resultant.buf, matrix->buf, 4, scalar);

   return resultant;
}

//

static void fmatrix_multiply(float* matrix_dest, const float* matrix1, const float* matrix2, const int dim)
{
   int r, c, i;
   float sum;

   for (r = 0; r < dim; r++)
   {
      for (c = 0; c < dim; c++)
      {
         sum = 0.0f;

         for (i = 0; i < dim; i++)
         {
            int mat1_idx = (dim*r + i);
            int mat2_idx = (c + dim*i);

            sum += (matrix1[mat1_idx] * matrix2[mat2_idx]);
         }

         matrix_dest[dim*r + c] = sum;
      }
   }
}

fmatrix_2x2 fmatrix_2x2_multiply(const fmatrix_2x2* matrix1, const fmatrix_2x2* matrix2)
{
   fmatrix_2x2 resultant;

   fmatrix_multiply(&(resultant.buf[0]), &(matrix1->buf[0]), &(matrix2->buf[0]), 2);

   return resultant;
}

fmatrix_3x3 fmatrix_3x3_multiply(const fmatrix_3x3* matrix1, const fmatrix_3x3* matrix2)
{
   fmatrix_3x3 resultant;

   fmatrix_multiply(&(resultant.buf[0]), &(matrix1->buf[0]), &(matrix2->buf[0]), 3);

   return resultant;
}

fmatrix_4x4 fmatrix_4x4_multiply(const fmatrix_4x4* matrix1, const fmatrix_4x4* matrix2)
{
   fmatrix_4x4 resultant;

   fmatrix_multiply(&(resultant.buf[0]), &(matrix1->buf[0]), &(matrix2->buf[0]), 4);

   return resultant;
}

/*************************
 * 
 *         DEBUG
 * 
 *************************/

#ifdef DEBUG

#include <math.h>
#include <stdio.h>

static int fmatrix_are_equal(const float* matrix1, const float* matrix2, const int dim)
{
   int are_equal = 1;

   int r, c;
   float a, b;

   for (r = 0; r < dim; r++)
   {
      for (c = 0; c < dim; c++)
      {
         a = matrix1[dim*r + c];
         b = matrix2[dim*r + c];

         // Row and Column are equal AND !(cur_val = 1)
         if ( !(fabs(a - b) < 0.0001) )
         {
            are_equal = 0;
         }
      }
   }

   return are_equal;
}

int fmatrix_2x2_are_equal(const fmatrix_2x2* matrix1, const fmatrix_2x2* matrix2)
{
   return fmatrix_are_equal(matrix1->buf, matrix2->buf, 2);
}

int fmatrix_3x3_are_equal(const fmatrix_3x3* matrix1, const fmatrix_3x3* matrix2)
{
   return fmatrix_are_equal(matrix1->buf, matrix2->buf, 3);
}

int fmatrix_4x4_are_equal(const fmatrix_4x4* matrix1, const fmatrix_4x4* matrix2)
{
   return fmatrix_are_equal(matrix1->buf, matrix2->buf, 4);
}

static int fmatrix_is_identity(const float* matrix, const int dim)
{
   int is_identity = 1;

   int r, c;
   float cur_val;

   for (r = 0; r < dim; r++)
   {
      for (c = 0; c < dim; c++)
      {
         cur_val = matrix[dim*r + c];

         // Row and Column are equal AND !(cur_val = 1)
         if ( ((r == c) && !(fabs(cur_val - 1.0f) < 0.0001)) || ((r != c) && !(fabs(cur_val) < 0.0001)) )
         {
            is_identity = 0;
         }
      }
   }

   return is_identity;
}

int fmatrix_2x2_is_identity(const fmatrix_2x2* matrix)
{
   return fmatrix_is_identity(matrix->buf, 2);
}

int fmatrix_3x3_is_identity(const fmatrix_3x3* matrix)
{
   return fmatrix_is_identity(matrix->buf, 3);
}

int fmatrix_4x4_is_identity(const fmatrix_4x4* matrix)
{
   return fmatrix_is_identity(matrix->buf, 4);
}

static void fmatrix_print_matrix_form(const float* matrix, const int dim)
{
   printf("Printing matrix representation\n");
   
   int r, c;
   for (r = 0; r < dim; r++)
   {
      for (c = 0; c < dim; c++)
      {
         float val = matrix[dim*r + c];
         printf("Row = %d, Col = %d, Val = %f\n", r, c, val);
      }
   }
   printf("\n\n");
}

static void fmatrix_print_buffer_form(const float buf[], const int length)
{
   printf("Printing buffer representation\n");

   int i;
   for (i = 0; i < length; i++)
   {
      float val = buf[i];
      printf("Buffer Index = %d, Val = %f\n", i, val);
   }
   printf("\n\n");
}

void fmatrix_2x2_print(const fmatrix_2x2* matrix)
{
   fmatrix_print_matrix_form(&(matrix->mat[0][0]), 2);
   // fmatrix_print_buffer_form(matrix->buf, 2*2);
}

void fmatrix_3x3_print(const fmatrix_3x3* matrix)
{
   fmatrix_print_matrix_form(&(matrix->mat[0][0]), 3);
   // fmatrix_print_buffer_form(matrix->buf, 3*3);
}

void fmatrix_4x4_print(const fmatrix_4x4* matrix)
{
   fmatrix_print_matrix_form(&(matrix->mat[0][0]), 4);
   // fmatrix_print_buffer_form(matrix->buf, 4*4);
}

#endif