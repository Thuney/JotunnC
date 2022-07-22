#include "camera.h"

void camera_init(struct camera_base_t* camera)
{
   fmatrix_4x4_init(&camera->projection_matrix);
   fmatrix_4x4_init(&camera->view_matrix);
   fmatrix_4x4_init(&camera->view_projection_matrix);

   fvector3_init(&camera->position);
   fquaternion_init(&camera->orientation);

   camera_set_projection(camera, 0.0f, 0.0f, 0.0f);
}

void camera_recalculate_view_matrix(struct camera_base_t* camera)
{
   
}

void camera_set_projection(struct camera_base_t* camera, float aspect_ratio, float near_plane, float far_plane)
{
   camera->aspect_ratio = aspect_ratio;
   camera->near_plane   = near_plane;
   camera->far_plane    = far_plane;

   camera_recalculate_view_matrix(camera);
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
