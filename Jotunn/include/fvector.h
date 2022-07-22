#pragma once

typedef union fvector2_t
{
    float vec[2];
    struct
    {
        float x, y;
    } comp;
    /*
    *  < x, y >
    */
} fvector2;

typedef union fvector3_t
{
    float vec[3];
    struct
    {
        float x, y, z;
    } comp;
    /*
    *  < x, y, z >
    */
} fvector3;

typedef union fvector4_t
{
    float vec[4];
    struct
    {
        float w, x, y, z;
    } comp;
    /*
    *  < w, x, y, z >
    */
} fvector4;

void fvector2_init(fvector2* vector);
void fvector3_init(fvector3* vector);
void fvector4_init(fvector4* vector);

float fvector2_magnitude(const fvector2* vector);
float fvector3_magnitude(const fvector3* vector);
float fvector4_magnitude(const fvector4* vector);

// <V1> + <V2>
fvector2 fvector2_add(const fvector2* vector1, const fvector2* vector2);
fvector3 fvector3_add(const fvector3* vector1, const fvector3* vector2);
fvector4 fvector4_add(const fvector4* vector1, const fvector4* vector2);

// <V1> - <V2>
fvector2 fvector2_subtract(const fvector2* vector1, const fvector2* vector2);
fvector3 fvector3_subtract(const fvector3* vector1, const fvector3* vector2);
fvector4 fvector4_subtract(const fvector4* vector1, const fvector4* vector2);

// <V>*s
fvector2 fvector2_scale(const fvector2* vector, float scalar);
fvector3 fvector3_scale(const fvector3* vector, float scalar);
fvector4 fvector4_scale(const fvector4* vector, float scalar);

// <V1> dot product <V2>
float fvector2_dot(const fvector2* vector1, const fvector2* vector2);
float fvector3_dot(const fvector3* vector1, const fvector3* vector2);

// <V1> cross product <V2>
fvector3 fvector3_cross(const fvector3* vector1, const fvector3* vector2);