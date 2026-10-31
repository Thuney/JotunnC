#include <memory.h>

#include "fquaternion.h"

/*******************************************************************************
 *
 *   Function: fquaternion_init()
 *
 ******************************************************************************/
void fquaternion_init(
  union fquaternion_t* out_quaternion)
{
  memset(
    out_quaternion,
    0,
    sizeof(union fquaternion_t));
}
