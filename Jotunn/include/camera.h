#pragma once

#include "fmatrix.h"
#include "fvector.h"
#include "fquaternion.h"

struct camera_base_t
{
   fmatrix_4x4 projection_matrix;
   fmatrix_4x4 view_matrix;
   fmatrix_4x4 view_projection_matrix;

   float aspect_ratio;
   float near_plane;
   float far_plane;

   fvector3 position;
   fquaternion orientation;
};

void camera_init(struct camera_base_t* camera);

void camera_recalculate_view_matrix(struct camera_base_t* camera);
void camera_set_projection(struct camera_base_t* camera, float aspect_ratio, float near_plane, float far_plane);
fvector3 camera_get_front_vector(struct camera_base_t* camera);
fvector3 camera_get_left_vector(struct camera_base_t* camera);
fvector3 camera_get_up_vector(struct camera_base_t* camera);