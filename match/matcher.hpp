#pragma once

#ifdef MATCHER_SELECTA
#include "detail/matcher_impl_selecta.hpp"
#else
#include "detail/matcher_impl.hpp"
#endif
#include "constexpr_sort.h"


template <typename... Lambdas>
constexpr auto dispatch(Lambdas&&... lambdas)
{
   using namespace detail;

   using sorted_lambda_idx_t = constexpr_sort_index_t<size_t, detail::first_arg_t<Lambdas>::value... >;

   return detail::dispatch_sorted( std::forward_as_tuple(std::forward<Lambdas>(lambdas)...), sorted_lambda_idx_t{});
}



constexpr auto match(int x)
{
   return [x](auto&&... lambdas)
   {
      return dispatch(std::forward<decltype(lambdas)>(lambdas)...)(x);
   };
}
