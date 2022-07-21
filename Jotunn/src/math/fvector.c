#include <math.h>
#include <memory.h>

#include "fvector.h"

void fvector2_init(struct fvector2* vector)
{
   memset(vector, 0, sizeof(struct fvector2));
}

void fvector3_init(struct fvector3* vector)
{
   memset(vector, 0, sizeof(struct fvector3));
}

float fvector2_magnitude(struct fvector2 vector)
{
   float magnitude;
   magnitude = sqrtf((vector.x*vector.x) + (vector.y * vector.y));
   return magnitude;
}

float fvector3_magnitude(struct fvector3 vector)
{
   float magnitude;
   magnitude = sqrtf((vector.x*vector.x) + (vector.y * vector.y) + (vector.z * vector.z));
   return magnitude;
}

struct fvector2 fvector2_add(struct fvector2 vector1, struct fvector2 vector2)
{
   struct fvector2 resultant;
   resultant.x = (vector1.x + vector2.x);
   resultant.y = (vector1.y + vector2.y);

   return resultant;
}

struct fvector3 fvector3_add(struct fvector3 vector1, struct fvector3 vector2)
{
   struct fvector3 resultant;
   resultant.x = (vector1.x + vector2.x);
   resultant.y = (vector1.y + vector2.y);
   resultant.y = (vector1.z + vector2.z);

   return resultant;
}

struct fvector2 fvector2_subtract(struct fvector2 vector1, struct fvector2 vector2)
{
   struct fvector2 resultant;
   resultant.x = (vector1.x - vector2.x);
   resultant.y = (vector1.y - vector2.y);

   return resultant;
}

struct fvector3 fvector3_subtract(struct fvector3 vector1, struct fvector3 vector2)
{
   struct fvector3 resultant;
   resultant.x = (vector1.x - vector2.x);
   resultant.y = (vector1.y - vector2.y);
   resultant.y = (vector1.z - vector2.z);

   return resultant;
}

// <V>*s
struct fvector2 fvector2_scale(struct fvector2 vector, float scalar)
{
   struct fvector2 resultant;
   resultant.x = (vector.x * scalar);
   resultant.y = (vector.y * scalar);

   return resultant;
}

struct fvector3 fvector3_scale(struct fvector3 vector, float scalar)
{
   struct fvector3 resultant;
   resultant.x = (vector.x * scalar);
   resultant.y = (vector.y * scalar);
   resultant.z = (vector.z * scalar);

   return resultant;
}

// <V1> dot product <V2>
float fvector2_dot(struct fvector2 vector1, struct fvector2 vector2)
{
   return ((vector1.x*vector2.x)+(vector1.y*vector2.y));
}

float fvector3_dot(struct fvector3 vector1, struct fvector3 vector2)
{
   return ((vector1.x*vector2.x)+(vector1.y*vector2.y)+(vector1.z*vector2.z));
}

// <V1> cross product <V2>
struct fvector3 fvector3_cross(struct fvector3 vector1, struct fvector3 vector2)
{
   struct fvector3 resultant;
   resultant.x = ((vector1.y*vector2.z) - (vector1.z*vector2.y));
   resultant.y = ((vector1.z*vector2.x) - (vector1.x*vector2.z));
   resultant.z = ((vector1.x*vector2.y) - (vector1.y*vector2.x));
   return resultant;
}