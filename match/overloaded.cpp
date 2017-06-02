#include <iostream>
#include <string>


template <typename... Lambdas>
struct overloaded : public Lambdas...
{
   using Lambdas::operator()...;

   overloaded(Lambdas&&... lambdas) : Lambdas(std::forward<Lambdas>(lambdas))... {}
};


template <typename... Lambdas>
auto overload(Lambdas&&... ls) -> overloaded<std::decay_t<Lambdas>...>
{
   return { std::forward<Lambdas>(ls)... };
};





int main()
{
   using namespace std;
   auto f = overloaded
   {  [](int x)      { std::cout << "int: " << x << std::endl; }
   ,  [](string x)   { std::cout << "string: " << x << std::endl; }
   };

   f(3);
   f("hello");
}
