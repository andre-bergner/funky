#pragma once

#include "jdottir.hpp"
#include "../funky/signature.h"
#include <tuple>
#include <exception>


struct Key_t { std::string_view key; };
Key_t operator""_key (const char* s, std::size_t n) { return { std::string_view(s,n) }; }


template <typename... Pairs>
struct map_t { std::tuple<Pairs...> m; };

//template<class... Pairs> map(Pairs...) -> map<Pairs...>;

template<class... Pairs>
auto map(Pairs&&... ps)
{
   // TODO check validity
   return map_t<Pairs...>{ {std::forward<Pairs>(ps)...} };
};


template <typename F>
auto operator>>(Key_t k, F&& f)
{
   return std::make_pair(k, std::forward<F>(f));
}

template <typename... Ts>
auto operator>>(Key_t k, map_t<Ts...> const& m)
{
   return std::make_pair(k, m);
}

template <typename F>
using key_value_t = std::pair<Key_t,F>;

/*
template <typename T>
void parse(Value const& v, key_value_t<T> const& p)
{
   auto l = p.second;
   using arg_t = argument_t<decltype(l), 0>;
   if (v.holds_alternative<arg_t>())
      v.visit(std::move(l), [](...){});
   else
      throw std::runtime_error("Could not parse" + std::string(p.first.key) + " to type " + typeid(arg_t).name());
}
*/
template <typename F>
void parse(Value const& v, F const& f)
{
   auto l = f;
   using arg_t = argument_t<decltype(l), 0>;
   if (v.holds_alternative<arg_t>())
      v.visit(std::move(l), [](...){});
   else
      throw std::runtime_error(std::string("Could not parse to type ") + typeid(arg_t).name());
}


template <size_t... Ns, typename Tuple, typename F>
void for_each( std::index_sequence<Ns...>, Tuple&& t, F&& f)
{
   (f( std::get<Ns>(t) ), ...);
}

template <typename... Fs>
void parse(Value const& v, map_t<key_value_t<Fs>...> const& m)
{
   for_each( std::make_index_sequence<sizeof...(Fs)>{}
           , m.m
           , [&v](auto const& p){ parse(v[p.first.key], p.second); }
           //, [&v](auto const& p){ parse(v[p.first.key], p); }
           );
}

/*
template <typename Structure>
void parse(Value const& v, Structure&& s)
{
   auto const& p = std::get<0>(s.m);
   parse(v[p.first.key], p);
}
*/