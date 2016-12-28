#pragma once

#include <tuple>
#include <functional>
#include <type_traits>
#include <array>


namespace pipeline {

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




   //  ---------------------------------------------------------------------------------------------
   // Reactive Pipe
   //  ---------------------------------------------------------------------------------------------

   //  sink<T>:    bool(T)
   //  pipe<F>:    bool(sink<T>) -> bool(sink<U>)    i.e.  bool(bool(T)) -> bool(bool(U))
   //  source:     void(sink)

   struct source_tag {};
   struct pipe_tag   {};
   struct sink_tag   {};

   template <typename Tag, typename Function>
   struct Element
   {
      template <typename... A>
      auto operator()(A&&... args) { return f(FORWARD(args)...); }
      Function f;
   };

   template <typename ElementType>
   struct tag_of;

   template <typename Tag, typename F>
   struct tag_of<Element<Tag,F>> { using type = Tag; };

   template <typename ElementType>
   using tag_of_t = typename tag_of<std::decay_t<ElementType>>::type;



   template <typename F> using Source = Element<source_tag,F>;
   template <typename F> using Pipe   = Element<pipe_tag,F>;
   template <typename F> using Sink   = Element<sink_tag,F>;


   template <typename F> Source<F> source( F&& f ) { return { FORWARD(f) }; }
   template <typename F> Pipe<F>   pipe( F&& f )   { return { FORWARD(f) }; }
   template <typename F> Sink<F>   sink( F&& f )   { return { FORWARD(f) }; }





   template <typename SourceT, typename PipeT>
   decltype(auto) compose( source_tag, pipe_tag, SourceT&& s, PipeT&& p )
   {
      return source([ src = CAPTURE(s), pip=CAPTURE(p) ](auto&& sink)
      {
         return src.value.f(pip.value.f( FORWARD(sink) ) );
      });
   }

   template <typename PipeL, typename PipeR>
   decltype(auto) compose( pipe_tag, pipe_tag, PipeL&& pl, PipeR&& pr )
   {
      return pipe([ pl = CAPTURE(pl), pr = CAPTURE(pr) ](auto&& x)
      {
         return pl.value.f(pr.value.f( FORWARD(x) ));
      });
   }

   template <typename PipeT, typename SinkT>
   decltype(auto) compose( pipe_tag, sink_tag, PipeT&& p, SinkT&& s )
   {
      return sink(p.f(s.f));
   }

   template <typename SourceT, typename SinkT>
   decltype(auto) compose( source_tag, sink_tag, SourceT&& s, SinkT&& sk )
   {
      s.f(sk.f);
   }


   template <typename LeftElem, typename RightElem>
   decltype(auto) operator|( LeftElem&& l, RightElem&& r )
   {
      return compose( tag_of_t<LeftElem>{}, tag_of_t<RightElem>{}
                    , std::forward<LeftElem>(l), std::forward<RightElem>(r)
                    );
   }







   template <typename F, typename G>
   auto operator>( Pipe<F> p, G&& sink_func )
   {
      return p | sink(std::forward<G>(sink_func));
   }




   template <typename F>
   auto subscribe( F&& f )
   {
      return sink([f](auto&& x)
      {
         f(FORWARD(x));
         return true;
      });
   }


   template <typename F>
   auto filter( F&& f )
   {
      return pipe([f](auto&& a)
      {
         return [f=std::move(f),a](auto&& x) mutable
         {
            if (f(x)) return a(FORWARD(x));
            return true;
         };
      });
   }


   template <typename F>
   auto transform( F&& f )
   {
      return pipe([f](auto&& a)
      {
         return [f=std::move(f),a](auto&& x) mutable
         {
            return a(f(x));
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
         return [f=std::move(f),a,taking=true](auto&& x) mutable
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
         return [f=std::move(f),a,dropping=true](auto&& x) mutable
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



   template <typename Source1, typename Source2>
   decltype(auto) merge( source_tag, source_tag, Source1&& src1, Source2&& src2 )
   {
      return source([ src1 = CAPTURE(src1), src2 = CAPTURE(src2) ]
      (auto&& snk)
      {
         using sink_t = std::decay_t<decltype(snk)>;
         auto shared_sink = [shink = std::make_shared<sink_t>(FORWARD(snk))]
                            (auto&& x){ return (*shink)(FORWARD(x)); };
         src1.value.f(shared_sink);
         src2.value.f(shared_sink);
      });
   }

   template <typename Source1, typename Source2>
   decltype(auto) merge( Source1&& src1, Source2&& src2 )
   {
      return merge( tag_of_t<Source1>{}, tag_of_t<Source2>{}
                  , std::forward<Source1>(src1), std::forward<Source2>(src2)
                  );
   }

   // TODO fix these:

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


