#pragma once

#ifdef MATCHER_SELECTA
#  include "detail/matcher_impl_selecta.hpp"
#else
#  include "detail/matcher_impl.hpp"
#endif
#include "constexpr_sort.h"
#include "detail/linear_matcher_impl.hpp"


//  ------------------------------------------------------------------------------------------------
// match/case building blocks
//  ------------------------------------------------------------------------------------------------

template <typename Value, typename Lambda>
struct match_pair;


// alternative names
// • token     / tok
// • pattern   / pat
// • value     / val
// • condition / cond

// runtime (non-constexpr) values

template <typename Value>
struct case_t
{
   const Value value;
};

template <typename Value>
auto case_(Value&& x) -> case_t<Value>
{ return {std::forward<Value>(x)}; }


// compiletime (constexpr) values

template <auto N>
using val_t = std::integral_constant<decltype(N),N>;

template <auto N>
inline constexpr auto val = val_t<N>{};




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


template <auto Value, typename Lambda>
auto operator >>= (val_t<Value>, Lambda&& lambda)
{
   //return { std::move(c.value), std::forward<Lambda>(lambda) };
   return [lambda](val_t<Value>) -> decltype(auto) { return lambda(); };
}



//  ------------------------------------------------------------------------------------------------
// matcher 
//  ------------------------------------------------------------------------------------------------


//  ------------------------------------------------------------------------------------------------
// traits 
//  ------------------------------------------------------------------------------------------------

namespace detail
{
   template <typename T>
   struct void_ { using type = void; };

   template <typename T>
   using void_t = typename void_<T>::type;


   template <typename Type, typename enabler = void>
   struct has_static_constexpr_value : std::false_type {};

   template <typename Type>
   struct has_static_constexpr_value<Type, void_t<decltype(Type::value)>> : std::true_type {};

   template <typename Type>
   constexpr bool has_static_constexpr_value_v = has_static_constexpr_value<Type>::value;



   template <typename MatchCase, typename enabler = void>
   struct is_constexpr_case : std::false_type {};

   template <typename Lambda>
   struct is_constexpr_case<Lambda> : has_static_constexpr_value<first_arg_t<Lambda> > {};

   template <typename Value, typename Lambda>
   struct is_constexpr_case< match_pair<Value,Lambda> > : std::false_type {};

   template <typename MatchCase>
   constexpr bool is_constexpr_case_v = is_constexpr_case<std::decay_t<MatchCase>>::value;

}


//  ------------------------------------------------------------------------------------------------
// matcher for constexpr values
//  ------------------------------------------------------------------------------------------------

template <typename... Lambdas>
constexpr auto make_matcher(Lambdas&&... lambdas)
{
   using namespace detail;

   using sorted_lambda_idx_t = constexpr_sort_index_t<size_t, detail::first_arg_t<Lambdas>::value... >;

   return detail::dispatch_sorted( std::forward_as_tuple(std::forward<Lambdas>(lambdas)...), sorted_lambda_idx_t{});
}

//  ------------------------------------------------------------------------------------------------
// matcher for non-constexpr values
//  ------------------------------------------------------------------------------------------------

template <typename... Cases>
constexpr auto make_matcher2(Cases&&... cases)
{
   using result_t = typename detail::result<decltype(cases.lambda)...>::type;

   return [=](auto&& x) -> detail::opt_result_t<result_t>
   {
      return detail::match_linear<result_t>( x, cases... );
   };
}


//  ------------------------------------------------------------------------------------------------
// matcher -- main entry point with dispatch
//  ------------------------------------------------------------------------------------------------

namespace detail
{
   template <typename X, typename... Cases>
   auto dispatch_match(std::true_type, X&& x, Cases&&... cases)      // constexpr-case → binary search
   {
      return make_matcher(std::forward<Cases>(cases)...)(std::forward<X>(x));
   }

   template <typename X, typename... Cases>
   auto dispatch_match(std::false_type, X&& x, Cases&&... cases)     // non-constexpr-case → linear search
   {
      using result_t = typename detail::result<decltype(cases.lambda)...>::type;
      return detail::match_linear<result_t>( x, std::forward<Cases>(cases)... );
   }
}

template <typename Value>
constexpr auto match(Value&& x)
{
   return [x](auto&&... cases)
   {
      using namespace detail;
      constexpr bool all_cases_are_constexpr = (is_constexpr_case_v<decltype(cases)> && ...);
      using all_cases_are_constexpr_t = std::integral_constant<bool, all_cases_are_constexpr>;
      return dispatch_match( all_cases_are_constexpr_t{}, x, std::forward<decltype(cases)>(cases)... );
   };
}



namespace detail
{
   size_t get_next_id()
   {
      static size_t id_counter = 0;
      return id_counter++;
   }

   template <typename T>
   struct type_id { static const size_t id; };

   template <typename T>
   const size_t type_id<T>::id = get_next_id();
}


template <typename Value>
constexpr auto match_type(Value&& x)
{
   return [x](auto&&... type_cases)
   {
      auto wrap = [](auto&& l)
      {
         return [l]( detail::type_id<detail::first_arg_t<decltype(l)>> ){
            return [l](auto&& x){      // <-- how to get x  back??
               return l(x);
            };
         };
      };

      return match(std::move(x))( wrap(std::forward<decltype(type_cases)>(type_cases))... );
   };
}
