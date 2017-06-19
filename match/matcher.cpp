#include "detail_result.h"
#include "constexpr_sort.h"

#include <tuple>


template <int N>
using int_ = std::integral_constant<int,N>;


namespace detail
{
   template <size_t Ofs, typename Seq>
   struct shift_seq;

   template <size_t Ofs, size_t... Ns>
   struct shift_seq<Ofs,std::index_sequence<Ns...>>
   {
      using type = std::index_sequence<(Ofs + Ns)...>;
   };

   template <size_t Ofs, typename Seq>
   using shift_seq_t = typename shift_seq<Ofs,Seq>::type;



   template <typename Tuple, size_t... Ns>
   constexpr auto tie_indexed(Tuple&& t, std::index_sequence<Ns...>)
   {
      return std::tie( std::get<Ns>(t)... );
   }


   template <typename ReturnType, typename X, typename F>
   constexpr auto binary_dispatch(X x, std::tuple<F> const& t) -> opt_result_t<ReturnType>
   {
      using match_t = first_arg_t<F>;
      if (match_t::value == x)
         return std::get<0>(t)(match_t{}), true;
      return false;
   }

   template <typename ReturnType, typename X, typename... Fs>
   constexpr auto binary_dispatch(X x, std::tuple<Fs...> t) -> opt_result_t<ReturnType>
   {
      constexpr size_t s = sizeof...(Fs);

      using match_t = first_arg_t<std::tuple_element_t<s/2,std::tuple<Fs...>>>;
      if (x < match_t::value)
      {
         using left_idx_t = std::make_index_sequence<s/2>;
         return binary_dispatch<ReturnType>(x, tie_indexed(t, left_idx_t{}));
      }
      else
      {
         using right_idx_t = shift_seq_t<s/2,std::make_index_sequence<s/2+(s&1)>>;
         return binary_dispatch<ReturnType>(x, tie_indexed(t, right_idx_t{}));
      }
   }


}


template <typename... Lambdas, size_t... Ns>
constexpr auto dispatch_sorted(std::tuple<Lambdas...> lambdas, std::index_sequence<Ns...> )
{
   using namespace detail;

   using result_t = result_t<Lambdas...>;

   return [t = std::make_tuple(std::get<Ns>(lambdas)...)](auto x) -> opt_result_t<result_t>
   {
      return binary_dispatch<result_t>(x, t);
   };
}


template <typename... Lambdas>
constexpr auto dispatch(Lambdas&&... lambdas)
{
   using namespace detail;

   using sorted_lambda_idx_t = constexpr_sort_index_t<size_t, detail::first_arg_t<Lambdas>::value... >;

   return dispatch_sorted( std::forward_as_tuple(std::forward<Lambdas>(lambdas)...), sorted_lambda_idx_t{});
}



constexpr auto match(int x)
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

#if 1
   for (auto n : {1,2,3,4,5,6,7})
      match(n)
      (  [&](int_<4>){ cout << "4our" << endl; }
      ,  [&](int_<6>){ cout << "6ix"  << endl; }
      ,  [&](int_<1>){ cout << "1ne"  << endl; }
      ,  [&](int_<3>){ cout << "3ree"  << endl; }
      ,  [&](int_<7>){ cout << "7even" << endl; }
      );
#else
   volatile int k;
   volatile int n = 7;
      match(n)
      (  [&](int_<4>){ k = 3; }
      ,  [&](int_<6>){ k = 5; }
      ,  [&](int_<1>){ k = 1337; }
      ,  [&](int_<7>){ k = -2; }
      );
#endif

/*
      match(n)
      (  [](int_<1337>)             { … }
      ,  [](any_of_int<4,2,7,1338>) { … }
      ,  [](range_<13,37>)          { … }
      ,  [](int_<7>)                { … }
      );
*/
}

