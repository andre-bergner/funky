#include "pipeline.h"
#include <iostream>

int main()
{
   using namespace pipeline;

   auto for_each = [](auto&& rng){
      return source([&rng](auto&& sink){
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



   const auto xs1 = {1,2,3,4,5,6,7,8,9,10,11};
   const auto x123 = {1,2,3};
   const auto x456 = {'a','b','c'};

   std::cout << "--- for_each ----------" << std::endl;

   for_each(xs1)
   | pipeline()
   | subscribe([](auto x) { std::cout << x << std::endl; });;

   std::cout << "--- join -------------" << std::endl;

   join(for_each(x123),for_each(x456))
   | subscribe([](auto x) { std::cout << x << std::endl; });;

   std::cout << "--- group_by_n -------" << std::endl;

   for_each(std::initializer_list<int>{1,2,3,4,5,6,7,8,9,10,11})
   | group_by_n<2,int>()
   | subscribe([](auto&& xs) { for (auto x:xs) std::cout << x << " "; std::cout << std::endl; });;

}
