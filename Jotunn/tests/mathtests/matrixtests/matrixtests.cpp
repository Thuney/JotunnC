#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MatrixTests
#include <boost/test/unit_test.hpp>

extern "C" 
{
   #include "fmatrix.h"
}

#include <math.h>
#include <iostream>

BOOST_AUTO_TEST_CASE(ConstructorTests, *boost::unit_test::tolerance(0.0001))
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

BOOST_AUTO_TEST_CASE(TautologicalTests, *boost::unit_test::tolerance(0.0001))
{
   BOOST_TEST( 0 == 0 );
   BOOST_TEST( 0.0f == 0.0f );
}