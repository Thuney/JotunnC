#include <math.h>
#include <memory.h>

#include "fvector.h"

#ifdef DEBUG

/*******************************************************************************
 *
 *   Function: fvector2_print()
 *
 ******************************************************************************/
void fvector2_print(
  const union fvector2_t* in_vector)
{
  fprintf(
    stdout,
    "fvector2 - X = %f, Y = %f\n",
    in_vector->comp.x,
    in_vector->comp.y);
}

/*******************************************************************************
 *
 *   Function: fvector3_print()
 *
 ******************************************************************************/
void fvector3_print(
  const union fvector3_t* in_vector)
{
  fprintf(
    stdout,
    "fvector3 - X = %f, Y = %f, Z = %f\n",
    in_vector->comp.x,
    in_vector->comp.y,
    in_vector->comp.z);
}

/*******************************************************************************
 *
 *   Function: fvector4_print()
 *
 ******************************************************************************/
void fvector4_print(
  const union fvector4_t* in_vector)
{
  fprintf(
    stdout,
    "fvector4 - W = %f, X = %f, Y = %f, Z = %f\n",
    in_vector->comp.w,
    in_vector->comp.x,
    in_vector->comp.y,
    in_vector->comp.z);
}

#endif

/*******************************************************************************
 *
 *   Function: fvector2_init()
 *
 ******************************************************************************/
void fvector2_init(
  union fvector2_t* out_vector)
{
  memset(
    out_vector,
    0,
    sizeof(union fvector2_t));
}

/*******************************************************************************
 *
 *   Function: fvector3_init()
 *
 ******************************************************************************/
void fvector3_init(
  union fvector3_t* out_vector)
{
  memset(
    out_vector,
    0,
    sizeof(union fvector3_t));
}

/*******************************************************************************
 *
 *   Function: fvector4_init()
 *
 ******************************************************************************/
void fvector4_init(
  union fvector4_t* out_vector)
{
  memset(
    out_vector,
    0,
    sizeof(union fvector4_t));
}

/*******************************************************************************
 *
 *   Function: fvector2_set()
 *
 ******************************************************************************/
void fvector2_set(
  union fvector2_t* out_vector,
  float in_x,
  float in_y)
{
  out_vector->comp.x = in_x;
  out_vector->comp.y = in_y;
}

/*******************************************************************************
 *
 *   Function: fvector3_set()
 *
 ******************************************************************************/
void fvector3_set(
  union fvector3_t* out_vector,
  float in_x,
  float in_y,
  float in_z)
{
  out_vector->comp.x = in_x;
  out_vector->comp.y = in_y;
  out_vector->comp.z = in_z;
}

/*******************************************************************************
 *
 *   Function: fvector4_set()
 *
 ******************************************************************************/
void fvector4_set(
  union fvector4_t* out_vector,
  float in_w,
  float in_x,
  float in_y,
  float in_z)
{
  out_vector->comp.w = in_w;
  out_vector->comp.x = in_x;
  out_vector->comp.y = in_y;
  out_vector->comp.z = in_z;
}

/*******************************************************************************
 *
 *   Function: fvector2_setv()
 *
 ******************************************************************************/
void fvector2_setv(
  union fvector2_t* out_vector,
  union fvector2_t in_vector)
{
  fvector2_set(
    out_vector,
    in_vector.comp.x,
    in_vector.comp.y);
}

/*******************************************************************************
 *
 *   Function: fvector3_setv()
 *
 ******************************************************************************/
void fvector3_setv(
  union fvector3_t* out_vector,
  union fvector3_t in_vector)
{
  fvector3_set(
    out_vector,
    in_vector.comp.x,
    in_vector.comp.y,
    in_vector.comp.z);
}

/*******************************************************************************
 *
 *   Function: fvector4_setv()
 *
 ******************************************************************************/
void fvector4_setv(
  union fvector4_t* out_vector,
  union fvector4_t in_vector)
{
  fvector4_set(
    out_vector,
    in_vector.comp.w,
    in_vector.comp.x,
    in_vector.comp.y,
    in_vector.comp.z);
}

/*******************************************************************************
 *
 *   Function: fvector2_magnitude()
 *
 ******************************************************************************/
float fvector2_magnitude(
  const union fvector2_t* in_vector)
{
  const float magnitude = sqrtf(
    (in_vector->comp.x * in_vector->comp.x) + (in_vector->comp.y * in_vector->comp.y));
  return magnitude;
}

/*******************************************************************************
 *
 *   Function: fvector3_magnitude()
 *
 ******************************************************************************/
float fvector3_magnitude(
  const union fvector3_t* in_vector)
{
  const float magnitude = sqrtf(
    (in_vector->comp.x * in_vector->comp.x) + (in_vector->comp.y * in_vector->comp.y) +
    (in_vector->comp.z * in_vector->comp.z));
  return magnitude;
}

/*******************************************************************************
 *
 *   Function: fvector4_magnitude()
 *
 ******************************************************************************/
float fvector4_magnitude(
  const union fvector4_t* in_vector)
{
  const float magnitude = sqrtf(
    (in_vector->comp.w * in_vector->comp.w) + (in_vector->comp.x * in_vector->comp.x) +
    (in_vector->comp.y * in_vector->comp.y) + (in_vector->comp.z * in_vector->comp.z));
  return magnitude;
}

/*******************************************************************************
 *
 *   Function: fvector2_normalize()
 *
 ******************************************************************************/
union fvector2_t fvector2_normalize(
  const union fvector2_t* in_vector)
{
  return fvector2_scale(
    in_vector,
    1.0f / fvector2_magnitude(in_vector));
}

/*******************************************************************************
 *
 *   Function: fvector3_normalize()
 *
 ******************************************************************************/
union fvector3_t fvector3_normalize(
  const union fvector3_t* in_vector)
{
  return fvector3_scale(
    in_vector,
    1.0f / fvector3_magnitude(in_vector));
}

/*******************************************************************************
 *
 *   Function: fvector4_normalize()
 *
 ******************************************************************************/
union fvector4_t fvector4_normalize(
  const union fvector4_t* in_vector)
{
  return fvector4_scale(
    in_vector,
    1.0f / fvector4_magnitude(in_vector));
}

/*******************************************************************************
 *
 *   Function: fvector2_add()
 *
 ******************************************************************************/
union fvector2_t fvector2_add(
  const union fvector2_t* in_vector1,
  const union fvector2_t* in_vector2)
{
  union fvector2_t resultant;
  resultant.comp.x = (in_vector1->comp.x + in_vector2->comp.x);
  resultant.comp.y = (in_vector1->comp.y + in_vector2->comp.y);

  return resultant;
}

/*******************************************************************************
 *
 *   Function: fvector3_add()
 *
 ******************************************************************************/
union fvector3_t fvector3_add(
  const union fvector3_t* in_vector1,
  const union fvector3_t* in_vector2)
{
  union fvector3_t resultant;
  resultant.comp.x = (in_vector1->comp.x + in_vector2->comp.x);
  resultant.comp.y = (in_vector1->comp.y + in_vector2->comp.y);
  resultant.comp.z = (in_vector1->comp.z + in_vector2->comp.z);

  return resultant;
}

/*******************************************************************************
 *
 *   Function: fvector4_add()
 *
 ******************************************************************************/
union fvector4_t fvector4_add(
  const union fvector4_t* in_vector1,
  const union fvector4_t* in_vector2)
{
  union fvector4_t resultant;
  resultant.comp.w = (in_vector1->comp.w + in_vector2->comp.w);
  resultant.comp.x = (in_vector1->comp.x + in_vector2->comp.x);
  resultant.comp.y = (in_vector1->comp.y + in_vector2->comp.y);
  resultant.comp.z = (in_vector1->comp.z + in_vector2->comp.z);

  return resultant;
}

/*******************************************************************************
 *
 *   Function: fvector2_subtract()
 *
 ******************************************************************************/
union fvector2_t fvector2_subtract(
  const union fvector2_t* in_vector1,
  const union fvector2_t* in_vector2)
{
  union fvector2_t resultant;
  resultant.comp.x = (in_vector1->comp.x - in_vector2->comp.x);
  resultant.comp.y = (in_vector1->comp.y - in_vector2->comp.y);

  return resultant;
}

/*******************************************************************************
 *
 *   Function: fvector3_subtract()
 *
 ******************************************************************************/
union fvector3_t fvector3_subtract(
  const union fvector3_t* in_vector1,
  const union fvector3_t* in_vector2)
{
  union fvector3_t resultant;
  resultant.comp.x = (in_vector1->comp.x - in_vector2->comp.x);
  resultant.comp.y = (in_vector1->comp.y - in_vector2->comp.y);
  resultant.comp.z = (in_vector1->comp.z - in_vector2->comp.z);

  return resultant;
}

/*******************************************************************************
 *
 *   Function: fvector4_subtract()
 *
 ******************************************************************************/
union fvector4_t fvector4_subtract(
  const union fvector4_t* in_vector1,
  const union fvector4_t* in_vector2)
{
  union fvector4_t resultant;
  resultant.comp.w = (in_vector1->comp.w - in_vector2->comp.w);
  resultant.comp.x = (in_vector1->comp.x - in_vector2->comp.x);
  resultant.comp.y = (in_vector1->comp.y - in_vector2->comp.y);
  resultant.comp.z = (in_vector1->comp.z - in_vector2->comp.z);

  return resultant;
}

/*******************************************************************************
 *
 *   Function: fvector2_scale()
 *
 ******************************************************************************/
union fvector2_t fvector2_scale(
  const union fvector2_t* in_vector,
  float in_scalar)
{
  return (union fvector2_t){(in_vector->comp.x * in_scalar), (in_vector->comp.y * in_scalar)};
}

/*******************************************************************************
 *
 *   Function: fvector3_scale()
 *
 ******************************************************************************/
union fvector3_t fvector3_scale(
  const union fvector3_t* in_vector,
  float in_scalar)
{
  return (union fvector3_t){(in_vector->comp.x * in_scalar),
                    in_vector->comp.y * in_scalar,
                    (in_vector->comp.z * in_scalar)};
}

/*******************************************************************************
 *
 *   Function: fvector4_scale()
 *
 ******************************************************************************/
union fvector4_t fvector4_scale(
  const union fvector4_t* in_vector,
  float in_scalar)
{
  return (union fvector4_t){(in_vector->comp.w * in_scalar),
                    in_vector->comp.x * in_scalar,
                    in_vector->comp.y * in_scalar,
                    (in_vector->comp.z * in_scalar)};
}

/*******************************************************************************
 *
 *   Function: fvector2_dot()
 *
 ******************************************************************************/
float fvector2_dot(
  const union fvector2_t* in_vector1,
  const union fvector2_t* in_vector2)
{
  return (
    (in_vector1->comp.x * in_vector2->comp.x) + (in_vector1->comp.y * in_vector2->comp.y));
}

/*******************************************************************************
 *
 *   Function: fvector3_dot()
 *
 ******************************************************************************/
float fvector3_dot(
  const union fvector3_t* in_vector1,
  const union fvector3_t* in_vector2)
{
  return (
    (in_vector1->comp.x * in_vector2->comp.x) + (in_vector1->comp.y * in_vector2->comp.y) +
    (in_vector1->comp.z * in_vector2->comp.z));
}

/*******************************************************************************
 *
 *   Function: fvector3_cross()
 *
 ******************************************************************************/
union fvector3_t fvector3_cross(
  const union fvector3_t* in_vector1,
  const union fvector3_t* in_vector2)
{
  union fvector3_t resultant;
  resultant.comp.x =
    ((in_vector1->comp.y * in_vector2->comp.z) - (in_vector1->comp.z * in_vector2->comp.y));
  resultant.comp.y =
    ((in_vector1->comp.z * in_vector2->comp.x) - (in_vector1->comp.x * in_vector2->comp.z));
  resultant.comp.z =
    ((in_vector1->comp.x * in_vector2->comp.y) - (in_vector1->comp.y * in_vector2->comp.x));
  return resultant;
}
