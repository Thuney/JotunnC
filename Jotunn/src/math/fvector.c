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