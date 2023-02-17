#pragma once

#include "fmatrix.h"
#include "fvector.h"
#include "fquaternion.h"

enum camera_projection_t
{
   CAMERA_ORTHOGRAPHIC,
   CAMERA_PERSPECTIVE
};

struct camera_base_t
{
   enum camera_projection_t projection_type;

   fmatrix_4x4 projection_matrix;
   fmatrix_4x4 view_matrix;
   fmatrix_4x4 view_projection_matrix;

   fvector3 position;
   fvector3 up;
   fvector3 front;

   float near_plane;
   float far_plane;
   //
   void (*camera_reproject)(struct camera_base_t* camera, float window_width, float window_height, float near_plane, float far_plane);
};

struct camera_ortho_t
{
   struct camera_base_t base;
   //
   float left;
   float right;
   float top;
   float bottom;
};

struct camera_perspective_t
{
   struct camera_base_t base;
   //
   float aspect_ratio;
};

void camera_init_orthographic(struct camera_ortho_t* camera, const fvector3 position, const fvector3 up, const fvector3 front);
void camera_init_perspective(struct camera_perspective_t* camera, const fvector3 position, const fvector3 up, const fvector3 front);

void camera_set_projection_orthographic(struct camera_ortho_t* ortho_camera, const float left, const float right, const float top, const float bottom, const float near_plane, const float far_plane);
void camera_set_projection_perspective(struct camera_perspective_t* perspective_camera, const float aspect_ratio, const float near_plane, const float far_plane);

void camera_recalculate_view_projection_matrix(struct camera_base_t* camera);
fvector3 camera_get_front_vector(struct camera_base_t* camera);
fvector3 camera_get_left_vector(struct camera_base_t* camera);
fvector3 camera_get_up_vector(struct camera_base_t* camera);