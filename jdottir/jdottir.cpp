#include "jdottir.hpp"
#include <iostream>


int main()
{
   std::cout << std::boolalpha;

   //Value vt0{ "homogen. array", {1, 2, 3}}; //  does not compile, yet
   Value single_key_value{ { "the answer", 42 } };
   Value single_list{ 1, 2.01, "three" };

   Value vt
   {  { "a double", 3.14 }
   ,  { "a string", "hello" }
   ,  { "nothing", nullptr }
   ,  { "a bool", true }
   ,  { "homogen. array", {1, 2, 3} }
   ,  { "heteroge. array", {1, 2.71, true, "text" } }
   ,  { "another tree",
         {  {"Elite", 1337 }
         ,  {"key", "value" }
         }
      }
   };

   // Syntactic Sugar
   Value sugar
   {  "a double"_k   ( 3.14 )
   ,  "a string"_k   ("hello")
   ,  "a bool"_k  >>  true
   ,  "homogen. array"_k( 1, 2, 3 )
   //,  "heteroge. array"_k  ({ 1, 2.71, true, "text" })
   ,  { "another tree",
         {  {"Elite", 1337 }
         ,  {"key", "value" }
         }
      }
   };

   std::cout << vt << std::endl;

   auto vt2 = vt;
}
