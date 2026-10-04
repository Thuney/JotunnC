#include <memory.h>

#include "fquaternion.h"

/*******************************************************************************
 *
 * Function: fquaternion_init()
 *
 ******************************************************************************/
void fquaternion_init(
  fquaternion* quaternion)
{
  memset(
    quaternion,
    0,
    sizeof(fquaternion));
}
