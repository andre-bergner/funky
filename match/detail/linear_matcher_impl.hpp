#pragma once

#include "result.h"

#include <tuple>

namespace detail
{
   template <typename ReturnType, typename X>
   constexpr auto match_linear(X&&) -> opt_result_t<ReturnType>
   {
      return {};
   }

   template <typename ReturnType, typename X, typename Case, typename... Cases>
   constexpr auto match_linear(X const& x, Case&& c, Cases&&... cs) -> opt_result_t<ReturnType>
   {
      if (c.value == x)
         return invoker<ReturnType>::apply(c.lambda);
      else
         return match_linear<ReturnType>( x, std::forward<Cases>(cs)... );
   }
}
