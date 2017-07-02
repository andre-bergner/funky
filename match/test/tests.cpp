#include "nano_tests.hpp"

#include "matcher.hpp"

template <int N>
using int_ = std::integral_constant<int,N>;


int main()
{
/*
   nano_tests::def("match on constexpr int using arrow notation", []
   {
      auto result = match1(2)
      (  val<1> >>= []{ return 1337; }
      ,  val<2> >>= []{ return 7357; }
      );

      ASSERT_FATAL( result );
      ASSERT( 7357 == *result );
   });


   nano_tests::def("match on constexpr int using lambda argument", []
   {
      auto result = match1(2)
      (  [] (val_t<1>) { return 1337; }
      ,  [] (val_t<2>) { return 7357; }
      );

      ASSERT_FATAL( result );
      ASSERT( 7357 == *result );
   });
*/

   nano_tests::def("match on case_: returns value of correct match", []
   {
      auto result = match2(1.1)
      (  case_(4.1) >>= []{ return 314; }
      ,  case_(6.1) >>= []{ return 47; }
      ,  case_(1.1) >>= []{ return 1337; }
      ,  case_(3.1) >>= []{ return 7357; }
      ,  case_(7.1) >>= []{ return -42; }
      );

      ASSERT_FATAL( result );
      ASSERT( 1337 == *result );
   });


   nano_tests::def("match on case_: returns nothing if no match found", []
   {
      auto result = match2(2.5)
      (  case_(1.) >>= []{ return 314; }
      ,  case_(2.) >>= []{ return 47; }
      );

      ASSERT( !result );
   });


   nano_tests::def("match function argument value", []
   {
      std::vector<int> const expected = {1,3,4,6,7};
      std::vector<int>       result;

      for (auto n : {1,2,3,4,5,6,7})
         match(n)
         (  [&](int_<4>){ result.push_back(4); }
         ,  [&](int_<6>){ result.push_back(6); }
         ,  [&](int_<1>){ result.push_back(1); }
         ,  [&](int_<3>){ result.push_back(3); }
         ,  [&](int_<7>){ result.push_back(7); }
         );

      ASSERT( expected == result );
   });


   nano_tests::run();
}

