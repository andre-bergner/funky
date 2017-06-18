#include "detail_result.h"

#include <tuple>
#include <iostream>


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
   auto tuple_indexed(Tuple&& t, std::index_sequence<Ns...>)
   {
      return std::tie( std::get<Ns>(t)... );
   }


   template <typename ReturnType, typename X, typename F>
   auto binary_dispatch(X x, std::tuple<F> const& t) -> opt_result_t<ReturnType>
   {
      using match_t = first_arg_t<F>;
      if (match_t::value == x)
         return std::get<0>(t)(match_t{}), true;
      return false;
   }

   template <typename ReturnType, typename X, typename... Fs>
   auto binary_dispatch(X x, std::tuple<Fs...> t) -> opt_result_t<ReturnType>
   {
      constexpr size_t s = sizeof...(Fs);

      using match_t = first_arg_t<std::tuple_element_t<s/2,std::tuple<Fs...>>>;
      if (x < match_t::value)
      {
         using left_idx_t = std::make_index_sequence<s/2>;
         return binary_dispatch<ReturnType>(x, tuple_indexed(t, left_idx_t{}));
      }
      else
      {
         using right_idx_t = shift_seq_t<s/2,std::make_index_sequence<s/2+(s&1)>>;
         return binary_dispatch<ReturnType>(x, tuple_indexed(t, right_idx_t{}));
      }
   }


}


template <typename... Lambdas>
auto dispatch(Lambdas&&... lambdas)
{
   using namespace detail;

   // TODO: sort lambdas

   using result_t = result_t<Lambdas...>;

   return [t = std::make_tuple(std::forward<Lambdas>(lambdas)...)](auto x) -> opt_result_t<result_t>
   {
      return binary_dispatch<result_t>(x, t);
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

