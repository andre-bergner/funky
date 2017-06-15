//#include "signature.h"
#include "../funky/signature.h"
#include "static_selecta.h"

#include <optional>
#include <type_traits>
#include <iostream>



template <int N>
using int_ = std::integral_constant<int,N>;


namespace detail
{

   // Some helper type trait

   template <typename Function, typename... Functions>
   struct result
   {
      using type = typename signature<Function>::result_type;

      template <typename...> struct type_list {};

      template <typename F>
      using as_type = std::decay_t<type>;

      using all_results = type_list<typename signature<Functions>::result_type...>;
      using expected_results = type_list<as_type<Functions>...>;

      static_assert( std::is_same< all_results, expected_results>::value
                   , "All functions must have the same result type.");
   };

   template <typename... Functions>
   using result_t = typename result<Functions...>::type;


   template <typename T>
   using opt_result_t = std::conditional_t<std::is_same<void,T>::value, bool, std::optional<T>>;


   template <typename ResultType>
   struct invoker
   {
      template <typename Function, typename Arg>
      static decltype(auto) apply(Function& f, Arg&& a)
      {
          return f(std::forward<Arg>(a));
      }
   };

   template <>
   struct invoker<void>
   {
      template <typename Function, typename Arg>
      static decltype(auto) apply(Function& f, Arg&& a)
      {
          return f(std::forward<Arg>(a)), true;
      }
   };


   template <typename Function>
   using first_arg_t = std::remove_reference_t<typename signature<Function>::template argument<0>::type>;


}



template <typename Lambda>
static constexpr auto make_action(Lambda&& l)
{
   using arg_t = detail::first_arg_t<Lambda>;
   return [l]() noexcept(noexcept(l(arg_t{}))) { l(arg_t{}); };
}


template <typename... Lambdas>
auto dispatch(Lambdas&&... lambdas)
{
   using namespace detail;

   // TODO: sort lambdas

   using result_t = result_t<Lambdas...>;
   using match_list_t = std::integer_sequence<int, detail::first_arg_t<Lambdas>::value... >;


   return [sel = make_static_selecta(make_action(std::forward<Lambdas>(lambdas))...)](auto x) -> opt_result_t<result_t>
   {

      constexpr int matches[] = { detail::first_arg_t<Lambdas>::value... };
      int i = 0;
      for (auto m : matches)
      {
         if (x == m)
            return sel(i),true;
         ++i;
      }
      return false;
   };
}



auto match(int x)
{
   return [x](auto&&... lambdas)
   {
      return dispatch(std::forward<decltype(lambdas)>(lambdas)...)(x);
   };
}




#include <iostream>

int main()
{
   using namespace std;

   for (auto n : {1,2,3,4,5,6,7})
      match(n)
      (  [](int_<1>){ cout << "1ne" << endl; }
      ,  [](int_<4>){ cout << "4our" << endl; }
      ,  [](int_<6>){ cout << "6ix" << endl; }
      ,  [](int_<7>){ cout << "7even" << endl; }
      );

/*
      match(n)
      (  [](int_<1337>)             { … }
      ,  [](any_of_int<4,2,7,1338>) { … }
      ,  [](range_<13,37>)          { … }
      ,  [](int_<7>)                { … }
      );
*/

}

