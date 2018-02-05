#include <iostream>
#include "jdottir.hpp"
#include "parser.hpp"


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

   Value value
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

   std::cout << value << std::endl;
   {
      std::cout << "------- [] ---------" << std::endl;
      std::cout << value["a double"] << std::endl;
      std::cout << value["wrong index"] << std::endl;

      value["a double"].visit
      (  [](double x){ std::cout << x << std::endl; }
      ,  [](auto const&){ std::cout << "not a double" << std::endl; }
      );

      assert( value == value );
      assert( !(value != value) );

      if (value["a double"] == 3.14)
         std::cout << "value[\"a double\"] == 3.14  ✔" << std::endl;
      else
         std::cout << "value[\"a double\"] == 3.14  ✗" << std::endl;

      if (3.14 != value["a double"])
         std::cout << "value[\"a double\"] != 3.14  ✔" << std::endl;
      else
         std::cout << "value[\"a double\"] != 3.14  ✗" << std::endl;

      if (value["a double"] == "hello")
         std::cout << "value[\"a double\"] == \"hello\"  ✔" << std::endl;
      else
         std::cout << "value[\"a double\"] == \"hello\"  ✗" << std::endl;

      if ("hello" != value["a double"])
         std::cout << "value[\"a double\"] != \"hello\"  ✔" << std::endl;
      else
         std::cout << "value[\"a double\"] != \"hello\"  ✗" << std::endl;

   }



   std::cout << "-------------------------" << std::endl;
   std::cout << "PARSING TEST" << std::endl;
   std::cout << "-------------------------" << std::endl;

   try {
      parse(value, map
         (  "a bool"_key          >> [&](bool x)    { std::cout << "bool: " << x << std::endl; }
         ,  "a double"_key        >> [&](double x)  { std::cout << "double: " << x << std::endl; }
         //,  "homogen. array"_key  >> [&](std::vector<int> xs)  { }
         //,  "heteroge. array"_key >> [&](std::tuple<int,float,bool,std::string> t) {  }
         ,  "another tree"_key >> map
            (  "Elite"_key >> [&](int x)         {  std::cout << "  elite: " << x << std::endl;  }
            ,  "key"_key   >> [&](std::string x) {  std::cout << "  key: " << x << std::endl;  }
            )
         )
      );
   }
   catch (std::runtime_error const& e) {
      std::cout << e.what() << std::endl;
   }
}
