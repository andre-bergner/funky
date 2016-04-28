#include <tuple>

template < typename Function , typename... Args >
auto curry( Function&& f , Args&&... args );


//   -----------------------------------------------------------------------------------
//  The curry-binder
//   -----------------------------------------------------------------------------------

template < typename Function , typename... Bound_args >
struct Currying_binder
{
private:

   Function const &    f_;
   std::tuple< typename std::remove_reference<Bound_args>::type... >   args_;

   struct curry_arguments {};
   struct try_to_invoke_function : curry_arguments {};

   template < typename... Args >
   auto dispatch( try_to_invoke_function , Args&&... args ) const -> decltype( f_(args...) )
   {  return  f_( std::forward<Args>(args)... );  }

   template < typename... Args >
   auto dispatch( curry_arguments , Args&&... args ) const -> decltype( curry( f_ , std::forward<Args>(args)... ) )
   { return  curry( f_ , std::forward<Args>(args)... ); }

   template < std::size_t... Ns , typename... Other_args >
   auto call( std::index_sequence<Ns...> , Other_args&&... other_args )
   {  return dispatch( try_to_invoke_function{} , std::get<Ns>(args_)... , std::forward<Other_args>(other_args)... );  }

public:

   Currying_binder( Function const &f , Bound_args&&... as )
   : f_(f) , args_( std::forward<Bound_args>(as)...) {}


   template < typename... Other_args >
   auto operator()( Other_args&&... other_args ) 
   {
      return  call( std::make_index_sequence<sizeof...(Bound_args)>()
                  , std::forward<Other_args>(other_args)... );
   }

};


template < typename Function , typename... Args >
auto curry( Function&& f , Args&&... args )
{
   return Currying_binder<Function,Args...>( f , std::forward<Args>(args)... );
}









//   ----------------------------------------------------------------------------

#define MAKE_TPL_FUNCTOR( FUNCTOR , FUNCTION_TEMPLATE )                        \
namespace {                                                                    \
   struct {                                                                    \
      template < typename V1 , typename V2 >                                   \
      auto operator()( V1&& v1 , V2&& v2 ) const                               \
      {                                                                        \
         return FUNCTION_TEMPLATE( std::forward<V1>(v1)                        \
                                 , std::forward<V2>(v2) );                     \
      }                                                                        \
   }                                                                           \
   FUNCTOR;                                                                    \
}                                                                              \

//   ----------------------------------------------------------------------------

#define MAKE_TPL_FUNCTOR2( FUNCTOR , FUNCTION_TEMPLATE )                       \
namespace {                                                                    \
   auto FUNCTOR = []( auto&& v1 , auto&& v2 )                                  \
   {                                                                           \
      return FUNCTION_TEMPLATE( std::forward<decltype(v1)>(v1)                 \
                              , std::forward<decltype(v2)>(v2) );              \
   };                                                                          \
}                                                                              \

//   ----------------------------------------------------------------------------

#define MAKE_TPL_FUNCTOR3( FUNCTOR , FUNCTION_TEMPLATE )                       \
namespace {                                                                    \
   auto FUNCTOR = []( auto&&... vs )                                           \
   {                                                                           \
      return FUNCTION_TEMPLATE( std::forward<decltype(vs)>(vs) );              \
   };                                                                          \
}                                                                              \

//   ----------------------------------------------------------------------------

#define CURRY( CURRY_FUNCTION , FUNCTION_TEMPLATE )                            \
   MAKE_TPL_FUNCTOR2( FUNCTION_TEMPLATE##__CURRYFIED____ , FUNCTION_TEMPLATE ) \
   auto CURRY_FUNCTION  =  curry( FUNCTION_TEMPLATE##__CURRYFIED____ );        \



template < typename Value1 , typename Value2 >//, requires< is_addable<Value1,Value2>()... >
auto add_impl( Value1 const & v1 , Value2 const & v2 )
{
   return v1 + v2;
}

CURRY( add , add_impl );

auto sub = curry( [](auto x , auto y) { return x-y; } );
auto mac = curry( [](auto x , auto y , auto z) { return x + y*z; } );




#include <cassert>
using namespace std;

int main()
{
   auto add3to = add(3);
   assert( add3to(4) == 7 );
   assert( add(3)(4) == 7 );
   assert( add(3, 4) == 7 );

   auto subfrom8 = sub(8);
   assert( subfrom8(3) == 5 );
   assert( sub(8)(3)   == 5 );
   assert( sub(8, 3)   == 5 );

   assert( mac(1)(2,3)        == 7 );
   assert( mac(1,2)(3)        == 7 );
   assert( mac(1)(2)(3)       == 7 );
   assert( mac()(1)()(2)()(3) == 7 );
}
