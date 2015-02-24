#include <tuple>


// the generic signature falls back on the signature of the call operator if present
template < class C >
struct signature : signature< decltype( &C::operator() ) >  {};


// pointer to member function (incl. const & volatile ) fall back on the plain function signatures
template < class C , typename Result , typename... Args >
struct signature< Result (C::*)(Args...) > : signature< Result ( Args... ) > {};

template < class C , typename Result , typename... Args >
struct signature< Result (C::*)(Args...) const > : signature< Result ( Args... ) > {};

template < class C , typename Result , typename... Args >
struct signature< Result (C::*)(Args...) volatile > : signature< Result ( Args... ) > {};

template < class C , typename Result , typename... Args >
struct signature< Result (C::*)(Args...) const volatile > : signature< Result ( Args... ) > {};


// pointer and references to free function fall back on the plain function signatures
template < typename Result , typename... Args >
struct signature< Result (*)(Args...) > : signature< Result ( Args... ) > {};

template < typename Result , typename... Args >
struct signature< Result (&)(Args...) > : signature< Result ( Args... ) > {};


// actual implementation just for pure function signature types
template < typename Result , typename... Args >
struct signature< Result ( Args... ) >
{
   static constexpr auto num_args = sizeof...(Args);

   template< size_t n >
   using  argument = typename std::tuple_element< n, std::tuple<Args...> >;
   using  result_type = Result;
};



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

}


