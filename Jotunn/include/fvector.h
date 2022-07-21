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

float fvector2_magnitude(struct fvector2 vector);
float fvector3_magnitude(struct fvector3 vector);

// <V1> + <V2>
struct fvector2 fvector2_add(struct fvector2 vector1, struct fvector2 vector2);
struct fvector3 fvector3_add(struct fvector3 vector1, struct fvector3 vector2);

// <V1> - <V2>
struct fvector2 fvector2_subtract(struct fvector2 vector1, struct fvector2 vector2);
struct fvector3 fvector3_subtract(struct fvector3 vector1, struct fvector3 vector2);

// <V>*s
struct fvector2 fvector2_scale(struct fvector2 vector, float scalar);
struct fvector3 fvector3_scale(struct fvector3 vector, float scalar);

// <V1> dot product <V2>
float fvector2_dot(struct fvector2 vector1, struct fvector2 vector2);
float fvector3_dot(struct fvector3 vector1, struct fvector3 vector2);

// <V1> cross product <V2>
struct fvector3 fvector3_cross(struct fvector3 vector1, struct fvector3 vector2);