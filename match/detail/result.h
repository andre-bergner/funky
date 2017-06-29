#pragma once

#include "../../funky/signature.h"

#include <type_traits>
#include <optional>


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
      template <typename Function, typename... Args>
      static decltype(auto) apply(Function&& f, Args&&... args)
      {
          return std::forward<Function>(f)( std::forward<Args>(args)... );
      }
   };

   template <>
   struct invoker<void>
   {
      template <typename Function, typename... Args>
      static decltype(auto) apply(Function&& f, Args&&... args)
      {
          return std::forward<Function>(f)( std::forward<Args>(args)... ), true;
      }
   };



   template <typename Function>
   using first_arg_t = std::remove_reference_t<typename signature<Function>::template argument<0>::type>;

}
