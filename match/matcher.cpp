//#define MATCHER_SELECTA
#include "matcher.hpp"

template <int N>
using int_ = std::integral_constant<int,N>;

#include <iostream>

int main()
{
   using namespace std;

#if 1
   for (auto n : {1,2,3,4,5,6,7})
      match(n)
      (  [&](int_<4>){ cout << "4our" << endl; }
      ,  [&](int_<6>){ cout << "6ix"  << endl; }
      ,  [&](int_<1>){ cout << "1ne"  << endl; }
      ,  [&](int_<3>){ cout << "3ree"  << endl; }
      ,  [&](int_<7>){ cout << "7even" << endl; }
      );
#else
   volatile int k;
   volatile int n = 3;
      match(n)
      (  [&](int_<4>){ k = 314; }
      ,  [&](int_<6>){ k = 47; }
      ,  [&](int_<1>){ k = 1337; }
      ,  [&](int_<3>){ k = 7357; }
      ,  [&](int_<7>){ k = -42; }
      );
#endif
}

