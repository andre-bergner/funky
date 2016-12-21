#include <tuple>
#include <functional>
#include <type_traits>
#include <iostream>

   #define  FORWARD(x)  std::forward<decltype(x)>(x)

   template <typename F> struct Source { F f; };
   template <typename F> Source<F> source( F&& f ) { return { FORWARD(f) }; }

   template <typename F> struct Pipe { F f; };
   template <typename F> Pipe<F> pipe( F&& f ) { return { FORWARD(f) }; }

   template <typename F> struct Sink { F f; };
   template <typename F> Sink<F> sink( F&& f ) { return { FORWARD(f) }; }


   template <typename F, typename G>
   auto operator|( Source<F> s, Pipe<G> p )
   {
      return source([ src=s.f, p=p.f ](auto&& sink){ return src(p(sink)); });
   }

   template <typename F, typename G>
   auto operator|( Pipe<F> l, Pipe<G> r )
   {
      return pipe([ f=l.f, g=r.f ](auto&& x){ return f(g((x))); });
   }

   template <typename F, typename G>
   auto operator|( Pipe<F> p, Sink<G> s )
   {
      return sink(p.f(s.f));
   }

   template <typename F, typename G>
   void operator|( Source<F> s, Sink<G> p )
   {
      s.f(p.f);
   }

   template <typename F>
   auto merge_in( Source<F> src )
   {
      return pipe([src=std::move(src)](auto&& snk)
      {
         src.f(std::ref(snk));
         return [&snk](auto&& x) mutable
         {
            snk(FORWARD(x));
         };
      });
   }

   auto take( size_t n )
   {
      return pipe([n](auto&& a)
      {
         return [n=n,a](auto&& x) mutable
         {
            if (n > 0) { --n; a(x); }
         };
      });
   }

   template <typename F>
   auto subscribe( F&& f )
   {
      return sink([f](auto&& v){ f(FORWARD(v)); });
   }


int main()
{
   std::function<void(int)>  c1;
   std::function<void(char)> c2;

   auto connect1 = [&](auto&& f){ c1 = FORWARD(f); };
   auto connect2 = [&](auto&& f){ c2 = FORWARD(f); };

   source(connect1)
   | merge_in(source(connect2))
   | take(3)
   | subscribe([](auto x){ std::cout << x; });

   c1(3);
   c2('x');
   c1(4);
   c2('y');
   c1(5);
   c2('z');

   std::cout << "\n" << std::endl;
}
