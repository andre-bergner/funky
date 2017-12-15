#include <iostream>
#include "jdottir.hpp"


int main()
{
   std::cout << std::boolalpha;

   {
      std::cout << "------- default ---------" << std::endl;
      Value v;
      std::cout << v << std::endl;
   }

   {  // basic copy
      std::cout << "------- basic copy ---------" << std::endl;

      Value v1{ 1337 };
      auto v2 = v1;
      std::cout << v1 << std::endl;
      std::cout << v2 << std::endl;
   }

   std::cout << "------- complex tree ---------" << std::endl;
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
   {
      std::cout << "------- [] ---------" << std::endl;
      std::cout << vt["a double"] << std::endl;
      std::cout << vt["wrong index"] << std::endl;
   }
}
