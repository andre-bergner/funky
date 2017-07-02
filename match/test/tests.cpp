#include "nano_tests.hpp"

#include "matcher.hpp"

template <int N>
using int_ = std::integral_constant<int,N>;


void test_tools()
{
   nano_tests::def("has_static_constexpr_value", []
   {
      using namespace detail;
      ASSERT(  has_static_constexpr_value_v< int_<1337> > );
      ASSERT( !has_static_constexpr_value_v< int > );
   });

   nano_tests::def("is_constexpr_case", []
   {
      using namespace detail;
      auto case1 = [](int_<1337>){};
      auto case2 = [](int){};
      auto case3 = case_(1337) >>= []{};
      ASSERT_TRUE(  is_constexpr_case_v< decltype(case1) > );
      ASSERT_FALSE( is_constexpr_case_v< decltype(case2) > );
      ASSERT_FALSE( is_constexpr_case_v< decltype(case3) > );
   });
}



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
      auto result = match(1.1)
      (  case_(4.1) >>= []{ return 314; }
      ,  case_(6.1) >>= []{ return 47; }
      ,  case_(1.1) >>= []{ return 1337; }
      ,  case_(3.1) >>= []{ return 7357; }
      ,  case_(7.1) >>= []{ return -42; }
      );

      ASSERT_FATAL( result );
      ASSERT( 1337 == *result );
   });

   nano_tests::def("matcher using case_: returns value of correct match", []
   {
      auto matcher = make_matcher2
      (  case_(4.1) >>= []{ return 314; }
      ,  case_(6.1) >>= []{ return 47; }
      ,  case_(1.1) >>= []{ return 1337; }
      ,  case_(3.1) >>= []{ return 7357; }
      ,  case_(7.1) >>= []{ return -42; }
      );

      auto result = matcher(1.1);
      ASSERT_FATAL( result );
      ASSERT( 1337 == *result );
   });


   nano_tests::def("match on case_: returns nothing if no match found", []
   {
      auto result = match(2.5)
      (  case_(1.) >>= []{ return 314; }
      ,  case_(2.) >>= []{ return 47; }
      );

      ASSERT( !result );
   });

   nano_tests::def("matcher using case_: returns nothing if no match found", []
   {
      auto matcher = make_matcher2
      (  case_(1.) >>= []{ return 314; }
      ,  case_(2.) >>= []{ return 47; }
      );

      ASSERT( !matcher(1.5) );
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

   test_tools();

   nano_tests::run();
}

