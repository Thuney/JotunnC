#include <math.h>
#include <memory.h>

#include "fvector.h"

#ifdef DEBUG

/*******************************************************************************
 *
 * Function: fvector2_print()
 *
 ******************************************************************************/
void fvector2_print(
  const fvector2* vector)
{
  fprintf(
    stdout,
    "fvector2 - X = %f, Y = %f\n",
    vector->comp.x,
    vector->comp.y);
}

/*******************************************************************************
 *
 * Function: fvector3_print()
 *
 ******************************************************************************/
void fvector3_print(
  const fvector3* vector)
{
  fprintf(
    stdout,
    "fvector3 - X = %f, Y = %f, Z = %f\n",
    vector->comp.x,
    vector->comp.y,
    vector->comp.z);
}

/*******************************************************************************
 *
 * Function: fvector4_print()
 *
 ******************************************************************************/
void fvector4_print(
  const fvector4* vector)
{
  fprintf(
    stdout,
    "fvector4 - W = %f, X = %f, Y = %f, Z = %f\n",
    vector->comp.w,
    vector->comp.x,
    vector->comp.y,
    vector->comp.z);
}
#endif

/*******************************************************************************
 *
 * Function: fvector2_init()
 *
 ******************************************************************************/
void fvector2_init(
  fvector2* vector)
{
  memset(
    vector,
    0,
    sizeof(fvector2));
}

/*******************************************************************************
 *
 * Function: fvector3_init()
 *
 ******************************************************************************/
void fvector3_init(
  fvector3* vector)
{
  memset(
    vector,
    0,
    sizeof(fvector3));
}

/*******************************************************************************
 *
 * Function: fvector4_init()
 *
 ******************************************************************************/
void fvector4_init(
  fvector4* vector)
{
  memset(
    vector,
    0,
    sizeof(fvector4));
}

/*******************************************************************************
 *
 * Function: fvector2_set()
 *
 ******************************************************************************/
void fvector2_set(
  fvector2* vector,
  const float x,
  const float y)
{
  vector->comp.x = x;
  vector->comp.y = y;
}

/*******************************************************************************
 *
 * Function: fvector3_set()
 *
 ******************************************************************************/
void fvector3_set(
  fvector3* vector,
  const float x,
  const float y,
  const float z)
{
  vector->comp.x = x;
  vector->comp.y = y;
  vector->comp.z = z;
}

/*******************************************************************************
 *
 * Function: fvector4_set()
 *
 ******************************************************************************/
void fvector4_set(
  fvector4* vector,
  const float w,
  const float x,
  const float y,
  const float z)
{
  vector->comp.w = w;
  vector->comp.x = x;
  vector->comp.y = y;
  vector->comp.z = z;
}

/*******************************************************************************
 *
 * Function: fvector2_setv()
 *
 ******************************************************************************/
void fvector2_setv(
  fvector2* dest_vector,
  const fvector2 src_vector)
{
  fvector2_set(
    dest_vector,
    src_vector.comp.x,
    src_vector.comp.y);
}

/*******************************************************************************
 *
 * Function: fvector3_setv()
 *
 ******************************************************************************/
void fvector3_setv(
  fvector3* dest_vector,
  const fvector3 src_vector)
{
  fvector3_set(
    dest_vector,
    src_vector.comp.x,
    src_vector.comp.y,
    src_vector.comp.z);
}

/*******************************************************************************
 *
 * Function: fvector4_setv()
 *
 ******************************************************************************/
void fvector4_setv(
  fvector4* dest_vector,
  const fvector4 src_vector)
{
  fvector4_set(
    dest_vector,
    src_vector.comp.w,
    src_vector.comp.x,
    src_vector.comp.y,
    src_vector.comp.z);
}

/*******************************************************************************
 *
 * Function: fvector2_magnitude()
 *
 ******************************************************************************/
float fvector2_magnitude(
  const fvector2* vector)
{
  float magnitude;
  magnitude = sqrtf(
    (vector->comp.x * vector->comp.x) + (vector->comp.y * vector->comp.y));
  return magnitude;
}

/*******************************************************************************
 *
 * Function: fvector3_magnitude()
 *
 ******************************************************************************/
float fvector3_magnitude(
  const fvector3* vector)
{
  float magnitude;
  magnitude = sqrtf(
    (vector->comp.x * vector->comp.x) + (vector->comp.y * vector->comp.y) +
    (vector->comp.z * vector->comp.z));
  return magnitude;
}

/*******************************************************************************
 *
 * Function: fvector4_magnitude()
 *
 ******************************************************************************/
float fvector4_magnitude(
  const fvector4* vector)
{
  float magnitude;
  magnitude = sqrtf(
    (vector->comp.w * vector->comp.w) + (vector->comp.x * vector->comp.x) +
    (vector->comp.y * vector->comp.y) + (vector->comp.z * vector->comp.z));
  return magnitude;
}

/*******************************************************************************
 *
 * Function: fvector2_normalize()
 *
 ******************************************************************************/
fvector2 fvector2_normalize(
  const fvector2* vector)
{
  // float magnitude = fvector2_magnitude(vector);
  // return (fvector2) { (vector->comp.x / magnitude), (vector->comp.y / magnitude) };

  return fvector2_scale(
    vector,
    1.0f / fvector2_magnitude(vector));
}

/*******************************************************************************
 *
 * Function: fvector3_normalize()
 *
 ******************************************************************************/
fvector3 fvector3_normalize(
  const fvector3* vector)
{
  return fvector3_scale(
    vector,
    1.0f / fvector3_magnitude(vector));
}

/*******************************************************************************
 *
 * Function: fvector4_normalize()
 *
 ******************************************************************************/
fvector4 fvector4_normalize(
  const fvector4* vector)
{
  return fvector4_scale(
    vector,
    1.0f / fvector4_magnitude(vector));
}

/*******************************************************************************
 *
 * Function: fvector2_add()
 *
 ******************************************************************************/
fvector2 fvector2_add(
  const fvector2* vector1,
  const fvector2* vector2)
{
  fvector2 resultant;
  resultant.comp.x = (vector1->comp.x + vector2->comp.x);
  resultant.comp.y = (vector1->comp.y + vector2->comp.y);

  return resultant;
}

/*******************************************************************************
 *
 * Function: fvector3_add()
 *
 ******************************************************************************/
fvector3 fvector3_add(
  const fvector3* vector1,
  const fvector3* vector2)
{
  fvector3 resultant;
  resultant.comp.x = (vector1->comp.x + vector2->comp.x);
  resultant.comp.y = (vector1->comp.y + vector2->comp.y);
  resultant.comp.z = (vector1->comp.z + vector2->comp.z);

  return resultant;
}

/*******************************************************************************
 *
 * Function: fvector4_add()
 *
 ******************************************************************************/
fvector4 fvector4_add(
  const fvector4* vector1,
  const fvector4* vector2)
{
  fvector4 resultant;
  resultant.comp.x = (vector1->comp.w + vector2->comp.w);
  resultant.comp.x = (vector1->comp.x + vector2->comp.x);
  resultant.comp.y = (vector1->comp.y + vector2->comp.y);
  resultant.comp.z = (vector1->comp.z + vector2->comp.z);

  return resultant;
}

/*******************************************************************************
 *
 * Function: fvector2_subtract()
 *
 ******************************************************************************/
fvector2 fvector2_subtract(
  const fvector2* vector1,
  const fvector2* vector2)
{
  fvector2 resultant;
  resultant.comp.x = (vector1->comp.x - vector2->comp.x);
  resultant.comp.y = (vector1->comp.y - vector2->comp.y);

  return resultant;
}

/*******************************************************************************
 *
 * Function: fvector3_subtract()
 *
 ******************************************************************************/
fvector3 fvector3_subtract(
  const fvector3* vector1,
  const fvector3* vector2)
{
  fvector3 resultant;
  resultant.comp.x = (vector1->comp.x - vector2->comp.x);
  resultant.comp.y = (vector1->comp.y - vector2->comp.y);
  resultant.comp.z = (vector1->comp.z - vector2->comp.z);

  return resultant;
}

/*******************************************************************************
 *
 * Function: fvector4_subtract()
 *
 ******************************************************************************/
fvector4 fvector4_subtract(
  const fvector4* vector1,
  const fvector4* vector2)
{
  fvector4 resultant;
  resultant.comp.x = (vector1->comp.w - vector2->comp.w);
  resultant.comp.x = (vector1->comp.x - vector2->comp.x);
  resultant.comp.y = (vector1->comp.y - vector2->comp.y);
  resultant.comp.z = (vector1->comp.z - vector2->comp.z);

  return resultant;
}

// <V>*s

/*******************************************************************************
 *
 * Function: fvector2_scale()
 *
 ******************************************************************************/
fvector2 fvector2_scale(
  const fvector2* vector,
  float scalar)
{
  return (fvector2){(vector->comp.x * scalar), (vector->comp.y * scalar)};
}

/*******************************************************************************
 *
 * Function: fvector3_scale()
 *
 ******************************************************************************/
fvector3 fvector3_scale(
  const fvector3* vector,
  float scalar)
{
  return (fvector3){(vector->comp.x * scalar),
                    vector->comp.y * scalar,
                    (vector->comp.z * scalar)};
}

/*******************************************************************************
 *
 * Function: fvector4_scale()
 *
 ******************************************************************************/
fvector4 fvector4_scale(
  const fvector4* vector,
  float scalar)
{
  return (fvector4){(vector->comp.w * scalar),
                    vector->comp.x * scalar,
                    vector->comp.y * scalar,
                    (vector->comp.z * scalar)};
}

// <V1> dot product <V2>

/*******************************************************************************
 *
 * Function: fvector2_dot()
 *
 ******************************************************************************/
float fvector2_dot(
  const fvector2* vector1,
  const fvector2* vector2)
{
  return (
    (vector1->comp.x * vector2->comp.x) + (vector1->comp.y * vector2->comp.y));
}

/*******************************************************************************
 *
 * Function: fvector3_dot()
 *
 ******************************************************************************/
float fvector3_dot(
  const fvector3* vector1,
  const fvector3* vector2)
{
  return (
    (vector1->comp.x * vector2->comp.x) + (vector1->comp.y * vector2->comp.y) +
    (vector1->comp.z * vector2->comp.z));
}

// <V1> cross product <V2>

/*******************************************************************************
 *
 * Function: fvector3_cross()
 *
 ******************************************************************************/
fvector3 fvector3_cross(
  const fvector3* vector1,
  const fvector3* vector2)
{
  fvector3 resultant;
  resultant.comp.x =
    ((vector1->comp.y * vector2->comp.z) - (vector1->comp.z * vector2->comp.y));
  resultant.comp.y =
    ((vector1->comp.z * vector2->comp.x) - (vector1->comp.x * vector2->comp.z));
  resultant.comp.z =
    ((vector1->comp.x * vector2->comp.y) - (vector1->comp.y * vector2->comp.x));
  return resultant;
}
