#pragma once

struct fvector2
{
   float x, y;
   /*
    *  < x, y >
    */
};

struct fvector3
{
   float x, y, z;
   /*
    *  < x, y, z >
    */
};

void fvector2_init(struct fvector2* vector);
void fvector3_init(struct fvector3* vector);