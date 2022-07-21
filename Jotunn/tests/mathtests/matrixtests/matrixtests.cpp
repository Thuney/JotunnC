#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MatrixTests
#include <boost/test/unit_test.hpp>

extern "C" 
{
   #include "fmatrix.h"
}

#include <math.h>
#include <iostream>

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

   mat_2d_added  = fmatrix_2x2_add(mat_2d_a, mat_2d_b);
   mat_2d_subbed = fmatrix_2x2_subtract(mat_2d_a, mat_2d_b);

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

      mat_2d_axb_mult = fmatrix_2x2_multiply(mat_2d_a, mat_2d_b);
      mat_2d_bxa_mult = fmatrix_2x2_multiply(mat_2d_b, mat_2d_a);

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

      const float mat_2d_axb_mult_key_buf[3][3] = { {101.0f, 161.0f, 262.0},
                                                    { 46.0f,  74.0f, 120.0},
                                                    {153.0f, 243.0f, 396.0} };

      const float mat_2d_bxa_mult_key_buf[3][3] = { { 24.0f,  26.0f,  27.0}, 
                                                    { 92.0f, 100.0f, 105.0},
                                                    {392.0f, 426.0f, 447.0} };

      fmatrix_3x3_set(&mat_3d_a, mat_3d_a_buf);
      fmatrix_3x3_set(&mat_3d_b, mat_3d_b_buf);

      fmatrix_3x3_set(&mat_3d_axb_mult_key, mat_2d_axb_mult_key_buf);
      fmatrix_3x3_set(&mat_3d_bxa_mult_key, mat_2d_bxa_mult_key_buf);

      mat_3d_axb_mult = fmatrix_3x3_multiply(mat_3d_a, mat_3d_b);
      mat_3d_bxa_mult = fmatrix_3x3_multiply(mat_3d_b, mat_3d_a);

      BOOST_TEST( fmatrix_3x3_are_equal(&mat_3d_axb_mult, &mat_3d_axb_mult_key) );
      BOOST_TEST( fmatrix_3x3_are_equal(&mat_3d_bxa_mult, &mat_3d_bxa_mult_key) );
   }
}

BOOST_AUTO_TEST_CASE(TautologicalTests, *boost::unit_test::tolerance(0.0001))
{
   BOOST_TEST( 0 == 0 );
   BOOST_TEST( 0.0f == 0.0f );
}