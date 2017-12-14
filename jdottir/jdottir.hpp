#include <ostream>
#include <string>
#include <initializer_list>
#include <variant>
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

public:
   Value();

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

   template <typename... Xs>  // enable_if_t< is_convertible_to<Value,Xs>... >
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


   friend std::ostream& operator<<(std::ostream& os, Value const& x)
   {
      std::visit(overloaded
      {  [&os](auto const& arg){ os << arg; }
      ,  [&os](std::nullptr_t){ os << "NULL"; }
      //,  [&os](key_value_t const& p){ os << p.first << ": " << *(p.second); }
      ,  [&os](map_t const& xs){
            os << "{";
            for (auto const& x : xs)
               os << x.first << ": " << *(x.second) << std::endl;
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
   /*
   template <typename X>
   auto operator()(X&& x)
   {
      return std::make_pair(value, Value(std::forward<X>(x)));
   }*/

   auto operator()(std::initializer_list<Value> xs)
   {
      return std::make_pair(value, Value(std::move(xs)));
   }

};

auto operator>>( Key k, Value x) { return std::make_pair(k.value, std::move(x)); }
Key operator""_k (const char* s, std::size_t n) { return { std::string(s,s+n) }; }


