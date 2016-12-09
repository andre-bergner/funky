#include "pipeline.h"

#include <boost/range/adaptors.hpp>
#include <vector>
#include <algorithm>


#define NONIUS_RUNNER
#include <nonius/nonius.h++>
#include <nonius/main.h++>


namespace pl = pipeline;
namespace ad = boost::adaptors;

const auto numbers = []
{
   std::vector<int> v(10000);
   std::iota(v.begin(), v.end(), 0);
   return v;
}();


namespace pipeline
{
   auto for_each = [](auto&& rng){
      return source([&rng](auto&& sink){
         for (auto const& x : rng) if (!sink(x)) return;
      });
   };
}



NONIUS_BENCHMARK("pipeline filter", []
{
   volatile int sum = 0;
   pl::for_each(numbers)
   | pl::filter([](int x){ return x&1; })
   | pl::transform([](int x){ return x*x; })
   | pl::filter([](int x){ return x&1; })
   | pl::transform([](int x){ return x*x; })
   | pl::subscribe([&sum](int x){ sum += x; })
   ;
   return sum;
});


NONIUS_BENCHMARK("range filter",[]
{
   volatile int sum = 0;
   for (auto x : numbers
               | ad::filtered([](int x){ return x&1; })
               | ad::transformed([](int x){ return x*x; })
               | ad::filtered([](int x){ return x&1; })
               | ad::transformed([](int x){ return x*x; })
               )
   {
      sum += x;
   }
   return sum;
});


