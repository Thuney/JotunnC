#pragma once

struct fmatrix_2x2_t
{
   // Column A
   float a[2];
   // Column B
   float b[2];

/*
   ---------------
   | a[0] | b[0] |
   ---------------
   | a[1] | b[1] |
   ---------------
*/
};

struct fmatrix_3x3_t
{
   // Column A
   float a[3];
   // Column B
   float b[3];
   // Column C
   float c[3];
/*
   ----------------------
   | a[0] | b[0] | c[0] |
   ----------------------
   | a[1] | b[1] | c[1] |
   ----------------------
   | a[2] | b[2] | c[2] |
   ----------------------
*/
};

struct fmatrix_4x4_t
{
   // Column A
   float a[4];
   // Column B
   float b[4];
   // Column C
   float c[4];
   // Column D
   float d[4];
/*
   -----------------------------
   | a[0] | b[0] | c[0] | d[0] |
   -----------------------------
   | a[1] | b[1] | c[1] | d[1] |
   -----------------------------
   | a[2] | b[2] | c[2] | d[2] |
   -----------------------------
   | a[3] | b[3] | c[3] | d[3] |
   -----------------------------
*/
};

void fmatrix_2x2_init(struct fmatrix_2x2_t* matrix);
void fmatrix_3x3_init(struct fmatrix_3x3_t* matrix);
void fmatrix_4x4_init(struct fmatrix_4x4_t* matrix);

void fmatrix_2x2_identity(struct fmatrix_2x2_t* matrix);
void fmatrix_3x3_identity(struct fmatrix_3x3_t* matrix);
void fmatrix_4x4_identity(struct fmatrix_4x4_t* matrix);