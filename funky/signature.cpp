#include "signature.h"


//   -----------------------------------------------------------------------------------------------
//  TEST
//   -----------------------------------------------------------------------------------------------

#include <cassert>
#include <type_traits>

struct R {};
struct A {};

R     R_int(A,int);
auto  auto_int_to_int(int) { return 42; }

int main()
{
   using namespace std;

   assert(( is_same< signature< decltype(R_int) >::result_type       , R >   ::value ));
   assert(( is_same< signature< decltype(R_int) >::argument<0>::type , A >   ::value ));
   assert(( is_same< signature< decltype(R_int) >::argument<1>::type , int > ::value ));

   assert(( is_same< signature< decltype(auto_int_to_int) >::result_type       , int > ::value ));
   assert(( is_same< signature< decltype(auto_int_to_int) >::argument<0>::type , int > ::value ));



   using ptr_void_int = void (*)(int);
   using ref_void_int = void (&)(int);
   assert(( is_same< signature< ptr_void_int >::argument<0>::type , int > ::value ));
   assert(( is_same< signature< ref_void_int >::argument<0>::type , int > ::value ));



   auto lamda_void_void = []{};
   assert(( is_same< signature< decltype(lamda_void_void) >::result_type , void > ::value ));



   auto lamda_int_float = [](float) mutable { return 3; };
   assert(( is_same< signature< decltype(lamda_int_float) >::result_type       , int >   ::value ));
   assert(( is_same< signature< decltype(lamda_int_float) >::argument<0>::type , float > ::value ));



   struct { int operator()() { return 0; } }                 funob;
   struct { int operator()() const { return 0; } }           funob_const;
   struct { int operator()() volatile { return 0; } }        funob_volatile;
   struct { int operator()() const volatile { return 0; } }  funob_const_volatile;

   assert(( is_same< signature< decltype(funob) >::result_type                , int > ::value ));
   assert(( is_same< signature< decltype(funob_const) >::result_type          , int > ::value ));
   assert(( is_same< signature< decltype(funob_volatile) >::result_type       , int > ::value ));
   assert(( is_same< signature< decltype(funob_const_volatile) >::result_type , int > ::value ));

   auto& r_funob                = funob;
   auto& r_funob_const          = funob_const;
   auto& r_funob_volatile       = funob_volatile;
   auto& r_funob_const_volatile = funob_const_volatile;

   assert(( is_same< signature< decltype(r_funob) >::result_type                , int > ::value ));
   assert(( is_same< signature< decltype(r_funob_const) >::result_type          , int > ::value ));
   assert(( is_same< signature< decltype(r_funob_volatile) >::result_type       , int > ::value ));
   assert(( is_same< signature< decltype(r_funob_const_volatile) >::result_type , int > ::value ));


   auto const& cr_funob                = funob;
   auto const& cr_funob_const          = funob_const;
   auto const& cr_funob_volatile       = funob_volatile;
   auto const& cr_funob_const_volatile = funob_const_volatile;

   assert(( is_same< signature< decltype(cr_funob) >::result_type                , int > ::value ));
   assert(( is_same< signature< decltype(cr_funob_const) >::result_type          , int > ::value ));
   assert(( is_same< signature< decltype(cr_funob_volatile) >::result_type       , int > ::value ));
   assert(( is_same< signature< decltype(cr_funob_const_volatile) >::result_type , int > ::value ));


   auto const volatile cv_funob                = funob;
   auto const volatile cv_funob_const          = funob_const;
   auto const volatile cv_funob_volatile       = funob_volatile;
   auto const volatile cv_funob_const_volatile = funob_const_volatile;

   assert(( is_same< signature< decltype(cv_funob) >::result_type                , int > ::value ));
   assert(( is_same< signature< decltype(cv_funob_const) >::result_type          , int > ::value ));
   assert(( is_same< signature< decltype(cv_funob_volatile) >::result_type       , int > ::value ));
   assert(( is_same< signature< decltype(cv_funob_const_volatile) >::result_type , int > ::value ));

   assert(( is_same< signature< decltype(std::move(funob)) >::result_type                , int > ::value ));
   assert(( is_same< signature< decltype(std::move(funob_const)) >::result_type          , int > ::value ));
   assert(( is_same< signature< decltype(std::move(funob_volatile)) >::result_type       , int > ::value ));
   assert(( is_same< signature< decltype(std::move(funob_const_volatile)) >::result_type , int > ::value ));

}


