#pragma once

#include "detail/result.h"

#include <tuple>

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

}
