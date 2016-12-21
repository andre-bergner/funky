#include "pipeline.h"
#include <iostream>


template <typename Proc, typename Dtor>
struct ProcessAndClose
{
   Proc p;
   Dtor d;

   template <typename... Args>
   auto operator()(Args&&... args) { return p(std::forward<Args>(args)...); }

   template <typename... Args>
   auto operator()(Args&&... args) const { return p(std::forward<Args>(args)...); }

   ~ProcessAndClose() { d(); }
};

template <typename Proc, typename Dtor>
auto process_and_close( Proc&& p, Dtor&& d )
{ return ProcessAndClose<Proc,Dtor>{ std::forward<Proc>(p), std::forward<Dtor>(d) }; }


auto for_each = [](auto&& rng){
   return pipeline::source([&rng](auto&& sink){
      for (auto const& x : rng) if (!sink(x)) return;
   });
};



int main()
{
   using namespace pipeline;

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


   auto print = [](auto x){ std::cout << x << std::endl; };

   const auto xs1 = {1,2,3,4,5,6,7,8,9,10,11};
   const auto x123 = {1,2,3};
   const auto xabc = {'a','b','c'};

   std::cout << "--- for_each ----------" << std::endl;

   for_each(xs1)
   | pipeline()
   | subscribe(print);

   std::cout << "--- merge ------------" << std::endl;

   merge(for_each(x123),for_each(xabc))
   | subscribe(print);

   std::cout << "--- merge | take 4 ---" << std::endl;

   merge(for_each(x123),for_each(xabc))
   | take(4)
   | subscribe(print);

   std::cout << "--- merge_in ---" << std::endl;

   for_each(x123)
   | transform  ([](auto x){ return x*x; })
   | merge_in(for_each(xabc))
   | transform  ([](auto x){ return static_cast<decltype(x)>(x+23); })
   | subscribe(print);

   std::cout << "--- group_by_n -------" << std::endl;

   for_each(std::initializer_list<int>{1,2,3,4,5,6,7,8,9,10,11})
   | group_by_n<2,int>()
   > [](auto&& xs) { for (auto x:xs) std::cout << x << " "; std::cout << std::endl; return true; };

   std::cout << "--- close stream ----" << std::endl;

   for_each(xs1)
   | transform([](auto x){ return x*x; })
   | subscribe(process_and_close( [](auto x){ std::cout << x << ", "; }
                                , []{ std::cout << std::endl; }
                                ));

   std::cout << "--- zip ----" << std::endl;

   zip(for_each(x123),for_each(xabc))
   | subscribe([](auto x) { std::cout << std::get<0>(x) << ", " << std::get<1>(x) << std::endl; });





   std::cout << "--- merge_in with callback ---" << std::endl;

   std::function<void(int)> c1;
   std::function<void(char)> c2;

   auto connect1 = [&](auto&& f){ c1 = FORWARD(f); };
   auto connect2 = [&](auto&& f){ c2 = FORWARD(f); };

   auto stream1 = source(connect1);
   auto stream2 = source(connect2);


   stream1
   | transform([](auto x){ return x*x; })
   | merge_in(stream2)
   | take(2)
   | subscribe(print);

   c1(3);
   c2('x');
   c1(4);
   c2('y');
   c1(5);
   c2('z');

   std::cout << "--- merge_in with l-value callback ---" << std::endl;

   auto tail = take(2)
             | subscribe(print);

   stream1
   | transform([](auto x){ return x*x; })
   | merge_in(stream2)
   | tail;

   c1(3);
   c2('x');
   c1(4);
   c2('y');
   c1(5);
   c2('z');


   std::cout << "--- merge_in with callback ---" << std::endl;

   merge(for_each(x123),for_each(xabc))
   | take(4)
   | subscribe(print);

   std::cout << "--- merge_in with callback ---" << std::endl;

   for_each(x123)
   | transform([](auto x){ return x*x; })
   | merge_in(for_each(xabc))
   | take(3)
   | subscribe(print);

}
