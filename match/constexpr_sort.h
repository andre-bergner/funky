#pragma once

#include <cstddef>
#include <utility>

template <typename Value, int Size>
struct constexpr_array
{
   Value  data[Size];

   constexpr auto operator[](int n)       -> Value &       { return data[n]; }
   constexpr auto operator[](int n) const -> Value const&  { return data[n]; }

   constexpr size_t size() const { return Size; }

   constexpr Value const* begin() const { return data; }
   constexpr Value const* end () const  { return begin()+size(); }

   static constexpr size_t size_ = Size;
};

template <typename First, typename Second>
struct constexpr_pair
{
   First   first;
   Second  second;
};


template <typename Integer, Integer... ns>
constexpr auto bubble_sort_preserve_index()
{
   constexpr size_t N = sizeof...(ns);

   using pair_t = constexpr_pair<Integer,size_t>;
   size_t idx = 0;
   constexpr_array<pair_t, N> a = {pair_t{ns,idx++}...};

   if (N > 1)
      for (size_t i = 0;  i < N - 1;  i++)
      {
         for (size_t j = 0;  j < N - i - 1;  j++)
         {
            if (a[j].first > a[j+1].first)
            {
               auto temp = a[j];
               a[j]   = a[j+1];
               a[j+1] = temp;
            }
         }
      }

   return a;
}


template <size_t N>
struct constexpr_pair_select;

template <>
struct constexpr_pair_select<0>
{
   template <typename First, typename Second>
   static constexpr First const& apply(constexpr_pair<First,Second> const& p) { return p.first; }

   template <typename First, typename Second>
   static constexpr First& apply(constexpr_pair<First,Second>& p) { return p.first; }
};

template <>
struct constexpr_pair_select<1>
{
   template <typename First, typename Second>
   static constexpr Second const& apply(constexpr_pair<First,Second> const& p) { return p.second; }

   template <typename First, typename Second>
   static constexpr Second& apply(constexpr_pair<First,Second>& p) { return p.second; }
};



template <size_t N, typename Integer, Integer... ns, size_t... is>
auto constexpr_sort_impl(std::integer_sequence<Integer, ns...>, std::index_sequence<is...> )
{
    constexpr auto array_sorted = bubble_sort_preserve_index<Integer, ns...>();
    return std::integer_sequence< Integer, constexpr_pair_select<N>::apply(array_sorted[is])... >{};
}


template <size_t N, typename Integer, Integer... ns>
auto constexpr_sort()
{
    auto seq = std::integer_sequence<Integer, ns...>();
    auto idx = std::make_index_sequence<sizeof...(ns)>();

    return constexpr_sort_impl<N>(seq, idx);
}



template <typename Integer, Integer... ns>
using constexpr_sort_t = decltype(constexpr_sort<0,Integer,ns...>());

template <typename Integer, Integer... ns>
using constexpr_sort_index_t = decltype(constexpr_sort<1,Integer,ns...>());


