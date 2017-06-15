#include <cstddef>
#include <utility>

template <typename Value, int Size>
struct constexpr_array
{
   Value  data[Size];

   constexpr auto operator[](int n)       -> Value &       { return data[n]; }
   constexpr auto operator[](int n) const -> Value const&  { return data[n]; }

   constexpr size_t size() const { return Size; }
   
   static constexpr size_t size_ = Size;
};



template <typename Integer, Integer... ns>
constexpr auto bubble_sort()
{
   constexpr size_t N = sizeof...(ns);

   constexpr_array<Integer, N> a = {ns...};

   if (N > 1)
      for (size_t i = 0;  i < N - 1;  i++)
      {
         for (size_t j = 0;  j < N - i - 1;  j++)
         {
            if (a[j] > a[j+1])
            {
               auto temp = a[j];
               a[j] = a[j+1];
               a[j+1]= temp;
            }
         }
      }

   return a;
}



template <typename Integer, Integer... ns, size_t... is>
auto constexpr_sort_impl(std::integer_sequence<Integer, ns...>, std::index_sequence<is...> )
{
    constexpr auto array_sorted = bubble_sort<Integer, ns...>();
    return std::integer_sequence< Integer, array_sorted[is]... >{};
}


template <typename Integer, Integer... ns>
auto constexpr_sort()
{
    auto seq = std::integer_sequence<Integer, ns...>();
    auto idx = std::make_index_sequence<sizeof...(ns)>();

    return constexpr_sort_impl(seq, idx);
}



template <typename Integer, Integer... ns>
using constexpr_sort_t = decltype(constexpr_sort<Integer,ns...>());



