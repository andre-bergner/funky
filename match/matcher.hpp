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






template <typename Value, typename Lambda>
struct match_pair;


// alternative names
// • token     / tok
// • pattern   / pat
// • value     / val
// • condition / cond

template <typename Value>
struct case_t
{
   const Value value;
};

template <typename Value>
auto case_(Value&& x) -> case_t<Value>
{ return {std::forward<Value>(x)}; }


template <typename Value, typename Lambda>
struct match_pair
{
   Value    value;
   Lambda   lambda;
};


template <typename Value, typename Lambda>
auto operator >= (case_t<Value>&& c, Lambda&& lambda) -> match_pair<Value,Lambda>
{
   return { std::move(c.value), std::forward<Lambda>(lambda) };
}




namespace detail
{
   template <typename ReturnType, typename X>
   constexpr auto linear_dispatch(X&&) -> opt_result_t<ReturnType>
   {
      return {};
   }

   template <typename ReturnType, typename X, typename Case, typename... Cases>
   constexpr auto linear_dispatch(X const& x, Case&& c, Cases&&... cs) -> opt_result_t<ReturnType>
   {
      if (c.value == x)
         return invoker<ReturnType>::apply(c.lambda);
      else
         return linear_dispatch<ReturnType>( x, std::forward<Cases>(cs)... );
   }
}




template <typename Value>
constexpr auto match2(Value&& x)
{
   return [x=std::forward<Value>(x)](auto&&... cases)
   {
      using result_t = typename detail::result<decltype(cases.lambda)...>::type;
      return detail::linear_dispatch<result_t>( x, std::forward<decltype(cases)>(cases)... );
   };
}

