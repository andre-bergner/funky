#include <ostream>
#include <string>
#include <initializer_list>
#include <variant>
#include <optional>
#include <string_view>
#include <vector>
#include <memory>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

 
// Value = bool | int | double | string | list<Value> | map<string, Value>

class Value
{
   // TODO: use arena allocators

   using key_value_t = std::pair<std::string, std::shared_ptr<Value>>;
   using map_t = std::vector<key_value_t>;

   template <typename T>
   static constexpr bool is_core_value
   =  std::is_convertible_v<T, bool>
   |  std::is_convertible_v<T, int>
   |  std::is_convertible_v<T, double>
   |  std::is_convertible_v<T, std::string>
   |  std::is_convertible_v<T, std::nullptr_t>
   ;

   template <bool... bs>
   static constexpr bool all_of = (bs && ...);

public:
   Value() {}

   Value(bool x)         : value_{x} {}
   Value(int x)          : value_{x} {}
   Value(double x)       : value_{x} {}
   Value(std::string x)  : value_{x} {}
   Value(const char* x)  : value_{std::string(x)} {}
   Value(std::nullptr_t) : value_{nullptr} {}

public:

   Value(std::initializer_list<std::pair<std::string,Value>> xs)
   :  value_{[&]{
         //std::vector<Value> values;
         map_t values;
         for (auto const& x : xs)
            values.emplace_back(std::move(x.first), std::make_shared<Value>(std::move(x.second)) );
         return values;
      }()}
   {}

   template <typename... Xs, typename = std::enable_if_t<all_of<is_core_value<Xs>...> >>
   //template <typename... Xs>
   Value(Xs... xs)
   :  value_{[&]{
         std::vector<Value> values;
         (values.emplace_back(std::move(xs)), ...);
         return values;
      }()}
   {}

   // efficient specialization for homogenuous container
   // template <typename... Xs, enable_if_t< is_same<Xs...> >>
   // Value(Xs... xs)


   std::optional<Value> operator[](std::string_view key)
   {
      using result_t = std::optional<Value>;
      return std::visit(overloaded
      {  [this](auto const&) -> result_t { return {}; }
      ,  [this, &key](map_t const& map) -> result_t {
            for (auto const& x : map)
               if (x.first == key) return *(x.second);
            return {};
         }
      }, value_);
   }


   friend std::ostream& operator<<(std::ostream& os, Value const& x)
   {
      std::visit(overloaded
      {  [&os](auto const& arg){ os << arg; }
      ,  [&os](std::string const& s){ os << '"' << s << '"'; }
      ,  [&os](std::nullptr_t){ os << "null"; }
      //,  [&os](key_value_t const& p){ os << p.first << ": " << *(p.second); }
      ,  [&os](map_t const& xs){
            os << "{";
            for (auto const& x : xs)
               os << '"' << x.first << "\": " << *(x.second) << std::endl;
            os << "}" << std::endl;
         }
      ,  [&os](std::vector<Value> const& xs){
            os << "[";
            for (auto const& x : xs)
               os << x << ", ";
            os << "]" << std::endl;
         }
      }, x.value_);
      return os;
   }

private:


   using value_t = std::variant
   <  std::nullptr_t
   ,  bool
   ,  int
   ,  double
   ,  std::string
   ,  std::vector<Value>
   ,  map_t
   >;

   value_t  value_;
};


struct Key {
   std::string value;

   template <typename... Args>
   auto operator()(Args&&... args)
   {
      return std::make_pair(value, Value{std::forward<Args>(args)...});
   }
};

auto operator>>( Key k, Value x) { return std::make_pair(k.value, std::move(x)); }
Key operator""_k (const char* s, std::size_t n) { return { std::string(s,s+n) }; }


