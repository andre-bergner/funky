#pragma once

#include <iostream>
#include <functional>
#include <vector>

#define ASSERT( EXPR )                             \
    if (not bool(EXPR)) {                          \
        std::cout                                  \
        << "\n   🔥 Assertion '" #EXPR "' in line "  \
        << std::to_string(__LINE__) << " failed!"  \
        << std::endl;                              \
        ::nano_tests::g_current_test_succeeded = false; \
    }

#define ASSERT_FATAL( EXPR )                       \
    if (not bool(EXPR)) {                          \
        std::cout                                  \
        << "\n   🔥 Assertion '" #EXPR "' in line "  \
        << std::to_string(__LINE__) << " failed!"  \
        << std::endl;                              \
        ::nano_tests::g_current_test_succeeded = false; \
        throw ::nano_tests::abort_current_test{};  \
    }

namespace nano_tests {

    struct abort_current_test {};

    struct TestCase {
        std::string             name;
        std::function<void()>   run;
    };

    static std::vector<TestCase>  g_tests {};
    static bool                   g_current_test_succeeded = true;

    void def(std::string name, std::function<void()> t) {
        g_tests.push_back({ std::move(name), std::move(t) });
    }

    void run() {
        for (auto const& test : g_tests) {
            try {
                std::cout << "Running: " << test.name << std::flush;
                g_current_test_succeeded = true;
                test.run();
                if (g_current_test_succeeded)
                    std::cout << u8"  ✅" << std::endl;
            }
            catch(abort_current_test) {
                std::cout << "   • Test aborted." << std::endl;
            }
            catch(...) {
                std::cout << "   • Unknown exception occured." << std::endl;
            }
        }
    }
}
