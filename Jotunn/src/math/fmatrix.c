#include <memory.h>

#include "fmatrix.h"

void fmatrix_2x2_identity(struct fmatrix_2x2_t* matrix)
{
   matrix->a[0] = matrix->b[1] = 1.0f;
}

void fmatrix_3x3_identity(struct fmatrix_3x3_t* matrix)
{
   matrix->a[0] = matrix->b[1] = matrix->c[2] = 1.0f;
}

void fmatrix_4x4_identity(struct fmatrix_4x4_t* matrix)
{
   matrix->a[0] = matrix->b[1] = matrix->c[2] = matrix->d[3] = 1.0f;
}

void fmatrix_2x2_init(struct fmatrix_2x2_t* matrix)
{
   memset(matrix, 0, sizeof(struct fmatrix_2x2_t));
   fmatrix_2x2_identity(matrix);
}

void fmatrix_3x3_init(struct fmatrix_3x3_t* matrix)
{
   memset(matrix, 0, sizeof(struct fmatrix_3x3_t));
   fmatrix_3x3_identity(matrix);
}

void fmatrix_4x4_init(struct fmatrix_4x4_t* matrix)
{
   memset(matrix, 0, sizeof(struct fmatrix_4x4_t));
   fmatrix_4x4_identity(matrix);
}