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


template <typename... Lambdas>
struct static_selecta
{
   static constexpr bool all_noexcept = (noexcept(std::declval<Lambdas>()()) && ...);

   using ov_t = overloaded<Lambdas...>;
   ov_t  ov;

   static_selecta(Lambdas&&... lambdas) : ov{std::forward<Lambdas>(lambdas)...} {}

   using fn_ptr_t = void (*)(ov_t&) noexcept(all_noexcept);

   template <typename Lambda>
   static constexpr auto make_caller()
   {
      return [](ov_t& ov) noexcept(all_noexcept) { static_cast<Lambda>(ov)(); };
   }

   constexpr static fn_ptr_t  fn[] = { make_caller<Lambdas>()... };

   void operator()(size_t n) noexcept(all_noexcept) { fn[n](ov); }
};

