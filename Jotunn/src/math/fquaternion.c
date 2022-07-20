#include <memory.h>

#include "fquaternion.h"

void fquaternion_init(struct fquaternion* quaternion)
{
   memset(quaternion, 0, sizeof(struct fquaternion));
}