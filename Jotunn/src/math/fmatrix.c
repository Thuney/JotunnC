#include <memory.h>

#include "fmatrix.h"

//

static void fmatrix_identity(float* matrix, int dim)
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

//

static void fmatrix_add(float* matrix_dest, float* matrix1, float* matrix2, int dim)
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

static void fmatrix_scale(float* matrix_dest, float* matrix_src, int dim, float scalar)
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

fmatrix_2x2 fmatrix_2x2_add(fmatrix_2x2 matrix1, fmatrix_2x2 matrix2)
{
   fmatrix_2x2 resultant;
   
   fmatrix_add(resultant.buf, matrix1.buf, matrix2.buf, 2);

   return resultant;
}

fmatrix_3x3 fmatrix_3x3_add(fmatrix_3x3 matrix1, fmatrix_3x3 matrix2)
{
   fmatrix_3x3 resultant;

   fmatrix_add(resultant.buf, matrix1.buf, matrix2.buf, 3);

   return resultant;
}

fmatrix_4x4 fmatrix_4x4_add(fmatrix_4x4 matrix1, fmatrix_4x4 matrix2)
{
   fmatrix_4x4 resultant;

   fmatrix_add(resultant.buf, matrix1.buf, matrix2.buf, 4);

   return resultant;
}

//

fmatrix_2x2 fmatrix_2x2_subtract(fmatrix_2x2 matrix1, fmatrix_2x2 matrix2)
{
   return fmatrix_2x2_add(matrix1, fmatrix_2x2_scale(matrix2, -1.0f));
}

fmatrix_3x3 fmatrix_3x3_subtract(fmatrix_3x3 matrix1, fmatrix_3x3 matrix2)
{
   return fmatrix_3x3_add(matrix1, fmatrix_3x3_scale(matrix2, -1.0f));
}

fmatrix_4x4 fmatrix_4x4_subtract(fmatrix_4x4 matrix1, fmatrix_4x4 matrix2)
{
   return fmatrix_4x4_add(matrix1, fmatrix_4x4_scale(matrix2, -1.0f));
}

//

fmatrix_2x2 fmatrix_2x2_scale(fmatrix_2x2 matrix, float scalar)
{
   fmatrix_2x2 resultant;

   fmatrix_scale(resultant.buf, matrix.buf, 2, scalar);

   return resultant;
}

fmatrix_3x3 fmatrix_3x3_scale(fmatrix_3x3 matrix, float scalar)
{
   fmatrix_3x3 resultant;

   fmatrix_scale(resultant.buf, matrix.buf, 3, scalar);

   return resultant;
}

fmatrix_4x4 fmatrix_4x4_scale(fmatrix_4x4 matrix, float scalar)
{
   fmatrix_4x4 resultant;

   fmatrix_scale(resultant.buf, matrix.buf, 3, scalar);

   return resultant;
}

//

fmatrix_2x2 fmatrix_2x2_multiply(fmatrix_2x2 matrix1, fmatrix_2x2 matrix2)
{
   fmatrix_2x2 resultant;

   return resultant;
}

fmatrix_3x3 fmatrix_3x3_multiply(fmatrix_3x3 matrix1, fmatrix_3x3 matrix2)
{
   fmatrix_3x3 resultant;

   return resultant;
}

fmatrix_4x4 fmatrix_4x4_multiply(fmatrix_4x4 matrix1, fmatrix_4x4 matrix2)
{
   fmatrix_4x4 resultant;

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

static int fmatrix_are_equal(float* matrix1, float* matrix2, const int dim)
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

int fmatrix_2x2_are_equal(fmatrix_2x2* matrix1, fmatrix_2x2* matrix2)
{
   return fmatrix_are_equal(matrix1->buf, matrix2->buf, 2);
}

int fmatrix_3x3_are_equal(fmatrix_3x3* matrix1, fmatrix_3x3* matrix2)
{
   return fmatrix_are_equal(matrix1->buf, matrix2->buf, 3);
}

int fmatrix_4x4_are_equal(fmatrix_4x4* matrix1, fmatrix_4x4* matrix2)
{
   return fmatrix_are_equal(matrix1->buf, matrix2->buf, 4);
}

static int fmatrix_is_identity(float* matrix, const int dim)
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

int fmatrix_2x2_is_identity(fmatrix_2x2* matrix)
{
   return fmatrix_is_identity(matrix->buf, 2);
}

int fmatrix_3x3_is_identity(fmatrix_3x3* matrix)
{
   return fmatrix_is_identity(matrix->buf, 3);
}

int fmatrix_4x4_is_identity(fmatrix_4x4* matrix)
{
   return fmatrix_is_identity(matrix->buf, 4);
}

static void fmatrix_print_matrix_form(float* matrix, int dim)
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

static void fmatrix_print_buffer_form(float buf[], int length)
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

void fmatrix_2x2_print(fmatrix_2x2* matrix)
{
   fmatrix_print_matrix_form(&(matrix->mat[0][0]), 2);
   fmatrix_print_buffer_form(matrix->buf, 2*2);
}

void fmatrix_3x3_print(fmatrix_3x3* matrix)
{
   fmatrix_print_matrix_form(&(matrix->mat[0][0]), 3);
   fmatrix_print_buffer_form(matrix->buf, 3*3);
}

void fmatrix_4x4_print(fmatrix_4x4* matrix)
{
   fmatrix_print_matrix_form(&(matrix->mat[0][0]), 4);
   fmatrix_print_buffer_form(matrix->buf, 4*4);
}

#endif