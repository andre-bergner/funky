#pragma once

#include <ostream>
#include <string>
#include <initializer_list>
#include <variant>
#include <string_view>
#include <vector>
#include <map>
#include <memory>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;  // seems to be a bit broken on clang

template <class... Fs>
auto overload(Fs&&... fs) { return overloaded<Fs...>{ std::forward<Fs>(fs)...}; }

 
// Value = bool | int | double | string | list<Value> | map<string, Value>

class Value
{
   // TODO
   // * need dedicated RevValue object:
   //    * replaces whole tree when op=() && is called
   //    * structurals shares otherwise
   // * mutable view vs immutable view -> obj[key] will behave differently
   // * structural sharing
   // * immutable design
   // * use arena allocators

   //using map_t = std::map<std::string, std::shared_ptr<Value>, std::less<>>;
   using map_t = std::map<std::string, Value, std::less<>>;

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

   struct none_t : std::monostate {};
   static constexpr none_t none = {};

   Value()               : value_{none} {}
   Value(bool x)         : value_{x} {}
   Value(int x)          : value_{x} {}
   Value(double x)       : value_{x} {}
   Value(std::string x)  : value_{x} {}
   Value(const char* x)  : value_{std::string(x)} {}
   Value(std::nullptr_t) : value_{nullptr} {}

public:
   Value(std::initializer_list<std::pair<std::string,Value>> xs)
   :  value_{[&]{
         map_t values;
         for (auto const& x : xs)
            //values.emplace(std::move(x.first), std::make_shared<Value>(std::move(x.second)) );
            values.emplace(std::move(x.first), std::move(x.second) );
         return values;
      }()}
   {}

   template <typename... Xs, typename = std::enable_if_t<all_of<is_core_value<Xs>...> >>
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

   bool operator==(Value const& that) const { return this->value_ == that.value_; }
   bool operator!=(Value const& that) const { return this->value_ != that.value_; }

   template <typename T>
   bool holds_alternative() const
   {
      return std::holds_alternative<T>(value_);
   }

   template <typename... Visitors>
   auto visit(Visitors&&... vs) const
   {
      return std::visit(overload(std::forward<Visitors>(vs)...), value_);
   }

/*
   Value const& operator[](std::string_view key) const
   {
      return std::visit(overloaded
      {  [this](auto const&) -> Value const& { throw 1337; }
      ,  [this, &key](map_t const& map) -> Value const& {
            if (auto it = map.find(key); it != map.end())
               return *it->second;     // copies value
            throw 1337;
         }
      }, value_);
   }
*/

   Value& operator[](std::string_view key) // rvalue version for reference types:  &&
   {
      return std::visit(overloaded
      {  [this](auto const&) -> Value& { throw 1337; }
      ,  [this, &key](map_t& map) -> Value& {
            if (auto it = map.find(key); it != map.end())
               return it->second;
            return map[std::string(key)] = Value{};
         }
      }, value_);
   }


   Value& operator=(Value that) &&
   {
      this->value_ = std::move(that.value_);
      return *this;
   }

   Value& operator=(Value that) &
   {
      this->value_ = std::move(that.value_);
      return *this;
   }

   friend std::ostream& operator<<(std::ostream& os, Value const& x)
   {
      std::visit(overloaded
      {  [&os](auto const& arg){ os << arg; }
      ,  [&os](none_t const& s){ os << "<>"; }
      ,  [&os](std::string const& s){ os << '"' << s << '"'; }
      ,  [&os](std::nullptr_t){ os << "null"; }
      ,  [&os](map_t const& xs){
            os << "{";
            for (auto const& x : xs)
               os << '"' << x.first << "\": " << x.second << std::endl;
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
   <  none_t
   ,  std::nullptr_t
   ,  bool
   ,  int
   ,  double
   ,  std::string
   ,  std::vector<Value>
   ,  map_t
   >;

   value_t  value_;
};


template <typename X>
bool operator==(Value const& v, X const& rhs)
{
   return v.visit
   (  [&rhs] (X const& lhs) { return lhs == rhs; }
   ,  []     (auto const&)  { return false; }
   );
}

template <typename X>
bool operator!=(Value const& v, X const& rhs) { return !(v == rhs); }

template <typename X>
bool operator==(X const& lhs, Value const& v) { return v == lhs; }

template <typename X>
bool operator!=(X const& lhs, Value const& v) { return v != lhs; }


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

