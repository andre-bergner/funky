#pragma once

#ifdef MATCHER_SELECTA
#  include "detail/matcher_impl_selecta.hpp"
#else
#  include "detail/matcher_impl.hpp"
#endif
#include "constexpr_sort.h"
#include "detail/linear_matcher_impl.hpp"


template <typename... Lambdas>
constexpr auto make_matcher(Lambdas&&... lambdas)
{
   using namespace detail;

   using sorted_lambda_idx_t = constexpr_sort_index_t<size_t, detail::first_arg_t<Lambdas>::value... >;

   return detail::dispatch_sorted( std::forward_as_tuple(std::forward<Lambdas>(lambdas)...), sorted_lambda_idx_t{});
}


template <typename Value>
constexpr auto match(Value&& x)
{
   return [x=std::forward<Value>(x)](auto&&... lambdas)
   {
      return make_matcher(std::forward<decltype(lambdas)>(lambdas)...)(x);
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
auto operator >>= (case_t<Value>&& c, Lambda&& lambda) -> match_pair<Value,Lambda>
{
   return { std::move(c.value), std::forward<Lambda>(lambda) };
}




template <typename Value>
constexpr auto match2(Value&& x)
{
   return [x=std::forward<Value>(x)](auto&&... cases)
   {
      using result_t = typename detail::result<decltype(cases.lambda)...>::type;
      return detail::match_linear<result_t>( x, std::forward<decltype(cases)>(cases)... );
   };
}



template <typename... Cases>
constexpr auto make_matcher2(Cases&&... cases)
{
   using result_t = typename detail::result<decltype(cases.lambda)...>::type;

   return [=](auto&& x) -> detail::opt_result_t<result_t>
   {
      return detail::match_linear<result_t>( x, cases... );
   };
}
