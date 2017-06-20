#pragma once

#include "detail/result.h"
#include "static_selecta.h"


namespace detail
{
   template <typename Value, int Size>
   struct constexpr_view
   {
      Value*  data;
      static constexpr size_t size = Size;

      constexpr_view(Value* p) : data{p} {}

      constexpr auto operator[](int n)       -> Value &       { return data[n]; }
      constexpr auto operator[](int n) const -> Value const&  { return data[n]; }
   };

#if 1

   template <typename ContainedValue, typename Value, typename Function>
   auto if_sorted_range_contains(ContainedValue x, constexpr_view<Value,1> xs, Function&& f, size_t i=0) -> decltype(f(x))
   {
      if (x == xs[0])
         return f(i);
      else
         return {};
   }

   template <typename ContainedValue, typename Value, int N, typename Function>
   auto if_sorted_range_contains(ContainedValue x, constexpr_view<Value,N> xs, Function&& f, size_t i=0) -> decltype(f(x))
   {
      if (x < xs[N/2])
         return if_sorted_range_contains(x, constexpr_view<Value,N/2>{xs.data}, std::forward<Function>(f),i);
      else
         return if_sorted_range_contains(x, constexpr_view<Value,N-N/2>{xs.data+N/2}, std::forward<Function>(f), i+N/2);
   }

#else

   template <typename ContainedValue, typename Value, int N, typename Function>
   auto if_sorted_range_contains(ContainedValue x, constexpr_view<Value,N> xs, Function&& f) -> decltype(f(x))
   {
      auto*  p = xs.data;
      for (auto n = N/2; n > 0; n /= 2)
      {
         if (x >= p[n])
            p += n;
      }

      if (x == *p)
         return f(p-xs.data);
      else
         return {};
   }

#endif

   template <typename ContainedValue, typename Value, size_t N, typename Function>
   auto if_range_contains(ContainedValue x, Value (&xs)[N], Function&& f) -> decltype(f(x))
   {
      int i = 0;
      for (auto m : xs)
      {
         if (x == m)
            return f(i);
         ++i;
      }
      return {};
   }


   template <typename... Lambdas, size_t... Ns>
   constexpr auto dispatch_sorted(std::tuple<Lambdas...> lambdas, std::index_sequence<Ns...> )
   {
      using namespace detail;

      using result_t = result_t<Lambdas...>;
      using match_list_t = std::integer_sequence<int, detail::first_arg_t<Lambdas>::value... >;

      auto make_action = [](auto&& l)
      {
         using arg_t = detail::first_arg_t<decltype(l)>;
         return [l]() noexcept(noexcept(l(arg_t{}))) { l(arg_t{}); };
      };


      return [sel = make_static_selecta(make_action(std::get<Ns>(lambdas))...)]
      (auto x) -> opt_result_t<result_t>
      {
         using tuple_t = std::tuple<Lambdas...>;
         constexpr int matches[] = { detail::first_arg_t<std::tuple_element_t<Ns,tuple_t>>::value... };

         return detail::if_sorted_range_contains(x, constexpr_view<int const,sizeof...(Lambdas)>{matches}, [&](auto&& v){
            return sel(std::forward<decltype(v)>(v)), true;
         });
      };
   }

}
