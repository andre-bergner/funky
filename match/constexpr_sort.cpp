#include "constexpr_sort.h"
#include <iostream>

template <int... ns>
void f(std::integer_sequence<int,ns...>)
{
    constexpr int xs[] = {ns...};

    for (int i : xs)
       std::cout << i << " ";

    std::cout << std::endl;
}


int main()
{
   f(constexpr_sort_t<int, 4, 7, 2, 9, 3, 7>{});
}


