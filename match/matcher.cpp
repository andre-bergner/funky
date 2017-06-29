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
   volatile int n = 3;
   volatile auto k
   = match(n)
      (  [](int_<4>){ return 314; }
      ,  [](int_<6>){ return 47; }
      ,  [](int_<1>){ return 1337; }
      ,  [](int_<3>){ return 7357; }
      ,  [](int_<7>){ return -42; }
      );
#endif

   std::cout << "----- match 2 -----" << std::endl;

   volatile auto x = 4.1;
   auto result = match2(x)
   (  case_(4.1) >= []{ return 314; }
   ,  case_(6.1) >= []{ return 47; }
   ,  case_(1.1) >= []{ return 1337; }
   ,  case_(3.1) >= []{ return 7357; }
   ,  case_(7.1) >= []{ return -42; }
   );
   std::cout << *result << std::endl;
}

