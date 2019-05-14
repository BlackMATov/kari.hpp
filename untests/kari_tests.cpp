/*******************************************************************************
 * This file is part of the "https://github.com/BlackMATov/kari.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2017-2019, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#define CATCH_CONFIG_FAST_COMPILE
#include <catch2/catch.hpp>

#include <kari.hpp/kari.hpp>

#include <cstdio>
#include <cstring>

namespace
{
    const auto _42_tl = []() {
        return 42;
    };

    const auto id_tl = [](int v) {
        return v;
    };

    const auto minus2_tl = [](int v1, int v2) {
        return v1 - v2;
    };

    const auto minus3_tl = [](int v1, int v2, int v3) {
        return v1 - v2 - v3;
    };

    const auto minus4_tl = [](int v1, int v2, int v3, int v4) {
        return v1 - v2 - v3 - v4;
    };
}

namespace
{
    const auto id_gl = [](auto&& v) {
        return v;
    };

    const auto minus2_gl = [](auto&& v1, auto&& v2) {
        return v1 - v2;
    };

    const auto minus3_gl = [](auto&& v1, auto&& v2, auto&& v3) {
        return v1 - v2 - v3;
    };

    const auto minus4_gl = [](auto&& v1, auto&& v2, auto&& v3, auto&& v4) {
        return v1 - v2 - v3 - v4;
    };
}

namespace
{
    struct id_gf {
        template < typename T >
        constexpr auto operator()(T&& v) const {
            return v;
        }
    };

    struct minus2_gf {
        template < typename T1, typename T2 >
        constexpr auto operator()(T1&& v1, T2&& v2) const {
            return v1 - v2;
        }
    };

    struct minus3_gf {
        template < typename T1, typename T2, typename T3 >
        constexpr auto operator()(T1&& v1, T2&& v2, T3&& v3) const {
            return v1 - v2 - v3;
        }
    };

    struct plusV_gf {
        template < typename A >
        constexpr decltype(auto) operator()(A&& a) const {
            return std::forward<A>(a);
        }

        template < typename A, typename B >
        constexpr decltype(auto) operator()(A&& a, B&& b) const {
            return std::forward<A>(a) + std::forward<B>(b);
        }

        template < typename A, typename B, typename... Cs >
        constexpr decltype(auto) operator()(A&& a, B&& b, Cs&&... cs) const {
            return (*this)(
                (*this)(std::forward<A>(a), std::forward<B>(b)),
                std::forward<Cs>(cs)...);
        }
    };

    constexpr auto f = kari::curry(minus2_gf{}, 10, 20);
}

namespace
{
    struct box {
        box(int v) : v_(v) {}
        ~box() {
            v_ = 100500;
        }

        box(box&& o) : v_(o.v_) {
            o.v_ = 100500;
            ++moveCount_;
        }

        box(const box& o) : v_(o.v_) {
            ++copyCount_;
        }

        box& operator=(box&& o) = delete;
        box& operator=(const box&) = delete;

        int v() const { return v_; }

        int addV(int add) {
            v_ += add;
            return v_;
        }

        static void resetCounters() {
            moveCount_ = 0;
            copyCount_ = 0;
        }

        static int moveCount() {
            return moveCount_;
        }

        static int copyCount() {
            return copyCount_;
        }
    private:
        int v_;
        static int moveCount_;
        static int copyCount_;
    };

    int box::moveCount_;
    int box::copyCount_;

    static box operator+(const box& lhs, const box& rhs) {
        return box(lhs.v() + rhs.v());
    }

    static box operator-(const box& lhs, const box& rhs) {
        return box(lhs.v() - rhs.v());
    }
}

namespace
{
    struct box_without_move {
        box_without_move(int v) : v_(v) {}
        ~box_without_move() {
            v_ = 100500;
        }

        box_without_move(const box_without_move& o) : v_(o.v_) {
            ++copyCount_;
        }

        box_without_move& operator=(const box_without_move&) = delete;

        int v() const { return v_; }

        int addV(int add) {
            v_ += add;
            return v_;
        }

        static void resetCounters() {
            copyCount_ = 0;
        }

        static int copyCount() {
            return copyCount_;
        }
    private:
        int v_;
        static int copyCount_;
    };

    int box_without_move::copyCount_;

    static box_without_move operator+(const box_without_move& lhs, const box_without_move& rhs) {
        return box_without_move(lhs.v() + rhs.v());
    }

    static box_without_move operator-(const box_without_move& lhs, const box_without_move& rhs) {
        return box_without_move(lhs.v() - rhs.v());
    }
}

using namespace kari;

TEST_CASE("kari_feature") {
    SECTION("underscore") {
        using namespace kari::underscore;
        REQUIRE((-_)(40) == -40);

        REQUIRE((_ + 40)(2) == 42);
        REQUIRE((_ - 2)(44) == 42);
        REQUIRE((_ * 21)(2) == 42);
        REQUIRE((_ / 2)(84) == 42);
        REQUIRE((_ % 100)(142) == 42);

        REQUIRE((_ == 42)(42));
        REQUIRE((_ != 42)(40));
        REQUIRE_FALSE((_ == 42)(40));
        REQUIRE_FALSE((_ != 42)(42));

        REQUIRE((40 + _)(2) == 42);
        REQUIRE((44 - _)(2) == 42);
        REQUIRE((21 * _)(2) == 42);
        REQUIRE((84 / _)(2) == 42);
        REQUIRE((142 % _)(100) == 42);

        REQUIRE((42 == _)(42));
        REQUIRE((42 != _)(40));
        REQUIRE_FALSE((42 == _)(40));
        REQUIRE_FALSE((42 != _)(42));

        REQUIRE((_ + _)(40,2) == 42);
        REQUIRE((_ - _)(44,2) == 42);
        REQUIRE((_ * _)(21,2) == 42);
        REQUIRE((_ / _)(84,2) == 42);
        REQUIRE((_ % _)(142,100) == 42);

        REQUIRE((_ == _)(42,42));
        REQUIRE((_ != _)(42,40));
        REQUIRE_FALSE((_ == _)(42,40));
        REQUIRE_FALSE((_ != _)(42,42));
    }
    SECTION("ref_functor") {
        REQUIRE(curry(minus3_gf())(1,2,3) == -4);
        {
            auto gf1 = id_gf();
            REQUIRE(curry(gf1)(1) == 1);
            auto gf2 = minus2_gf();
            REQUIRE(curry(gf2)(1,2) == -1);
            auto gf3 = minus3_gf();
            REQUIRE(curry(gf3)(1,2,3) == -4);
        }
        {
            const auto gf1 = id_gf();
            REQUIRE(curry(gf1)(1) == 1);
            const auto gf2 = minus2_gf();
            REQUIRE(curry(gf2)(1,2) == -1);
            const auto gf3 = minus3_gf();
            REQUIRE(curry(gf3)(1,2,3) == -4);
        }
    }
    SECTION("ref_forwarding") {
        {
            int i = 42;
            const int& g = curry([](const int& v, int){
                return std::ref(v);
            }, std::ref(i), 0);
            REQUIRE(&i == &g);
        }
        {
            int i = 42;
            const int& g = curry([](int& v, int){
                return std::ref(v);
            }, std::ref(i), 0);
            REQUIRE(&i == &g);
        }
    }
    SECTION("move_vs_copy") {
        {
            box::resetCounters();
            const auto b1 = box(1);
            const auto b2 = box(2);
            const auto b3 = box(3);
            const auto b4 = box(4);
            const auto b5 = box(5);
            curry([](auto&& v1, auto&& v2, auto&& v3, auto&& v4, auto&& v5){
                return v1 + v2 + v3 + v4 + v5;
            })(std::move(b1))(std::move(b2))(std::move(b3))(std::move(b4))(std::move(b5));
            REQUIRE(box::moveCount() == 25);
            REQUIRE(box::copyCount() == 5);
        }
        {
            box::resetCounters();
            auto b1 = box(1);
            auto b2 = box(2);
            auto b3 = box(3);
            auto b4 = box(4);
            auto b5 = box(5);
            curry([](auto&& v1, auto&& v2, auto&& v3, auto&& v4, auto&& v5){
                return v1 + v2 + v3 + v4 + v5;
            })(std::move(b1))(std::move(b2))(std::move(b3))(std::move(b4))(std::move(b5));
            REQUIRE(box::moveCount() == 30);
            REQUIRE(box::copyCount() == 0);
        }
        {
            box::resetCounters();
            curry([](auto&& v1, auto&& v2, auto&& v3, auto&& v4, auto&& v5){
                return v1 + v2 + v3 + v4 + v5;
            })(box(1))(box(2))(box(3))(box(4))(box(5));
            REQUIRE(box::moveCount() == 30);
            REQUIRE(box::copyCount() == 0);
        }
        {
            box::resetCounters();
            curry([](auto&& v1, auto&& v2, auto&& v3, auto&& v4, auto&& v5){
                return v1 + v2 + v3 + v4 + v5;
            })(box(1),box(2))(box(3),box(4))(box(5));
            REQUIRE(box::moveCount() == 30);
            REQUIRE(box::copyCount() == 0);
        }
        {
            box::resetCounters();
            curry([](auto&& v1, auto&& v2, auto&& v3, auto&& v4, auto&& v5){
                return v1 + v2 + v3 + v4 + v5;
            })(box(1),box(2),box(3),box(4),box(5));
            REQUIRE(box::moveCount() == 30);
            REQUIRE(box::copyCount() == 0);
        }
        {
            box::resetCounters();
            const auto c0 = curry([](auto&& v1, auto&& v2, auto&& v3, auto&& v4, auto&& v5){
                return v1 + v2 + v3 + v4 + v5;
            });
            const auto c1 = c0(box(1));
            const auto c2 = c1(box(2));
            const auto c3 = c2(box(3));
            const auto c4 = c3(box(4));
            const auto c5 = c4(box(5));
            REQUIRE(c5.v() == 15);
            REQUIRE(box::moveCount() == 30);
            REQUIRE(box::copyCount() == 10);
        }
        {
            box_without_move::resetCounters();
            const auto c0 = curry([](auto&& v1, auto&& v2, auto&& v3, auto&& v4, auto&& v5){
                return v1 + v2 + v3 + v4 + v5;
            });
            const auto c1 = c0(box_without_move(1));
            const auto c2 = c1(box_without_move(2));
            const auto c3 = c2(box_without_move(3));
            const auto c4 = c3(box_without_move(4));
            const auto c5 = c4(box_without_move(5));
            REQUIRE(c5.v() == 15);
            REQUIRE(box_without_move::copyCount() == 40);
        }
        {
            box::resetCounters();
            curryV(plusV_gf())(box(1),box(2),box(3),box(4),box(5))();
            REQUIRE(box::moveCount() == 35);
            REQUIRE(box::copyCount() == 0);
        }
        {
            box::resetCounters();
            const auto c0 = curryV(plusV_gf());
            const auto c1 = c0(box(1));
            const auto c2 = c1(box(2));
            const auto c3 = c2(box(3));
            const auto c4 = c3(box(4));
            const auto c5 = c4(box(5));
            REQUIRE(c5().v() == 15);
            REQUIRE(box::moveCount() == 35);
            REQUIRE(box::copyCount() == 15);
        }
    }
    SECTION("persistent") {
        auto c        = curry(minus3_gl);

        auto c10      = c(box(10));
        auto c10_2    = c10(box(2));
        auto c10_2_3  = c10_2(box(3));
        auto c10_2_3d = c10_2(box(3));
        REQUIRE(c10_2_3.v() == 5);
        REQUIRE(c10_2_3.v() == c10_2_3d.v());

        auto c9       = c(box(9));
        auto c9_1     = c9(box(1));
        auto c9_1_2   = c9_1(box(2));
        auto c9_1_2d  = c9_1(box(2));
        REQUIRE(c9_1_2.v() == 6);
        REQUIRE(c9_1_2.v() == c9_1_2d.v());

        auto c10_3    = c10(box(3));
        auto c10_3_5  = c10_3(box(5));
        auto c10_3_5d = c10_3(box(5));
        REQUIRE(c10_3_5.v() == 2);
        REQUIRE(c10_3_5.v() == c10_3_5d.v());
    }
}

TEST_CASE("kari") {
    SECTION("arity/min_arity") {
        REQUIRE(curry(minus3_gl).min_arity() == 0);
        REQUIRE(curryV(plusV_gf()).min_arity() == std::numeric_limits<std::size_t>::max());
        REQUIRE(curryN<3>(plusV_gf()).min_arity() == 3);
        REQUIRE(curryN<3>(plusV_gf())(1).min_arity() == 2);
        REQUIRE(curryN<3>(plusV_gf())(1)(2).min_arity() == 1);
    }
    SECTION("arity/recurring") {
        constexpr auto max_size_t = std::numeric_limits<std::size_t>::max();
        {
            REQUIRE(curry(curry(minus3_gl)).min_arity() == 0);
            REQUIRE(curry(curryV(plusV_gf())).min_arity() == max_size_t);
            REQUIRE(curry(curryN<3>(plusV_gf())).min_arity() == 3);
        }
        {
            REQUIRE(curryV(curry(minus3_gl)).min_arity() == max_size_t);
            REQUIRE(curryV(curryV(plusV_gf())).min_arity() == max_size_t);
            REQUIRE(curryV(curryN<3>(plusV_gf())).min_arity() == max_size_t);
        }
        {
            REQUIRE(curryN<2>(curry(minus3_gl)).min_arity() == 2);
            REQUIRE(curryN<2>(curryV(plusV_gf())).min_arity() == 2);
            REQUIRE(curryN<2>(curryN<3>(plusV_gf())).min_arity() == 2);
        }
    }
    SECTION("is_curried") {
        static_assert(!is_curried_v<void(int)>, "static unit test error");
        static_assert(!is_curried_v<decltype(minus2_gl)>, "static unit test error");
        static_assert(is_curried_v<decltype(curry(minus2_gl))>, "static unit test error");

        auto c = curry(minus3_gl);
        static_assert(is_curried_v<decltype(c)>, "static unit test error");

        auto c10 = c(box(10));
        static_assert(is_curried_v<decltype(c10)>, "static unit test error");

        const auto c10_2 = c10(box(2));
        static_assert(is_curried_v<decltype(c10_2)>, "static unit test error");

        auto c10_2_3 = c10_2(box(3));
        static_assert(!is_curried_v<decltype(c10_2_3)>, "static unit test error");

        REQUIRE(c10_2_3.v() == 5);
    }
    SECTION("typed_lambdas/simple") {
        REQUIRE(curry(_42_tl) == 42);
        REQUIRE(curry(id_tl)(42) == 42);
        REQUIRE(curry(minus2_tl)(8,4) == 4);
        REQUIRE(curry(minus3_tl)(8,5,2) == 1);
    }
    SECTION("typed_lambdas/one_by_one_calling") {
        REQUIRE(curry(id_tl)(42) == 42);
        REQUIRE(curry(minus2_tl)(8)(4) == 4);
        REQUIRE(curry(minus3_tl)(8)(5)(2) == 1);
    }
    SECTION("typed_lambdas/combined_calling") {
        REQUIRE(curry(minus3_tl)(8,5)(2) == 1);
        REQUIRE(curry(minus3_tl)(8)(5,2) == 1);

        REQUIRE(curry(minus4_tl)(14,2)(3)(4) == 5);
        REQUIRE(curry(minus4_tl)(14)(2,3)(4) == 5);
        REQUIRE(curry(minus4_tl)(14)(2)(3,4) == 5);
        REQUIRE(curry(minus4_tl)(14,2)(3,4) == 5);
        REQUIRE(curry(minus4_tl)(14,2,3)(4) == 5);
        REQUIRE(curry(minus4_tl)(14)(2,3,4) == 5);
    }
    SECTION("generic_lambdas/simple") {
        REQUIRE(curry(id_gl)(42) == 42);
        REQUIRE(curry(minus2_gl)(8,4) == 4);
        REQUIRE(curry(minus3_gl)(8,5,2) == 1);
    }
    SECTION("generic_lambdas/one_by_one_calling") {
        REQUIRE(curry(id_gl)(42) == 42);
        REQUIRE(curry(minus2_gl)(8)(4) == 4);
        REQUIRE(curry(minus3_gl)(8)(5)(2) == 1);
    }
    SECTION("generic_lambdas/combined_calling") {
        REQUIRE(curry(minus3_gl)(8,5)(2) == 1);
        REQUIRE(curry(minus3_gl)(8)(5,2) == 1);

        REQUIRE(curry(minus4_gl)(14,2)(3)(4) == 5);
        REQUIRE(curry(minus4_gl)(14)(2,3)(4) == 5);
        REQUIRE(curry(minus4_gl)(14)(2)(3,4) == 5);
        REQUIRE(curry(minus4_gl)(14,2)(3,4) == 5);
        REQUIRE(curry(minus4_gl)(14,2,3)(4) == 5);
        REQUIRE(curry(minus4_gl)(14)(2,3,4) == 5);
    }
    SECTION("nested_lambdas/full_apply") {
        {
            REQUIRE(curry([](){
                return _42_tl;
            })() == 42);
            //
            REQUIRE(curry([](){
                return curry(_42_tl);
            }) == 42);
        }
        {
            REQUIRE(curry([](){
                return id_gl;
            })(9) == 9);
            //
            REQUIRE(curry([](){
                return curry(id_gl);
            })(9) == 9);
        }
        {
            REQUIRE(curry([](){
                return minus2_gl;
            })(9,5) == 4);
            //
            REQUIRE(curry([](){
                return curry(minus2_gl);
            })(9,5) == 4);
            REQUIRE(curry([](){
                return curry(minus2_gl);
            })(9)(5) == 4);
            //
            REQUIRE(curry([](){
                return curry(minus3_gl);
            })(9,4,3) == 2);
            REQUIRE(curry([](){
                return curry(minus3_gl);
            })(9)(4)(3) == 2);
            REQUIRE(curry([](){
                return curry(minus3_gl);
            })(9,4)(3) == 2);
            REQUIRE(curry([](){
                return curry(minus3_gl);
            })(9)(4,3) == 2);
        }
    }
    SECTION("variadic_functions") {
        {
            const auto c = curry(plusV_gf());
            REQUIRE(c(15) == 15);
            REQUIRE(curry(plusV_gf(), 6) == 6);
        }
        {
            const auto c = curryV(plusV_gf());
            REQUIRE(c(1,2,3,4,5)() == 15);
            REQUIRE(curryV(plusV_gf(), 1, 2, 3)() == 6);
        }
        {
            const auto c = curryN<3>(plusV_gf());
            REQUIRE(c(1,2,3) == 6);
            REQUIRE(curryN<0>(plusV_gf(), 1, 2, 3) == 6);
        }
        {
            char buffer[256] = {'\0'};
            auto c = kari::curryV(std::snprintf, buffer, 256, "%d + %d = %d");
            c(37, 5, 42)();
            REQUIRE(std::strcmp("37 + 5 = 42", buffer) == 0);
        }
        {
            char buffer[256] = {'\0'};
            auto c = kari::curryN<3>(std::snprintf, buffer, 256, "%d + %d = %d");
            c(37, 5, 42);
            REQUIRE(std::strcmp("37 + 5 = 42", buffer) == 0);
        }
    }
    SECTION("variadic_functions/recurring") {
        {
            auto c0 = curry(curry(plusV_gf()));
            auto c1 = curry(curryV(plusV_gf()));
            auto c2 = curry(curryN<3>(plusV_gf()));
            REQUIRE(c0(42) == 42);
            REQUIRE(c1(40,2)() == 42);
            REQUIRE(c2(37,2,3) == 42);
        }
        {
            auto c0 = curryV(curry(plusV_gf()));
            auto c1 = curryV(curryV(plusV_gf()));
            auto c2 = curryV(curryN<3>(plusV_gf()));
            REQUIRE(c0(40,2)() == 42);
            REQUIRE(c1(40,2)() == 42);
            REQUIRE(c2(40,2)() == 42);
        }
        {
            auto c0 = curryN<2>(curry(plusV_gf()));
            auto c1 = curryN<2>(curryV(plusV_gf()));
            auto c2 = curryN<2>(curryN<3>(plusV_gf()));
            auto c3 = curryN<4>(curryN<3>(plusV_gf()));
            REQUIRE(c0(40,2) == 42);
            REQUIRE(c1(40,2) == 42);
            REQUIRE(c2(40,2) == 42);
            REQUIRE(c3(20,15,5,2) == 42);
        }
        {
            auto c0 = curry(plusV_gf());
            auto c1 = curryN<2>(c0);
            REQUIRE(c1(40,2) == 42);
        }
    }
    SECTION("member_functions") {
        {
            auto c0 = curry(&box::addV);
            auto c1 = curry(&box::v);
            REQUIRE(c0(box(10), 2) == 12);
            REQUIRE(c1(box(12)) == 12);
        }
        {
            auto c0 = curry(&box::addV);
            auto c1 = curry(&box::v);

            auto b1 = box(10);
            const auto b2 = box(10);

            REQUIRE(c0(std::ref(b1), 2) == 12);
            REQUIRE(c1(std::ref(b2)) == 10);

            REQUIRE(c0(b1, 2) == 14);
            REQUIRE(c1(b2) == 10);

            REQUIRE(c0(&b1, 2) == 14);
            REQUIRE(c1(&b2) == 10);
        }
    }
    SECTION("member_objects") {
        struct box2 : box {
            box2(int v) : box(v), ov(v) {}
            int ov;
        };
        auto c = curry(&box2::ov);
        auto b1 = box2(10);
        const auto b2 = box2(10);
        REQUIRE(c(box2(10)) == 10);
        REQUIRE(c(b1) == 10);
        REQUIRE(c(b2) == 10);
        REQUIRE(c(std::ref(b1)) == 10);
        REQUIRE(c(std::ref(b2)) == 10);
    }
}

TEST_CASE("kari_helpers") {
    SECTION("fid") {
        REQUIRE(fid(box(10)).v() == 10);
    }
    SECTION("fconst") {
        REQUIRE(fconst(box(10), 20).v() == 10);
        {
            auto b10 = fconst(box(10));
            REQUIRE(b10(20).v() == 10);
            REQUIRE(std::move(b10)(30).v() == 10);
            REQUIRE(b10(20).v() == 100500);
        }
    }
    SECTION("fflip") {
        REQUIRE(fflip(curry(std::minus<>()))(10, 20) == 10);
        REQUIRE(fflip(minus3_gl)(10,20,50) == -40);
    }
    SECTION("fpipe") {
        using namespace kari::underscore;
        REQUIRE(fpipe(_+2, _*2, 4) == 12);
        REQUIRE(((_+2) | (_*2) | 4) == 12);
        REQUIRE((4 | (_+2) | (_*2)) == 12);
    }
    SECTION("fcompose") {
        using namespace kari::underscore;
        REQUIRE(fcompose(_+2, _*2, 4) == 10);
        REQUIRE((_+2) * (_*2) * 4 == 10);
        REQUIRE(4 * (_+2) * (_*2) == 12);
        {
            const auto s3 = [](int v1, int v2, int v3){
                return v1 + v2 + v3;
            };
            const auto c = curry(s3) * (_*2) * 10 * 20 * 30;
            REQUIRE(c == 70);
        }
        {
            // (. (+2)) (*2) $ 10 == 24
            int i = fflip(fcompose)(_+2, _*2, 10);
            int j = (_*(_+2))(_*2)(10);
            REQUIRE(i == 24);
            REQUIRE(j == 24);
        }
        {
            // ((+2) .) (*2) $ 10 == 24
            int i = fcompose(_+2)(_*2, 10);
            int j = ((_+2) * _)(_*2)(10);
            REQUIRE(i == 22);
            REQUIRE(j == 22);
        }
    }
}

namespace kari_regression {
    namespace change_type_after_applying {
        template < typename C >
        struct first_type_impl;

        template < std::size_t N, typename F, typename A, typename... Args >
        struct first_type_impl<curry_t<N, F, A, Args...>> {
            using type = A;
        };

        template < typename C >
        struct first_type
        : first_type_impl<std::remove_cv_t<std::remove_reference_t<C>>> {};
    }
}

TEST_CASE("kari_regression") {
    SECTION("change_type_after_applying") {
        using namespace kari_regression::change_type_after_applying;
        const auto f3 = [](int& v1, int v2, int v3){
            return v1 + v2 + v3;
        };
        int i = 0;
        auto c0 = curry(f3, 0);
        auto c1 = c0(std::ref(i));
        auto c2 = std::move(c0)(std::ref(i));
        static_assert(std::is_same<
            typename first_type<decltype(c1)>::type,
            typename first_type<decltype(c2)>::type
        >::value,"static unit test error");
    }
    SECTION("curryN_already_curried_function") {
        auto c = curryN<3>(plusV_gf());
        auto c2 = curryN<3>(c);
        REQUIRE(c2(1,2,3) == 6);
    }
}
