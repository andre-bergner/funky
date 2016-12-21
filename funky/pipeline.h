#pragma once

#include <tuple>
#include <functional>
#include <type_traits>
#include <array>


namespace pipeline {

   #define  FORWARD(x)  std::forward<decltype(x)>(x)


   //  ---------------------------------------------------------------------------------------------
   // Reactive Pipe
   //  ---------------------------------------------------------------------------------------------

   //  sink<T>:    bool(T)
   //  pipe<F>:    bool(sink<T>) -> bool(sink<U>)    i.e.  bool(bool(T)) -> bool(bool(U))
   //  source:     void(sink)

   template <typename F>
   struct Source
   {
      F f;
   };

   template <typename F>
   Source<F> source( F&& f ) { return { FORWARD(f) }; }


   template <typename F>
   struct Pipe
   {
      F f;
   };

   template <typename F>
   Pipe<F> pipe( F&& f ) { return { FORWARD(f) }; }


   template <typename F>
   struct Sink
   {
      template <typename A>
      auto operator()(A&& arg) { return f(FORWARD(arg)); }
      F f;
   };

   template <typename F>
   Sink<F> sink( F&& f ) { return { FORWARD(f) }; }



   template <typename F, typename G>
   auto operator|( Source<F> s, Pipe<G> p )
   {
      return source([ src=s.f, p=p.f ](auto&& sink){ return src(p(sink)); });
   }

   template <typename F, typename G>
   auto operator|( Pipe<F> l, Pipe<G> r )  // -> Pipe<F*G>
   {
      return pipe([ f=l.f, g=r.f ](auto&& x){ return f(g((x))); });
   }

   template <typename F, typename G>
   auto operator|( Pipe<F> p, Sink<G> s )  // -> Sink<F*G>
   {
      return sink(p.f(s.f));
   }

   template <typename F, typename G>
   void operator|( Source<F> s, Sink<G> p )
   {
      s.f(p.f);
   }


   template <typename F, typename G>
   auto operator>( Pipe<F> p, G&& sink_func )
   {
      return p | sink(std::forward<G>(sink_func));
   }




   template <typename F>
   auto subscribe( F&& f )
   {
      return sink([f](auto&& v){ f(std::forward<decltype(v)>(v)); return true; });
   }


   template <typename F>
   auto filter( F&& f )
   {
      return pipe([f](auto&& a)
      {
         return [f,a](auto&& e) mutable
         {
            if (f(e)) return a(e);
            return true;
         };
      });
   }


   template <typename F>
   auto transform( F&& f )
   {
      return pipe([&f](auto&& a)
      {
         return [&f,a](auto&& e) mutable
         {
            return a(f(e));
         };
      });
   }


   auto repeat( size_t n )
   {
      return pipe([n](auto&& a)
      {
         return [n,a](auto&& x) mutable
         {
            auto k = n;
            while (k --> 0)
               if (!a(x)) return false;
            return true;
         };
      });
   }


   auto take( size_t n )
   {
      return pipe([n](auto&& a)
      {
         return [n=n,a](auto&& x) mutable
         {
            if (n > 0)  { --n; return a(x); }
            return false;
         };
      });
   }


   template <typename F>
   auto take_until( F&& f )
   {
      return pipe([f](auto&& a)
      {
         return [f,a,taking=true](auto&& x) mutable
         {
            if (taking)
            {
               if (f(x))
                  return taking = false;
               return a(x);
            }
            return false;
         };
      });
   }


   auto drop( size_t n )
   {
      return pipe([n](auto&& a)
      {
         return [n=n,a](auto&& x) mutable
         {
            if (n > 0) { --n; return true; }
            return a(x);
         };
      });
   }


   template <typename F>
   auto drop_until( F&& f )
   {
      return pipe([f](auto&& a)
      {
         return [f,a,dropping=true](auto&& x) mutable
         {
            if (dropping)
            {
               if (!f(x))  return true;
               dropping = false;
            }
            return a(x);
         };
      });
   }


   template <size_t GroupSize, typename Value>
   auto group_by_n()
   {
      return pipe([](auto&& snk)
      {
         return [n=size_t{0},snk,buf=std::array<Value,GroupSize>{{}}](auto&& x) mutable
         {
            buf[n++] = std::forward<decltype(x)>(x);
            if (n == GroupSize)
            {
               n=0;
               return snk(buf);
            }
            else
               return true;
         };
      });
   }



   template <typename F1, typename F2>
   auto merge( Source<F1> src1, Source<F2> src2 )
   {
      return source([=](auto snk){
         src1.f(std::ref(snk));
         src2.f(std::ref(snk));
      });
   }


   template <typename F>
   auto merge_in( Source<F> src )
   {
      return pipe([src=std::move(src)](auto&& snk)
      {
         //src.f(snk);
         src.f(std::ref(snk));
         return [&snk](auto&& x) mutable
         {
            return snk(FORWARD(x));
         };
      });
   }



   template <typename F1, typename F2>
   auto zip( Source<F1> src1, Source<F2> src2 )
   {
      return source([=](auto snk){
         int x1_ = 0;  // src1::value_type
         char x2_ = 0;  // src2::value_type

         src1.f([&snk,&x1_,&x2_](auto x1){ x1_ = x1; return snk(std::make_tuple(x1_,x2_)); });
         src2.f([&snk,&x1_,&x2_](auto x2){ x2_ = x2; return snk(std::make_tuple(x1_,x2_)); });
      });
   }
}


