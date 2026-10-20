#include "u8vector.h"

static uint8_t _u8vector_component_from_float(
  const float in_component);

/*******************************************************************************
 *
 *   Function: u8vector4_from_fvector4()
 *
 ******************************************************************************/
union u8vector4_t u8vector4_from_fvector4(
  const fvector4 in_vector)
{
  union u8vector4_t vector;

  for (unsigned int i = 0; i < 4; i++)
  {
    vector.vec[i] = _u8vector_component_from_float(in_vector.vec[i]);
  }

  return vector;
}

/*******************************************************************************
 *
 *   Function: u8vector4_to_fvector4()
 *
 ******************************************************************************/
fvector4 u8vector4_to_fvector4(
  const union u8vector4_t in_vector)
{
  fvector4 vector;

  for (unsigned int i = 0; i < 4; i++)
  {
    vector.vec[i] = ((float)in_vector.vec[i] / 255.0f);
  }

  return vector;
}

/*******************************************************************************
 *
 *   Function: _u8vector_component_from_float()
 *
 ******************************************************************************/
static uint8_t _u8vector_component_from_float(
  const float in_component)
{
  const float clamped_component =
    (in_component < 0.0f) ?
      0.0f :
      ((in_component > 1.0f) ? 1.0f : in_component);

  return (uint8_t)((clamped_component * 255.0f) + 0.5f);
}
