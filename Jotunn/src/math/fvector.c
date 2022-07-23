#include <math.h>
#include <memory.h>

#include "fvector.h"

#ifdef DEBUG
void fvector2_print(const fvector2* vector)
{
   fprintf(stdout, "fvector2 - X = %f, Y = %f\n", vector->comp.x, vector->comp.y);
}

void fvector3_print(const fvector3* vector)
{
   fprintf(stdout, "fvector3 - X = %f, Y = %f, Z = %f\n", vector->comp.x, vector->comp.y, vector->comp.z);
}

void fvector4_print(const fvector4* vector)
{
   fprintf(stdout, "fvector4 - W = %f, X = %f, Y = %f, Z = %f\n", vector->comp.w, vector->comp.x, vector->comp.y, vector->comp.z);
}
#endif

void fvector2_init(fvector2* vector)
{
   memset(vector, 0, sizeof(fvector2));
}

void fvector3_init(fvector3* vector)
{
   memset(vector, 0, sizeof(fvector3));
}

void fvector4_init(fvector4* vector)
{
   memset(vector, 0, sizeof(fvector4));
}

void fvector2_set(fvector2* vector, float x, float y)
{
   vector->comp.x = x;
   vector->comp.y = y;
}

void fvector3_set(fvector3* vector, float x, float y, float z)
{
   vector->comp.x = x;
   vector->comp.y = y;
   vector->comp.x = z;
}

void fvector4_set(fvector4* vector, float w, float x, float y, float z)
{
   vector->comp.w = w;
   vector->comp.x = x;
   vector->comp.y = y;
   vector->comp.z = z;
}

float fvector2_magnitude(const fvector2* vector)
{
   float magnitude;
   magnitude = sqrtf((vector->comp.x*vector->comp.x) + (vector->comp.y * vector->comp.y));
   return magnitude;
}

float fvector3_magnitude(const fvector3* vector)
{
   float magnitude;
   magnitude = sqrtf((vector->comp.x*vector->comp.x) + (vector->comp.y * vector->comp.y) + (vector->comp.z * vector->comp.z));
   return magnitude;
}

float fvector4_magnitude(const fvector4* vector)
{
   float magnitude;
   magnitude = sqrtf((vector->comp.w*vector->comp.w) + (vector->comp.x*vector->comp.x) + (vector->comp.y * vector->comp.y) + (vector->comp.z * vector->comp.z));
   return magnitude;
}

fvector2 fvector2_add(const fvector2* vector1, const fvector2* vector2)
{
   fvector2 resultant;
   resultant.comp.x = (vector1->comp.x + vector2->comp.x);
   resultant.comp.y = (vector1->comp.y + vector2->comp.y);

   return resultant;
}

fvector3 fvector3_add(const fvector3* vector1, const fvector3* vector2)
{
   fvector3 resultant;
   resultant.comp.x = (vector1->comp.x + vector2->comp.x);
   resultant.comp.y = (vector1->comp.y + vector2->comp.y);
   resultant.comp.z = (vector1->comp.z + vector2->comp.z);

   return resultant;
}

fvector4 fvector4_add(const fvector4* vector1, const fvector4* vector2)
{
   fvector4 resultant;
   resultant.comp.x = (vector1->comp.w + vector2->comp.w);
   resultant.comp.x = (vector1->comp.x + vector2->comp.x);
   resultant.comp.y = (vector1->comp.y + vector2->comp.y);
   resultant.comp.z = (vector1->comp.z + vector2->comp.z);

   return resultant;
}

fvector2 fvector2_subtract(const fvector2* vector1, const fvector2* vector2)
{
   fvector2 resultant;
   resultant.comp.x = (vector1->comp.x - vector2->comp.x);
   resultant.comp.y = (vector1->comp.y - vector2->comp.y);

   return resultant;
}

fvector3 fvector3_subtract(const fvector3* vector1, const fvector3* vector2)
{
   fvector3 resultant;
   resultant.comp.x = (vector1->comp.x - vector2->comp.x);
   resultant.comp.y = (vector1->comp.y - vector2->comp.y);
   resultant.comp.z = (vector1->comp.z - vector2->comp.z);

   return resultant;
}

fvector4 fvector4_subtract(const fvector4* vector1, const fvector4* vector2)
{
   fvector4 resultant;
   resultant.comp.x = (vector1->comp.w - vector2->comp.w);
   resultant.comp.x = (vector1->comp.x - vector2->comp.x);
   resultant.comp.y = (vector1->comp.y - vector2->comp.y);
   resultant.comp.z = (vector1->comp.z - vector2->comp.z);

   return resultant;
}

// <V>*s
fvector2 fvector2_scale(const fvector2* vector, float scalar)
{
   fvector2 resultant;
   resultant.comp.x = (vector->comp.x * scalar);
   resultant.comp.y = (vector->comp.y * scalar);

   return resultant;
}

fvector3 fvector3_scale(const fvector3* vector, float scalar)
{
   fvector3 resultant;
   resultant.comp.x = (vector->comp.x * scalar);
   resultant.comp.y = (vector->comp.y * scalar);
   resultant.comp.z = (vector->comp.z * scalar);

   return resultant;
}

fvector4 fvector4_scale(const fvector4* vector, float scalar)
{
   fvector4 resultant;
   resultant.comp.w = (vector->comp.w * scalar);
   resultant.comp.x = (vector->comp.x * scalar);
   resultant.comp.y = (vector->comp.y * scalar);
   resultant.comp.z = (vector->comp.z * scalar);

   return resultant;
}

// <V1> dot product <V2>
float fvector2_dot(const fvector2* vector1, const fvector2* vector2)
{
   return ((vector1->comp.x*vector2->comp.x)+(vector1->comp.y*vector2->comp.y));
}

float fvector3_dot(const fvector3* vector1, const fvector3* vector2)
{
   return ((vector1->comp.x*vector2->comp.x)+(vector1->comp.y*vector2->comp.y)+(vector1->comp.z*vector2->comp.z));
}

// <V1> cross product <V2>
fvector3 fvector3_cross(const fvector3* vector1, const fvector3* vector2)
{
   fvector3 resultant;
   resultant.comp.x = ((vector1->comp.y*vector2->comp.z) - (vector1->comp.z*vector2->comp.y));
   resultant.comp.y = ((vector1->comp.z*vector2->comp.x) - (vector1->comp.x*vector2->comp.z));
   resultant.comp.z = ((vector1->comp.x*vector2->comp.y) - (vector1->comp.y*vector2->comp.x));
   return resultant;
}