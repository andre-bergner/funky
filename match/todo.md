# TODO

* unit tests for...
  * correct slection of binary search
* benchmarks
* type match
  * fast type id:
    ```c++
    size_t get_next_id()
    {
      static size_t id_counter = 0;
      return id_counter++;
    }

    template <typename T>
    struct type_id { static const size_t id = get_next_id(); }
    ```
* match ranges
  ```c++
   match(n)
   (  [](int_<1337>)             { … }
   ,  [](any_of_int<4,2,7,1338>) { … }
   ,  [](range_<13,37>)          { … }
   ,  [](int_<7>)                { … }
   );
  ```
* match floats
  ```c++
   match(n)
   (  2.3_val >= []{ … }
   ,  3.2_val >= []{ … }
   ,  4.1_val >= []{ … }
   ,  5.0_val >= []{ … }
   );
  ```
* match case (linear search only)
  ```c++
   match(n)
   (  case_(obj1) >= []{ … }
   ,  case_(obj2) >= []{ … }
   ,  case_(obj3) >= []{ … }
   ,  case_(obj4) >= []{ … }
   );
  ```

* match multiple values
  ```c++
   match(n)
   (  case_(4.2,_) >= []{ … }
   ,  case_(_,6.1) >= []{ … }
   ,  case_(_,_)   >= []{ … }
   );
  ```
