/*******************************************************************************
 * This file is part of the "https://github.com/BlackMATov/kari.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2017-2020, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <kari.hpp/kari.hpp>
#include "doctest/doctest.hpp"

#include <algorithm>
#include <numeric>
#include <vector>

TEST_CASE("kari_examples") {
    SUBCASE("Basic currying") {
        using namespace kari_hpp;

        auto foo = [](int a, int b, int c){
            return a + b + c;
        };

        auto c0 = curry(foo); // currying of `foo` function
        auto c1 = c0(10);     // apply to first argument
        auto c2 = c1(20);     // apply to second argument
        auto rr = c2(12);     // apply to third argument and call the `foo` function

        REQUIRE(rr == 42);
    }

    SUBCASE("Partial application") {
        using namespace kari_hpp;

        curry_t c0 = [](int v1, int v2, int v3, int v4) {
            return v1 + v2 + v3 + v4;
        };

        auto c1 = c0(15, 20); // partial application of two arguments
        auto rr = c1(2, 5);   // partial application and call `(15,20,2,5)`

        REQUIRE(rr == 42);
    }

    SUBCASE("Calling nested curried functions") {
        using namespace kari_hpp;

        curry_t boo = [](int a, int b) {
            return a + b;
        };

        curry_t foo = [boo](int a, int b) {
            return boo(a + b);
        };

        auto c0 = foo(38,3,1);
        auto c1 = foo(38,3)(1);
        auto c2 = foo(38)(3,1);

        REQUIRE(c0 == 42);
        REQUIRE(c1 == 42);
        REQUIRE(c2 == 42);
    }

    SUBCASE("Binding member functions and member objects") {
        using namespace kari_hpp;

        struct foo_t {
            int v = 40;
            int add_v(int add) {
                v += add;
                return v;
            }
        } foo;

        auto c0 = curry(&foo_t::add_v);
        auto c1 = curry(&foo_t::v);

        auto r0 = c0(std::ref(foo))(2);
        auto r1 = c1(std::cref(foo));

        REQUIRE(r0 == 42);
        REQUIRE(r1 == 42);
    }

    SUBCASE("API/is_curried") {
        using namespace kari_hpp;

        constexpr curry_t c = [](int a, int b){
            return a + b;
        };

        STATIC_REQUIRE(is_curried_v<decltype(c)>);
        STATIC_REQUIRE(is_curried<decltype(c)>::value);
    }

    SUBCASE("Section of operators") {
        using namespace kari_hpp::ext::underscore;
        std::vector v{1, 2, 3, 4};

        REQUIRE(std::accumulate(v.begin(), v.end(), 0, _ + _) == 10);

        std::transform(v.begin(), v.end(), v.begin(), _ * 2);
        REQUIRE(v == std::vector{2, 4, 6, 8});

        std::transform(v.begin(), v.end(), v.begin(), - _);
        REQUIRE(v == std::vector{-2, -4, -6, -8});
    }

    SUBCASE("Pipe operator") {
        using namespace kari_hpp::ext;
        using namespace kari_hpp::ext::underscore;

        constexpr auto r0 = (_*2) | (_+2) | 4; // (4 * 2) + 2 = 10
        constexpr auto r1 = 4 | (_*2) | (_+2); // (4 * 2 + 2) = 10

        STATIC_REQUIRE(r0 == 10);
        STATIC_REQUIRE(r1 == 10);
    }

    SUBCASE("Compose operator") {
        using namespace kari_hpp::ext;
        using namespace kari_hpp::ext::underscore;

        constexpr auto r0 = (_*2) * (_+2) * 4; // (4 + 2) * 2 = 12
        constexpr auto r1 = 4 * (_*2) * (_+2); // (4 * 2 + 2) = 10

        STATIC_REQUIRE(r0 == 12);
        STATIC_REQUIRE(r1 == 10);
    }

    SUBCASE("Point-free style") {
        using namespace kari_hpp::ext;
        using namespace kari_hpp::ext::underscore;

        // (. (+2)) (*2) $ 10 == 24 // haskell analog
        constexpr auto r0 = (_*(_+2))(_*2) * 10;

        // ((+2) .) (*2) $ 10 == 22 // haskell analog
        constexpr auto r1 = ((_+2)*_)(_*2) * 10;

        STATIC_REQUIRE(r0 == 24);
        STATIC_REQUIRE(r1 == 22);
    }
}
