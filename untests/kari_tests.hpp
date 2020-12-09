/*******************************************************************************
 * This file is part of the "https://github.com/BlackMATov/kari.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2017-2020, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

namespace kari_tests
{
    inline constexpr auto _42_tl = []() {
        return 42;
    };

    inline constexpr auto id_tl = [](int v) {
        return v;
    };

    inline constexpr auto minus2_tl = [](int v1, int v2) {
        return v1 - v2;
    };

    inline constexpr auto minus3_tl = [](int v1, int v2, int v3) {
        return v1 - v2 - v3;
    };

    inline constexpr auto minus4_tl = [](int v1, int v2, int v3, int v4) {
        return v1 - v2 - v3 - v4;
    };

    inline constexpr auto id_gl = [](auto&& v) {
        return v;
    };

    inline constexpr auto minus2_gl = [](auto&& v1, auto&& v2) {
        return v1 - v2;
    };

    inline constexpr auto minus3_gl = [](auto&& v1, auto&& v2, auto&& v3) {
        return v1 - v2 - v3;
    };

    inline constexpr auto minus4_gl = [](auto&& v1, auto&& v2, auto&& v3, auto&& v4) {
        return v1 - v2 - v3 - v4;
    };
}

namespace kari_tests
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
}

namespace kari_tests
{
    template < typename = void >
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

    template < typename T >
    int box<T>::moveCount_;

    template < typename T >
    int box<T>::copyCount_;

    template < typename T >
    inline box<T> operator+(const box<T>& lhs, const box<T>& rhs) {
        return box(lhs.v() + rhs.v());
    }

    template < typename T >
    inline box<T> operator-(const box<T>& lhs, const box<T>& rhs) {
        return box(lhs.v() - rhs.v());
    }
}

namespace kari_tests
{
    template < typename = void >
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

    template < typename T >
    int box_without_move<T>::copyCount_;

    template < typename T >
    inline box_without_move<T> operator+(const box_without_move<T>& lhs, const box_without_move<T>& rhs) {
        return box_without_move(lhs.v() + rhs.v());
    }
}
