#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE matrix_tests
#include <boost/test/unit_test.hpp>

extern "C" 
{
   #include "camera.h"
   #include "fmatrix.h"
   #include "fvector.h"

   #include "stdio.h"
}

#include <math.h>

static bool are_matrices_transpose(const fmatrix_4x4* mat1, const fmatrix_4x4* mat2)
{
   bool are_transpose = true;

   int c, r;
   for (c = 0; c < 4; c++)
   {
      for (r = 0; r < 4; r++)
      {
         float mat1_val = mat1->mat[r][c];
         float mat2_val = mat2->mat[c][r];

         // fprintf(stdout, "%1d%1d - mat1_val = %f mat2_val = %f\n", c, r, mat1_val, mat2_val);

         if (!(fabs(mat1_val - mat2_val) < 0.001))
         {
            are_transpose = false;
         }
      }
   }

   // fprintf(stdout, "Are transpose: %1d\n", (int)are_transpose);

   return are_transpose;
}

BOOST_AUTO_TEST_CASE(initialization_tests, *boost::unit_test::tolerance(0.0001))
{
   int sizeof_mat_2d = sizeof(fmatrix_2x2);
   int sizeof_mat_3d = sizeof(fmatrix_3x3);
   int sizeof_mat_4d = sizeof(fmatrix_4x4);

   int sizeof_float  = sizeof(float);

   BOOST_TEST( sizeof_mat_2d ==  4*sizeof_float );
   BOOST_TEST( sizeof_mat_3d ==  9*sizeof_float );
   BOOST_TEST( sizeof_mat_4d == 16*sizeof_float );

   fmatrix_2x2 mat_2d;
   fmatrix_3x3 mat_3d;
   fmatrix_4x4 mat_4d;

   fmatrix_2x2_init(&mat_2d);
   fmatrix_3x3_init(&mat_3d);
   fmatrix_4x4_init(&mat_4d);

   BOOST_TEST( fmatrix_2x2_is_identity(&mat_2d) );
   BOOST_TEST( fmatrix_3x3_is_identity(&mat_3d) );
   BOOST_TEST( fmatrix_4x4_is_identity(&mat_4d) );

   fmatrix_2x2_transpose(&mat_2d);
   fmatrix_3x3_transpose(&mat_3d);
   fmatrix_4x4_transpose(&mat_4d);

   BOOST_TEST( fmatrix_2x2_is_identity(&mat_2d) );
   BOOST_TEST( fmatrix_3x3_is_identity(&mat_3d) );
   BOOST_TEST( fmatrix_4x4_is_identity(&mat_4d) );
}

BOOST_AUTO_TEST_CASE(basic_operation_tests, *boost::unit_test::tolerance(0.0001))
{
   fmatrix_2x2 mat_2d_a, mat_2d_b, mat_2d_added, mat_2d_added_key, mat_2d_subbed, mat_2d_subbed_key;

   fmatrix_2x2_init(&mat_2d_a);
   fmatrix_2x2_init(&mat_2d_b);

   // Both identity
   BOOST_TEST( fmatrix_2x2_are_equal(&mat_2d_a, &mat_2d_b) );

   const float mat_2d_equality_buf[2][2] = { {1.0f, 2.0f}, 
                                             {3.0f, 4.0f} };

   fmatrix_2x2_set(&mat_2d_a, mat_2d_equality_buf);
   fmatrix_2x2_set(&mat_2d_b, mat_2d_equality_buf);

   // Both equality
   BOOST_TEST( fmatrix_2x2_are_equal(&mat_2d_a, &mat_2d_b) );

   // Addition and Subtraction (inadvertently, scaling)
   const float mat_2d_a_buf[2][2] = { {1.0f, 2.0f}, 
                                      {3.0f, 4.0f} };

   const float mat_2d_b_buf[2][2] = { {1.5f,  18.0f}, 
                                      {6.2f, -37.2f} };

   const float mat_2d_added_key_buf[2][2] = { {2.5f,  20.0f}, 
                                              {9.2f, -33.2f} };

   const float mat_2d_subbed_key_buf[2][2] = { {-0.5f, -16.0f}, 
                                               {-3.2f,  41.2f} };

   fmatrix_2x2_set(&mat_2d_a, mat_2d_a_buf);
   fmatrix_2x2_set(&mat_2d_b, mat_2d_b_buf);
   fmatrix_2x2_set(&mat_2d_added_key, mat_2d_added_key_buf);
   fmatrix_2x2_set(&mat_2d_subbed_key, mat_2d_subbed_key_buf);

   mat_2d_added  = fmatrix_2x2_add(&mat_2d_a, &mat_2d_b);
   mat_2d_subbed = fmatrix_2x2_subtract(&mat_2d_a, &mat_2d_b);

   BOOST_TEST( fmatrix_2x2_are_equal(&mat_2d_added, &mat_2d_added_key) );
   BOOST_TEST( fmatrix_2x2_are_equal(&mat_2d_subbed, &mat_2d_subbed_key) );
}

BOOST_AUTO_TEST_CASE(multiplication_tests, *boost::unit_test::tolerance(0.0001))
{
   // fmatrix_2x2
   {
      fmatrix_2x2 mat_2d_a, mat_2d_b, mat_2d_axb_mult, mat_2d_axb_mult_key, mat_2d_bxa_mult, mat_2d_bxa_mult_key;

      fmatrix_2x2_init(&mat_2d_a);
      fmatrix_2x2_init(&mat_2d_b);

      const float mat_2d_a_buf[2][2] = { {1.0f, 7.0f}, 
                                         {2.0f, 4.0f} };

      const float mat_2d_b_buf[2][2] = { {3.0f, 3.0f}, 
                                         {5.0f, 2.0f} };

      const float mat_2d_axb_mult_key_buf[2][2] = { {38.0f, 17.0f}, 
                                                    {26.0f, 14.0f} };

      const float mat_2d_bxa_mult_key_buf[2][2] = { {9.0f, 33.0f}, 
                                                    {9.0f, 43.0f} };

      fmatrix_2x2_set(&mat_2d_a, mat_2d_a_buf);
      fmatrix_2x2_set(&mat_2d_b, mat_2d_b_buf);
      fmatrix_2x2_set(&mat_2d_axb_mult_key, mat_2d_axb_mult_key_buf);
      fmatrix_2x2_set(&mat_2d_bxa_mult_key, mat_2d_bxa_mult_key_buf);

      mat_2d_axb_mult = fmatrix_2x2_multiply(&mat_2d_a, &mat_2d_b);
      mat_2d_bxa_mult = fmatrix_2x2_multiply(&mat_2d_b, &mat_2d_a);

      BOOST_TEST( fmatrix_2x2_are_equal(&mat_2d_axb_mult, &mat_2d_axb_mult_key) );
      BOOST_TEST( fmatrix_2x2_are_equal(&mat_2d_bxa_mult, &mat_2d_bxa_mult_key) );
   }

   // fmatrix_3x3
   {
      fmatrix_3x3 mat_3d_a, mat_3d_b, mat_3d_axb_mult, mat_3d_axb_mult_key, mat_3d_bxa_mult, mat_3d_bxa_mult_key;

      fmatrix_3x3_init(&mat_3d_a);
      fmatrix_3x3_init(&mat_3d_b);

      const float mat_3d_a_buf[3][3] = { {5.0f, 6.0f, 6.0}, 
                                         {1.0f, 2.0f, 3.0},
                                         {9.0f, 9.0f, 9.0} };

      const float mat_3d_b_buf[3][3] = { { 1.0f,  1.0f,  2.0}, 
                                         { 3.0f,  5.0f,  8.0},
                                         {13.0f, 21.0f, 34.0} };

      const float mat_3d_axb_mult_key_buf[3][3] = { {101.0f, 161.0f, 262.0},
                                                    { 46.0f,  74.0f, 120.0},
                                                    {153.0f, 243.0f, 396.0} };

      const float mat_3d_bxa_mult_key_buf[3][3] = { { 24.0f,  26.0f,  27.0}, 
                                                    { 92.0f, 100.0f, 105.0},
                                                    {392.0f, 426.0f, 447.0} };

      fmatrix_3x3_set(&mat_3d_a, mat_3d_a_buf);
      fmatrix_3x3_set(&mat_3d_b, mat_3d_b_buf);

      fmatrix_3x3_set(&mat_3d_axb_mult_key, mat_3d_axb_mult_key_buf);
      fmatrix_3x3_set(&mat_3d_bxa_mult_key, mat_3d_bxa_mult_key_buf);

      mat_3d_axb_mult = fmatrix_3x3_multiply(&mat_3d_a, &mat_3d_b);
      mat_3d_bxa_mult = fmatrix_3x3_multiply(&mat_3d_b, &mat_3d_a);

      BOOST_TEST( fmatrix_3x3_are_equal(&mat_3d_axb_mult, &mat_3d_axb_mult_key) );
      BOOST_TEST( fmatrix_3x3_are_equal(&mat_3d_bxa_mult, &mat_3d_bxa_mult_key) );
   }

   // fmatrix_4x4
   {
      fmatrix_4x4 mat_4d_a, mat_4d_b, mat_4d_axb_mult, mat_4d_axb_mult_key, mat_4d_bxa_mult, mat_4d_bxa_mult_key;

      fmatrix_4x4_init(&mat_4d_a);
      fmatrix_4x4_init(&mat_4d_b);

      const float mat_4d_a_buf[4][4] = { {5.0f, 6.0f, 6.0}, 
                                         {1.0f, 2.0f, 3.0},
                                         {9.0f, 9.0f, 9.0} };

      const float mat_4d_b_buf[4][4] = { { 1.0f,  1.0f,  2.0}, 
                                         { 3.0f,  5.0f,  8.0},
                                         {13.0f, 21.0f, 34.0} };

      const float mat_4d_axb_mult_key_buf[4][4] = { {101.0f, 161.0f, 262.0},
                                                    { 46.0f,  74.0f, 120.0},
                                                    {153.0f, 243.0f, 396.0} };

      const float mat_4d_bxa_mult_key_buf[4][4] = { { 24.0f,  26.0f,  27.0}, 
                                                    { 92.0f, 100.0f, 105.0},
                                                    {392.0f, 426.0f, 447.0} };

      fmatrix_4x4_set(&mat_4d_a, mat_4d_a_buf);
      fmatrix_4x4_set(&mat_4d_b, mat_4d_b_buf);

      fmatrix_4x4_set(&mat_4d_axb_mult_key, mat_4d_axb_mult_key_buf);
      fmatrix_4x4_set(&mat_4d_bxa_mult_key, mat_4d_bxa_mult_key_buf);

      mat_4d_axb_mult = fmatrix_4x4_multiply(&mat_4d_a, &mat_4d_b);
      mat_4d_bxa_mult = fmatrix_4x4_multiply(&mat_4d_b, &mat_4d_a);

      BOOST_TEST( fmatrix_4x4_are_equal(&mat_4d_axb_mult, &mat_4d_axb_mult_key) );
      BOOST_TEST( fmatrix_4x4_are_equal(&mat_4d_bxa_mult, &mat_4d_bxa_mult_key) );
   }

   {
      fmatrix_2x2 mat_2d, mat_2d_transposed, mat_2d_transposed_key;
      fmatrix_3x3 mat_3d, mat_3d_transposed, mat_3d_transposed_key;
      fmatrix_4x4 mat_4d, mat_4d_transposed, mat_4d_transposed_key;

      const float mat_2d_data[2][2] = {{ 1.0f, 2.0f }, 
                                       { 3.0f, 4.0f }};

      const float mat_3d_data[3][3] = {{ 1.0f, 2.0f, 3.0f }, 
                                       { 4.0f, 5.0f, 6.0f },
                                       { 7.0f, 8.0f, 9.0f }};

      const float mat_4d_data[4][4] = {{  1.0f,  2.0f,  3.0f,  4.0f }, 
                                       {  5.0f,  6.0f,  7.0f,  8.0f },
                                       {  9.0f, 10.0f, 11.0f, 12.0f },
                                       { 13.0f, 14.0f, 15.0f, 16.0f }};

      const float mat_2d_transposed_data[2][2] = {{ 1.0f, 3.0f }, 
                                                  { 2.0f, 4.0f }};

      const float mat_3d_transposed_data[3][3] = {{ 1.0f, 4.0f, 7.0f }, 
                                                  { 2.0f, 5.0f, 8.0f },
                                                  { 3.0f, 6.0f, 9.0f }};

      const float mat_4d_transposed_data[4][4] = {{ 1.0f, 5.0f,  9.0f, 13.0f }, 
                                                  { 2.0f, 6.0f, 10.0f, 14.0f },
                                                  { 3.0f, 7.0f, 11.0f, 15.0f },
                                                  { 4.0f, 8.0f, 12.0f, 16.0f }};

      fmatrix_2x2_set(&mat_2d, mat_2d_data);
      fmatrix_3x3_set(&mat_3d, mat_3d_data);
      fmatrix_4x4_set(&mat_4d, mat_4d_data);

      fmatrix_2x2_set(&mat_2d_transposed_key, mat_2d_transposed_data);
      fmatrix_3x3_set(&mat_3d_transposed_key, mat_3d_transposed_data);
      fmatrix_4x4_set(&mat_4d_transposed_key, mat_4d_transposed_data);

      fmatrix_2x2_set(&mat_2d_transposed, mat_2d_data);
      fmatrix_3x3_set(&mat_3d_transposed, mat_3d_data);
      fmatrix_4x4_set(&mat_4d_transposed, mat_4d_data);

      fmatrix_2x2_transpose(&mat_2d_transposed);
      fmatrix_3x3_transpose(&mat_3d_transposed);
      fmatrix_4x4_transpose(&mat_4d_transposed);

      BOOST_TEST( fmatrix_2x2_are_equal(&mat_2d_transposed, &mat_2d_transposed_key) );
      BOOST_TEST( fmatrix_3x3_are_equal(&mat_3d_transposed, &mat_3d_transposed_key) );
      BOOST_TEST( fmatrix_4x4_are_equal(&mat_4d_transposed, &mat_4d_transposed_key) );

      BOOST_TEST( are_matrices_transpose(&mat_4d, &mat_4d_transposed) );
   }
}

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static bool are_matrices_equal(glm::mat4 glm_matrix, const fmatrix_4x4* our_matrix)
{
   bool are_equal = true;

   int c, r;
   for (c = 0; c < 4; c++)
   {
      for (r = 0; r < 4; r++)
      {
         float glm_val = glm_matrix[c][r];
         float our_val = our_matrix->mat[c][r];

         // fprintf(stdout, "%1d%1d - glm_val = %f our_val = %f\n", c, r, glm_val, our_val);

         if (!(fabs(our_val - glm_val) < 0.001))
         {
            are_equal = false;
         }
      }
   }

   // fprintf(stdout, "Are equal: %1d\n", (int)are_equal);

   return are_equal;
}

BOOST_AUTO_TEST_CASE(fmatrix_glm_tests, *boost::unit_test::tolerance(0.0001))
{
   {
      struct camera_ortho_t camera;

      const fvector3 camera_position = (fvector3) { 0.0f, 0.0f, -10.0f };
      const fvector3 camera_up       = (fvector3) { 0.0f, 1.0f,   0.0f };
      const fvector3 camera_front    = (fvector3) { 0.0f, 0.0f,   1.0f };

      const glm::vec3 glm_camera_position = glm::vec3(0.0f, 0.0f, -10.0f);
      const glm::vec3 glm_camera_up       = glm::vec3(0.0f, 1.0f,  0.0f);
      const glm::vec3 glm_camera_front    = glm::vec3(0.0f, 0.0f,  1.0f);

      float left       = 0.0f;
      float right      = 800.0f;
      float top        = 600.0f;
      float bottom     = 0.0f;
      float near_plane = 0.0f;
      float far_plane  = 100.0f;

      camera_init_orthographic(&(camera), camera_position, camera_up, camera_front);
      camera_set_projection_orthographic(&(camera), left, right, top, bottom, near_plane, far_plane);

      // Orthographic Projection Check

      glm::mat4 glm_ortho_mat = glm::ortho(left, right, bottom, top, near_plane, far_plane);

      fmatrix_4x4* our_projection_matrix = &(camera.base.projection_matrix);

      BOOST_TEST( are_matrices_equal(glm_ortho_mat, our_projection_matrix) );

      // Look At Matrix Check

      glm::mat4 glm_look_at_mat = glm::lookAt(glm_camera_position, (glm_camera_position + glm_camera_front), glm_camera_up);

      fmatrix_4x4* our_lookat_matrix = &(camera.base.view_matrix);

      BOOST_TEST( are_matrices_equal(glm_look_at_mat, our_lookat_matrix) );

      // View Projection Check

      glm::mat4 glm_view_projection_mat = glm_ortho_mat * glm_look_at_mat;

      // fmatrix_4x4 our_view_projection_mat = fmatrix_4x4_multiply(our_lookat_matrix, our_projection_matrix);

      fmatrix_4x4* our_view_projection_mat = &(camera.base.view_projection_matrix);

      BOOST_TEST( are_matrices_equal(glm_view_projection_mat, our_view_projection_mat) );
   }

   {
      const float scale_factor = 50.0f;
      const fvector3 scale_factors      = (fvector3) { {scale_factor, scale_factor, scale_factor} };
      const fvector3 translation_coords = (fvector3) { {400.0f, 200.0f, 0.0f} };

      // GLM
      glm::vec3 glm_translation_coords = glm::vec3(translation_coords.comp.x, translation_coords.comp.y, translation_coords.comp.z);
      glm::vec3 glm_scale_factors      = glm::vec3(scale_factor, scale_factor, scale_factor);

      glm::mat4 glm_identity_matrix(1.0f);
      glm::mat4 glm_scale_matrix(1.0f);
      glm::mat4 glm_translation_matrix(1.0f);
      glm::mat4 glm_identity_scale_translation_matrix(1.0f);

      glm_scale_matrix = glm::scale(glm_identity_matrix, glm_scale_factors);
      glm_translation_matrix = glm::translate(glm_identity_matrix, glm_translation_coords);

      glm_identity_scale_translation_matrix = glm_translation_matrix * glm_scale_matrix * glm_identity_matrix;

      // Ours
      fmatrix_4x4 transform_matrix;
      fmatrix_4x4_init(&transform_matrix);

      BOOST_TEST( are_matrices_equal(glm_identity_matrix, &transform_matrix) );

      transform_matrix = fmatrix_4x4_transform_scale(&transform_matrix, scale_factors);

      BOOST_TEST( are_matrices_equal(glm_scale_matrix, &transform_matrix) );

      transform_matrix = fmatrix_4x4_transform_translate(&transform_matrix, translation_coords);

      BOOST_TEST( are_matrices_equal(glm_identity_scale_translation_matrix, &transform_matrix) );
   }
}

BOOST_AUTO_TEST_CASE(TautologicalTests, *boost::unit_test::tolerance(0.0001))
{
   BOOST_TEST( 0 == 0 );
   BOOST_TEST( 0.0f == 0.0f );
}