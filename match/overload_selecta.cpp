#include <iostream>
#include <string>


template <typename... Lambdas>
struct overloaded : public Lambdas...
{
   using Lambdas::operator()...;

   overloaded(Lambdas&&... lambdas) : Lambdas(std::forward<Lambdas>(lambdas))... {}
};


template <typename... Lambdas>
struct selecta
{
   using ov_t = overloaded<Lambdas...>;
   ov_t  ov;

   selecta(Lambdas&&... lambdas) : ov{std::forward<Lambdas>(lambdas)...} {}

   using fn_ptr_t = void (*)(ov_t&);

   template <typename Lambda>
   static constexpr auto make_caller() { return [](ov_t& ov){ static_cast<Lambda>(ov)(); }; }

   constexpr static fn_ptr_t  fn[] = { make_caller<Lambdas>()... };

   void operator()(size_t n) { fn[n](ov); }
};





template <typename... Lambdas>
auto overload(Lambdas&&... ls) -> overloaded<std::decay_t<Lambdas>...>
{
   return { std::forward<Lambdas>(ls)... };
};





int main()
{
   using namespace std;

   auto sel = selecta
   {  [x = 3]     { std::cout << "first: " << x << std::endl; }
   ,  [x = 3.14]  { std::cout << "second:" << x << std::endl; }
   };

   volatile int n = 0;
   sel(n);
   n = 1;
   sel(n);
}



/*

std::ctype<char>::do_widen(char) const:
        mov     eax, esi
        ret
.LC0:
        .string "second:"
auto selecta<main::{lambda()#1}, main::{lambda()#2}>::make_caller<main::{lambda()#1}>()::{lambda(overloaded<{lambda()#1}, main::{lambda()#1}>&)#1}::_FUN(overloaded):
        push    rbp
        push    rbx
        mov     edx, 7
        mov     esi, OFFSET FLAT:.LC0
        sub     rsp, 24
        movsd   xmm0, QWORD PTR [rdi+8]
        mov     edi, OFFSET FLAT:std::cout
        movsd   QWORD PTR [rsp+8], xmm0
        call    std::basic_ostream<char, std::char_traits<char> >& std::__ostream_insert<char, std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*, long)
        movsd   xmm0, QWORD PTR [rsp+8]
        mov     edi, OFFSET FLAT:std::cout
        call    std::basic_ostream<char, std::char_traits<char> >& std::basic_ostream<char, std::char_traits<char> >::_M_insert<double>(double)
        mov     rbp, rax
        mov     rax, QWORD PTR [rax]
        mov     rax, QWORD PTR [rax-24]
        mov     rbx, QWORD PTR [rbp+240+rax]
        test    rbx, rbx
        je      .L10
        cmp     BYTE PTR [rbx+56], 0
        je      .L5
        movsx   esi, BYTE PTR [rbx+67]
.L6:
        mov     rdi, rbp
        call    std::basic_ostream<char, std::char_traits<char> >::put(char)
        add     rsp, 24
        mov     rdi, rax
        pop     rbx
        pop     rbp
        jmp     std::basic_ostream<char, std::char_traits<char> >::flush()
.L5:
        mov     rdi, rbx
        call    std::ctype<char>::_M_widen_init() const
        mov     rax, QWORD PTR [rbx]
        mov     esi, 10
        mov     rax, QWORD PTR [rax+48]
        cmp     rax, OFFSET FLAT:std::ctype<char>::do_widen(char) const
        je      .L6
        mov     rdi, rbx
        call    rax
        movsx   esi, al
        jmp     .L6
.L10:
        call    std::__throw_bad_cast()
.LC1:
        .string "first: "
auto selecta<main::{lambda()#1}, main::{lambda()#2}>::make_caller<{lambda()#1}>()::{lambda(overloaded<{lambda()#1}, main::{lambda()#1}>&)#1}::_FUN(overloaded):
        push    rbp
        push    rbx
        mov     edx, 7
        mov     esi, OFFSET FLAT:.LC1
        sub     rsp, 8
        mov     ebx, DWORD PTR [rdi]
        mov     edi, OFFSET FLAT:std::cout
        call    std::basic_ostream<char, std::char_traits<char> >& std::__ostream_insert<char, std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*, long)
        mov     edi, OFFSET FLAT:std::cout
        mov     esi, ebx
        call    std::basic_ostream<char, std::char_traits<char> >::operator<<(int)
        mov     rbp, rax
        mov     rax, QWORD PTR [rax]
        mov     rax, QWORD PTR [rax-24]
        mov     rbx, QWORD PTR [rbp+240+rax]
        test    rbx, rbx
        je      .L18
        cmp     BYTE PTR [rbx+56], 0
        je      .L13
        movsx   esi, BYTE PTR [rbx+67]
.L14:
        mov     rdi, rbp
        call    std::basic_ostream<char, std::char_traits<char> >::put(char)
        add     rsp, 8
        mov     rdi, rax
        pop     rbx
        pop     rbp
        jmp     std::basic_ostream<char, std::char_traits<char> >::flush()
.L13:
        mov     rdi, rbx
        call    std::ctype<char>::_M_widen_init() const
        mov     rax, QWORD PTR [rbx]
        mov     esi, 10
        mov     rax, QWORD PTR [rax+48]
        cmp     rax, OFFSET FLAT:std::ctype<char>::do_widen(char) const
        je      .L14
        mov     rdi, rbx
        call    rax
        movsx   esi, al
        jmp     .L14
.L18:
        call    std::__throw_bad_cast()
main:
        sub     rsp, 40
        mov     rax, QWORD PTR .LC2[rip]
        mov     DWORD PTR [rsp+12], 0
        lea     rdi, [rsp+16]
        mov     DWORD PTR [rsp+16], 3
        mov     QWORD PTR [rsp+24], rax
        movsx   rax, DWORD PTR [rsp+12]
        call    [QWORD PTR selecta<main::{lambda()#1}, main::{lambda()#2}>::fn[0+rax*8]]
        mov     DWORD PTR [rsp+12], 1
        movsx   rax, DWORD PTR [rsp+12]
        lea     rdi, [rsp+16]
        call    [QWORD PTR selecta<main::{lambda()#1}, main::{lambda()#2}>::fn[0+rax*8]]
        xor     eax, eax
        add     rsp, 40
        ret
_GLOBAL__sub_I_main:
        sub     rsp, 8
        mov     edi, OFFSET FLAT:std::__ioinit
        call    std::ios_base::Init::Init()
        mov     edx, OFFSET FLAT:__dso_handle
        mov     esi, OFFSET FLAT:std::__ioinit
        mov     edi, OFFSET FLAT:std::ios_base::Init::~Init()
        add     rsp, 8
        jmp     __cxa_atexit
selecta<main::{lambda()#1}, main::{lambda()#2}>::fn:
        .quad   auto selecta<main::{lambda()#1}, main::{lambda()#2}>::make_caller<{lambda()#1}>()::{lambda(overloaded<{lambda()#1}, main::{lambda()#1}>&)#1}::_FUN(overloaded)
        .quad   auto selecta<main::{lambda()#1}, main::{lambda()#2}>::make_caller<main::{lambda()#1}>()::{lambda(overloaded<{lambda()#1}, main::{lambda()#1}>&)#1}::_FUN(overloaded)
.LC2:
        .long   1374389535
        .long   1074339512
  */
