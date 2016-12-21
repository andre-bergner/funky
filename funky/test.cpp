#include <tuple>
#include <functional>
#include <type_traits>
#include <iostream>

   template <typename Type>
   using remove_rvalue_reference_t = std::conditional_t
                                     <  std::is_rvalue_reference<Type>::value
                                     ,  std::remove_reference_t<Type>
                                     ,  Type
                                     >;

   template <typename X>
   auto ref_or_move(X&& x) -> remove_rvalue_reference_t<X&&> { return std::forward<X>(x); }

   template <typename X>
   struct capture_t
   {
      remove_rvalue_reference_t<X&&> value;
   };

   template <typename X>
   capture_t<X&&> capture(X&& x) { return {std::forward<X>(x)}; }


   #define  FORWARD(x)  std::forward<decltype(x)>(x)
   #define  CAPTURE(x)  capture(FORWARD(x))


   struct source_tag {};
   struct pipe_tag   {};
   struct sink_tag   {};

   template <typename Tag, typename Function>
   struct Element
   {
      Function f;
   };

   template <typename F> using Source = Element<source_tag,F>;
   template <typename F> using Pipe   = Element<pipe_tag,F>;
   template <typename F> using Sink   = Element<sink_tag,F>;


   template <typename F> Source<F> source( F&& f ) { return { FORWARD(f) }; }
   template <typename F> Pipe<F> pipe( F&& f ) { return { FORWARD(f) }; }
   template <typename F> Sink<F> sink( F&& f ) { return { FORWARD(f) }; }




   template <typename F, typename G>
   auto operator|( Source<F> s, Pipe<G> p )
   {
      return source([ src=s.f, p=p.f ](auto&& sink){ return src(p(sink)); });
   }
/*
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
*/
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
         return [n=n,a=CAPTURE(a)](auto&& x) mutable
         {
            if (n > 0) { --n; a.value(FORWARD(x)); }
         };
      });
   }

   template <typename F>
   auto subscribe( F&& f )
   {
      return sink([f=CAPTURE(f)](auto&& v){ f.value(FORWARD(v)); });
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

/*
   int n = 4;
   auto&& k1 = ref_or_move(n);
   auto&& k2 = ref_or_move(4);
   std::cout << std::is_lvalue_reference<decltype(k1)>::value << "   " << typeid(k1).name() << std::endl;
   std::cout << std::is_lvalue_reference<decltype(k2)>::value << "   " << typeid(k2).name() << std::endl;

   [](auto&& l1, auto&& l2){
      return [k1 = capture(FORWARD(l1)), k2 = capture(FORWARD(l2))]{
         std::cout << std::is_lvalue_reference<decltype(k1.value)>::value << "   " << typeid(k1.value).name() << std::endl;
         std::cout << std::is_lvalue_reference<decltype(k2.value)>::value << "   " << typeid(k2.value).name() << std::endl;
      };
   }(n,4)();
*/
}
