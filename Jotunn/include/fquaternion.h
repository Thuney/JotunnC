#pragma once

typedef union fquaternion_t
{
   float quat[4];
   struct comp_t
   {
      float w, x, y, z;
   } comp;
} fquaternion;

void fquaternion_init(fquaternion* quaternion);