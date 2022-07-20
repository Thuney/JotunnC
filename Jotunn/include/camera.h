#pragma once

#include "fmatrix.h"
#include "fvector.h"
#include "fquaternion.h"

struct camera_base_t
{
   struct fmatrix_4x4_t projection_matrix;
   struct fmatrix_4x4_t view_matrix;
   struct fmatrix_4x4_t view_projection_matrix;

   float aspect_ratio;
   float near_plane;
   float far_plane;

   struct fvector3 position;
   struct fquaternion orientation;
};

void camera_init(struct camera_base_t* camera);

void camera_recalculate_view_matrix(struct camera_base_t* camera);
void camera_set_projection(struct camera_base_t* camera, float aspect_ratio, float near_plane, float far_plane);
struct fvector3 camera_get_front_vector(struct camera_base_t* camera);
struct fvector3 camera_get_left_vector(struct camera_base_t* camera);
struct fvector3 camera_get_up_vector(struct camera_base_t* camera);