#include "pipeline.h"
#include <iostream>

int main()
{
   using namespace pipeline;

   auto for_each = [](auto&& rng){
      return source([rng](auto&& sink){
         for (auto const& x : rng) if (!sink(x)) return;
      });
   };

   auto pipeline = []{
      return filter     ([](auto x){ return x>3; })
           | transform  ([](auto x){ return x*x; })
           | filter     ([](auto x){ return x<150; })
           | transform  ([](auto x){ return x-10; })
           | take(3)
           | repeat(2)
           ;
   };

   auto p = pipeline() | sink([](auto x) { std::cout << "sink: " << x << std::endl; return true; });

   int n = 0;
   do { std::cout << "source: " << n << std::endl; }
   while ( p(n++) );

   std::cout << "cancelled" << std::endl;

   for_each(std::initializer_list<int>{1,2,3,4,5,6,7,8,9,10,11})
           | pipeline()
           | sink([](auto x) { std::cout << "sink: " << x << std::endl; return true; });;

   std::cout << "cancelled" << std::endl;
}
