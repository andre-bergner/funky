//   -----------------------------------------------------------------------------------------------
//    Copyright 2015 André Bergner. Distributed under the Apache Software License, Version 2.0.
//     (See accompanying file LICENSE or copy at http://www.apache.org/licenses/LICENSE-2.0)
//      --------------------------------------------------------------------------------------------

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
