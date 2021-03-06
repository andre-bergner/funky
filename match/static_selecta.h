#include <type_traits>

template <typename... Lambdas>
struct overloaded : public Lambdas...
{
   using Lambdas::operator()...;

   overloaded(Lambdas&&... lambdas) : Lambdas(std::forward<Lambdas>(lambdas))... {}
};


template <typename... Lambdas>
auto overload(Lambdas&&... ls) -> overloaded<std::decay_t<Lambdas>...>
{
   return { std::forward<Lambdas>(ls)... };
};


template <typename ResultType, typename... Lambdas>
struct static_selecta
{
   static constexpr bool all_noexcept = (noexcept(std::declval<Lambdas>()()) && ...);

   using ov_t = overloaded<Lambdas...>;
   ov_t  ov;

   static_selecta(Lambdas&&... lambdas) : ov{std::forward<Lambdas>(lambdas)...} {}


   using fn_ptr_t = ResultType (*)(ov_t&) noexcept(all_noexcept);
   using cfn_ptr_t = ResultType (*)(ov_t const&) noexcept(all_noexcept);


   template <typename Lambda>
   static constexpr auto make_caller()
   {
      return [](ov_t& ov) noexcept(all_noexcept) { return static_cast<Lambda>(ov)(); };
   }

   template <typename Lambda>
   static constexpr auto make_const_caller()
   {
      return [](ov_t const& ov) noexcept(all_noexcept) { return static_cast<Lambda>(ov)(); };
   }


   constexpr static fn_ptr_t  fn[]  = { make_caller<Lambdas>()... };
   constexpr static cfn_ptr_t cfn[] = { make_const_caller<Lambdas>()... };


   constexpr ResultType operator()(size_t n)       noexcept(all_noexcept) { return fn[n](ov); }
   constexpr ResultType operator()(size_t n) const noexcept(all_noexcept) { return cfn[n](ov); }
};


template <typename ResultType, typename... Lambdas>
auto make_static_selecta(Lambdas&&... ls) -> static_selecta<ResultType, Lambdas...>
{
   return { std::forward<Lambdas>(ls)... };
}
