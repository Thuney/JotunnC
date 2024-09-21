#include <memory.h>

#include "fquaternion.h"

void fquaternion_init(fquaternion* quaternion)
{
   memset(quaternion, 0, sizeof(fquaternion));
}