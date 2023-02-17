#include "camera.h"

#ifdef DEBUG
   #include <stdio.h>
#endif

static void camera_reproject(struct camera_base_t* camera, float window_width, float window_height, float near_plane, float far_plane)
{
   switch (camera->projection_type)
   {
      case CAMERA_ORTHOGRAPHIC:
      {
         struct camera_ortho_t* camera_ortho = (struct camera_ortho_t*)camera;
         camera_set_projection_orthographic(camera_ortho, 0.0f, window_width, window_height, 0.0f, near_plane, far_plane);
      }
      break;
      case CAMERA_PERSPECTIVE:
      {
         struct camera_perspective_t* camera_perspective = (struct camera_perspective_t*)camera;
         camera_set_projection_perspective(camera_perspective, (window_width/window_height), near_plane, far_plane);
      }
      break;
   }
}

static fmatrix_4x4 position_as_matrix(const fvector3 position)
{
   fmatrix_4x4 position_as_matrix;

   const float position_as_matrix_data[4][4] = {{ 1.0f, 0.0f, 0.0f, -position.comp.x }, 
                                                { 0.0f, 1.0f, 0.0f, -position.comp.y }, 
                                                { 0.0f, 0.0f, 1.0f, -position.comp.z }, 
                                                { 0.0f, 0.0f, 0.0f, 1.0f }};

   fmatrix_4x4_set(&(position_as_matrix), position_as_matrix_data);

   return position_as_matrix;
}

static fmatrix_4x4 calculate_look_at_matrix(const fvector3 position, const fvector3 target, const fvector3 up)
{
   fvector3 direction, right;

   fvector3 pos_minus_target, up_cross_direction;

   pos_minus_target = fvector3_subtract(&position, &target);
   direction = fvector3_normalize(&pos_minus_target);

   up_cross_direction = fvector3_cross(&up, &direction);
   right = fvector3_normalize(&up_cross_direction);

   fmatrix_4x4 coordinate_matrix;

   const float coordinate_matrix_data[4][4] = {{     right.comp.x,     right.comp.y,     right.comp.z, 0.0f }, 
                                               {        up.comp.x,        up.comp.y,        up.comp.z, 0.0f }, 
                                               { direction.comp.x, direction.comp.y, direction.comp.z, 0.0f }, 
                                               {             0.0f,             0.0f,             0.0f, 1.0f }};

   fmatrix_4x4_set(&(coordinate_matrix), coordinate_matrix_data);

   fmatrix_4x4 pos_matrix = position_as_matrix(position);

   fmatrix_4x4 look_at_matrix = fmatrix_4x4_multiply(&coordinate_matrix, &pos_matrix);
   fmatrix_4x4_transpose(&look_at_matrix);

   return look_at_matrix;
}

static void camera_init(struct camera_base_t* camera, const fvector3 position, const fvector3 up, const fvector3 front)
{
   fmatrix_4x4_init(&camera->projection_matrix);
   fmatrix_4x4_init(&camera->view_matrix);
   fmatrix_4x4_init(&camera->view_projection_matrix);

   fvector3_setv(&(camera->position), position);
   fvector3_setv(&(camera->up), up);
   fvector3_setv(&(camera->front), front);

   fvector3 target = fvector3_add(&position, &front);

   camera->view_matrix = calculate_look_at_matrix(position, target, up);
   camera_recalculate_view_projection_matrix(camera);

   camera->camera_reproject = &camera_reproject;
}

void camera_init_orthographic(struct camera_ortho_t* camera, const fvector3 position, const fvector3 up, const fvector3 front)
{
   camera_init(&(camera->base), position, up, front);
}

void camera_init_perspective(struct camera_perspective_t* camera, const fvector3 position, const fvector3 up, const fvector3 front)
{
   camera_init(&(camera->base), position, up, front);
}

void camera_set_projection_orthographic(struct camera_ortho_t* ortho_camera, const float left, const float right, const float top, const float bottom, const float near_plane, const float far_plane)
{
   struct camera_base_t* camera_base = &ortho_camera->base;

   camera_base->near_plane = near_plane;
   camera_base->far_plane  = far_plane;

   ortho_camera->left       = left;
   ortho_camera->right      = right;
   ortho_camera->top        = top;
   ortho_camera->bottom     = bottom;

   camera_base->projection_type = CAMERA_ORTHOGRAPHIC;

   const float ortho_matrix[4][4] = {{ (2.0f/(right - left)), 0.0f, 0.0f, -((right + left)/(right - left)) }, 
                                     { 0.0f, (2.0f/(top - bottom)), 0.0f, -((top + bottom)/(top - bottom)) }, 
                                     { 0.0f, 0.0f, (-2.0f/(far_plane - near_plane)), -((far_plane + near_plane)/(far_plane - near_plane)) }, 
                                     { 0.0f, 0.0f, 0.0f, 1.0f }};

   fmatrix_4x4_set(&camera_base->projection_matrix, ortho_matrix);

   fmatrix_4x4_transpose(&camera_base->projection_matrix);

   // #ifdef DEBUG
   //    fmatrix_4x4_print(&camera_base->projection_matrix);
   // #endif

   camera_recalculate_view_projection_matrix(camera_base);
}

void camera_set_projection_perspective(struct camera_perspective_t* perspective_camera, const float aspect_ratio, const float near_plane, const float far_plane)
{
   struct camera_base_t* camera_base = &perspective_camera->base;

   camera_base->near_plane = near_plane;
   camera_base->far_plane  = far_plane;

   perspective_camera->aspect_ratio = aspect_ratio;

   camera_base->projection_type = CAMERA_PERSPECTIVE;

   const float persp_matrix[4][4] = {{ 0.0f, 0.0f, 0.0f, 0.0f }, 
                                     { 0.0f, 0.0f, 0.0f, 0.0f }, 
                                     { 0.0f, 0.0f, 0.0f, 0.0f }, 
                                     { 0.0f, 0.0f, 0.0f, 0.0f }};

   fmatrix_4x4_set(&camera_base->projection_matrix, persp_matrix);
   fmatrix_4x4_transpose(&camera_base->projection_matrix);

   camera_recalculate_view_projection_matrix(camera_base);
}

void camera_recalculate_view_projection_matrix(struct camera_base_t* camera)
{
   camera->view_projection_matrix = fmatrix_4x4_multiply(&(camera->view_matrix), &(camera->projection_matrix));
}

fvector3 camera_get_front_vector(struct camera_base_t* camera)
{
   fvector3 front;
   fvector3_init(&front);

   return front;
}

fvector3 camera_get_left_vector(struct camera_base_t* camera)
{
   fvector3 left;
   fvector3_init(&left);

   return left;
}

fvector3 camera_get_up_vector(struct camera_base_t* camera)
{
   fvector3 up;
   fvector3_init(&up);

   return up;
}
