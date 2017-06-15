//#include "signature.h"
#include "../funky/signature.h"

#include <optional>
#include <type_traits>
#include <tuple>
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





   template <typename ReturnType, typename Value>
   auto dispatch_impl(Value) -> opt_result_t<ReturnType>
   {
      return {};
   }


   template <typename ReturnType, typename Value, typename Lambda, typename... Lambdas>
   auto dispatch_impl(Value x, Lambda& l, Lambdas&... ls) -> opt_result_t<ReturnType>
   {
      using match_t = std::remove_reference_t<typename signature<Lambda>::template argument<0>::type>;

      if ( match_t::value == x)
         return invoker<ReturnType>::apply(l,match_t{});
      else
         return dispatch_impl<ReturnType>(x,ls...);
   }




   template <size_t Ofs, typename Seq>
   struct shift_seq;

   template <size_t Ofs, size_t... Ns>
   struct shift_seq<Ofs,std::index_sequence<Ns...>>
   {
      using type = std::index_sequence<(Ofs + Ns)...>;
   };

   template <size_t Ofs, typename Seq>
   using shift_seq_t = typename shift_seq<Ofs,Seq>::type;



   template <typename Function>
   using match_on_t = std::remove_reference_t<typename signature<Function>::template argument<0>::type>;


   template <typename Tuple, size_t... Ns>
   auto tuple_indexed(Tuple&& t, std::index_sequence<Ns...>)
   {
      return std::tie( std::get<Ns>(t)... );
   }


   template <typename X, typename F>
   bool binary_dispatch(X x, std::tuple<F> const& t)
   {
      using match_t = match_on_t<F>;
      if (match_t::value == x)
         return std::get<0>(t)(match_t{}), true;
      return false;
   }

   template <typename X, typename... Fs>
   auto binary_dispatch(X x, std::tuple<Fs...> t)
   {
      constexpr size_t s = sizeof...(Fs);

      using match_t = match_on_t<std::tuple_element_t<s/2,std::tuple<Fs...>>>;
      if (x < match_t::value)
      {
         using left_idx_t = std::make_index_sequence<s/2>;
         return binary_dispatch(x, tuple_indexed(t, left_idx_t{}));
      }
      else
      {
         using right_idx_t = shift_seq_t<s/2,std::make_index_sequence<s/2+(s&1)>>;
         return binary_dispatch(x, tuple_indexed(t, right_idx_t{}));
      }
   }


}


template <typename... Lambdas>
auto dispatch(Lambdas&&... lambdas)
{
   using namespace detail;

   // TODO: sort lambdas

   using result_t = result_t<Lambdas...>;
   //return [=](auto x) -> opt_result_t<result_t>
   //{
   //   return dispatch_impl<result_t>(x, lambdas...);
   //};

   return [t = std::make_tuple(std::forward<Lambdas>(lambdas)...)](auto x) -> opt_result_t<result_t>
   {
      //return dispatch_impl<result_t>(x, lambdas...);
      return binary_dispatch(x, t);
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

