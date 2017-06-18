#include "../funky/signature.h"
#include "static_selecta.h"

#include <optional>
#include <type_traits>
#include <iostream>



template <int N>
using int_ = std::integral_constant<int,N>;


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
      template <typename Function, typename Arg>
      static decltype(auto) apply(Function& f, Arg&& a)
      {
          return f(std::forward<Arg>(a));
      }
   };

   template <>
   struct invoker<void>
   {
      template <typename Function, typename Arg>
      static decltype(auto) apply(Function& f, Arg&& a)
      {
          return f(std::forward<Arg>(a)), true;
      }
   };


   template <typename Function>
   using first_arg_t = std::remove_reference_t<typename signature<Function>::template argument<0>::type>;


   template <typename Value, int Size>
   struct constexpr_view
   {
      Value*  data;
      static constexpr size_t size = Size;

      constexpr_view(Value* p) : data{p} {}

      constexpr auto operator[](int n)       -> Value &       { return data[n]; }
      constexpr auto operator[](int n) const -> Value const&  { return data[n]; }
   };

#if 0

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
         return if_sorted_range_contains(x, constexpr_view<Value,N/2>{xs.data}, std::forward<Function>(f));
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
}



template <typename... Lambdas>
constexpr auto dispatch(Lambdas&&... lambdas)
{
   using namespace detail;

   // TODO: sort lambdas

   using result_t = result_t<Lambdas...>;
   using match_list_t = std::integer_sequence<int, detail::first_arg_t<Lambdas>::value... >;

   auto make_action = [](auto&& l)
   {
      using arg_t = detail::first_arg_t<decltype(l)>;
      return [l]() noexcept(noexcept(l(arg_t{}))) { l(arg_t{}); };
   };


   return [sel = make_static_selecta(make_action(std::forward<Lambdas>(lambdas))...)](auto x) -> opt_result_t<result_t>
   {

      constexpr int matches[] = { detail::first_arg_t<Lambdas>::value... };

      return detail::if_sorted_range_contains(x, constexpr_view<int const,sizeof...(Lambdas)>{matches}, [&](auto&& v){
         return sel(std::forward<decltype(v)>(v)), true;
      });
   };
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

   for (auto n : {1,2,3,4,5,6,7})
      match(n)
      (  [](int_<1>){ cout << "1ne" << endl; }
      ,  [](int_<4>){ cout << "4our" << endl; }
      ,  [](int_<6>){ cout << "6ix" << endl; }
      ,  [](int_<7>){ cout << "7even" << endl; }
      );
}
