#pragma once

typedef union fmatrix_2x2_t
{
   // Buffer Form
   float buf[4];
   // Matrix form - row major - mat[row][col]
   float mat[2][2];

/*
   --------------------------------------------
   | mat[0][0] / buf[?] || mat[0][1] / buf[?] |
   --------------------------------------------
   | mat[1][0] / buf[?] || mat[1][1] / buf[?] |
   --------------------------------------------
*/
} fmatrix_2x2;

typedef union fmatrix_3x3_t
{
   // Buffer Form
   float buf[9];
   // Matrix form - row major - mat[row][col]
   float mat[3][3];
/*
   ------------------------------------------------------------------
   | mat[0][0] / buf[?] || mat[0][1] / buf[?] || mat[0][2] / buf[?] |
   ------------------------------------------------------------------
   | mat[1][0] / buf[?] || mat[1][1] / buf[?] || mat[1][2] / buf[?] |
   ------------------------------------------------------------------
   | mat[2][0] / buf[?] || mat[2][1] / buf[?] || mat[2][2] / buf[?] |
   ------------------------------------------------------------------
*/
} fmatrix_3x3;

typedef union fmatrix_4x4_t
{
   // Buffer Form
   float buf[16];
   // Matrix form - row major - mat[row][col]
   float mat[4][4];
/*
   ------------------------------------------------------------------------------------------
   | mat[0][0] / buf[?] || mat[0][1] / buf[?] || mat[0][2] / buf[?]  || mat[0][3] / buf[?] ||
   ------------------------------------------------------------------------------------------
   | mat[1][0] / buf[?] || mat[1][1] / buf[?] || mat[1][2] / buf[?]  || mat[1][3] / buf[?] ||
   ------------------------------------------------------------------------------------------
   | mat[2][0] / buf[?] || mat[2][1] / buf[?] || mat[2][2] / buf[?]  || mat[2][3] / buf[?] ||
   ------------------------------------------------------------------------------------------
   | mat[3][0] / buf[?] || mat[3][1] / buf[?] || mat[3][2] / buf[?]  || mat[3][3] / buf[?] ||
   ------------------------------------------------------------------------------------------
*/
} fmatrix_4x4;

#ifdef DEBUG

int fmatrix_2x2_are_equal(fmatrix_2x2* matrix1, fmatrix_2x2* matrix2);
int fmatrix_3x3_are_equal(fmatrix_3x3* matrix1, fmatrix_3x3* matrix2);
int fmatrix_4x4_are_equal(fmatrix_4x4* matrix1, fmatrix_4x4* matrix2);

int fmatrix_2x2_is_identity(fmatrix_2x2* matrix);
int fmatrix_3x3_is_identity(fmatrix_3x3* matrix);
int fmatrix_4x4_is_identity(fmatrix_4x4* matrix);

void fmatrix_2x2_print(fmatrix_2x2* matrix);
void fmatrix_3x3_print(fmatrix_3x3* matrix);
void fmatrix_4x4_print(fmatrix_4x4* matrix);

#endif

void fmatrix_2x2_identity(fmatrix_2x2* matrix);
void fmatrix_3x3_identity(fmatrix_3x3* matrix);
void fmatrix_4x4_identity(fmatrix_4x4* matrix);

void fmatrix_2x2_init(fmatrix_2x2* matrix);
void fmatrix_3x3_init(fmatrix_3x3* matrix);
void fmatrix_4x4_init(fmatrix_4x4* matrix);

fmatrix_2x2 fmatrix_2x2_add(fmatrix_2x2 matrix1, fmatrix_2x2 matrix2);
fmatrix_3x3 fmatrix_3x3_add(fmatrix_3x3 matrix1, fmatrix_3x3 matrix2);
fmatrix_4x4 fmatrix_4x4_add(fmatrix_4x4 matrix1, fmatrix_4x4 matrix2);

fmatrix_2x2 fmatrix_2x2_subtract(fmatrix_2x2 matrix1, fmatrix_2x2 matrix2);
fmatrix_3x3 fmatrix_3x3_subtract(fmatrix_3x3 matrix1, fmatrix_3x3 matrix2);
fmatrix_4x4 fmatrix_4x4_subtract(fmatrix_4x4 matrix1, fmatrix_4x4 matrix2);

fmatrix_2x2 fmatrix_2x2_scale(fmatrix_2x2 matrix, float scalar);
fmatrix_3x3 fmatrix_3x3_scale(fmatrix_3x3 matrix, float scalar);
fmatrix_4x4 fmatrix_4x4_scale(fmatrix_4x4 matrix, float scalar);

fmatrix_2x2 fmatrix_2x2_multiply(fmatrix_2x2 matrix1, fmatrix_2x2 matrix2);
fmatrix_3x3 fmatrix_3x3_multiply(fmatrix_3x3 matrix1, fmatrix_3x3 matrix2);
fmatrix_4x4 fmatrix_4x4_multiply(fmatrix_4x4 matrix1, fmatrix_4x4 matrix2);