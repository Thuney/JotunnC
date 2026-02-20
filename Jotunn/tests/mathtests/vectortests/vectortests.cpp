#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE vector_tests
#include <boost/test/unit_test.hpp>

extern "C" 
{
   #include "fvector.h"

   #include "stdio.h"
}

#include <math.h>

BOOST_AUTO_TEST_CASE(initialization_tests, *boost::unit_test::tolerance(0.0001))
{
   fvector2 fvec2;
   fvector3 fvec3;
   fvector4 fvec4;

   fvector2_init(&fvec2);
   fvector3_init(&fvec3);
   fvector4_init(&fvec4);

   BOOST_TEST( fvec2.comp.x == 0.0f );
   BOOST_TEST( fvec2.comp.y == 0.0f );

   BOOST_TEST( fvec3.comp.x == 0.0f );
   BOOST_TEST( fvec3.comp.y == 0.0f );
   BOOST_TEST( fvec3.comp.z == 0.0f );

   BOOST_TEST( fvec4.comp.w == 0.0f );
   BOOST_TEST( fvec4.comp.x == 0.0f );
   BOOST_TEST( fvec4.comp.y == 0.0f );
   BOOST_TEST( fvec4.comp.z == 0.0f );

   fvector2_set(&fvec2, -1.0f, 1.0f);
   fvector3_set(&fvec3, -1.0f, 1.0f, -1.0f);
   fvector4_set(&fvec4, -1.0f, 1.0f, -1.0f, 1.0f);

   BOOST_TEST( fvec2.comp.x == -1.0f );
   BOOST_TEST( fvec2.comp.y ==  1.0f );

   BOOST_TEST( fvec3.comp.x == -1.0f );
   BOOST_TEST( fvec3.comp.y ==  1.0f );
   BOOST_TEST( fvec3.comp.z == -1.0f );

   BOOST_TEST( fvec4.comp.w == -1.0f );
   BOOST_TEST( fvec4.comp.x ==  1.0f );
   BOOST_TEST( fvec4.comp.y == -1.0f );
   BOOST_TEST( fvec4.comp.z ==  1.0f );
}

BOOST_AUTO_TEST_CASE(TautologicalTests, *boost::unit_test::tolerance(0.0001))
{
   BOOST_TEST( 0 == 0 );
   BOOST_TEST( 0.0f == 0.0f );
}