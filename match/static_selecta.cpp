#include "static_selecta.h"

#include <iostream>
#include <string>


int main()
{
   using namespace std;

   auto sel = static_selecta
   {  [x = 3]     { std::cout << "first: " << x << std::endl; }
   ,  [x = 3.14]  { std::cout << "second:" << x << std::endl; }
   };

   volatile int n = 0;
   sel(n);
   n = 1;
   sel(n);
}
