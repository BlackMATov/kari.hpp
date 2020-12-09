/*******************************************************************************
 * This file is part of the "https://github.com/BlackMATov/kari.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2017-2020, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "kari.hpp"

namespace kari_hpp
{
    //
    // fid
    //

    struct fid_t {
        template < typename A >
        constexpr auto operator()(A&& a) const {
            return std::forward<A>(a);
        }
    };
    inline constexpr auto fid = curry(fid_t{});

    //
    // fconst
    //

    struct fconst_t {
        template < typename A, typename B >
        constexpr auto operator()(A&& a, B&&) const {
            return std::forward<A>(a);
        }
    };
    inline constexpr auto fconst = curry(fconst_t{});

    //
    // fflip
    //

    struct fflip_t {
        template < typename F, typename A, typename B >
        constexpr auto operator()(F&& f, A&& a, B&& b) const {
            return curry(
                std::forward<F>(f),
                std::forward<B>(b),
                std::forward<A>(a));
        }
    };
    inline constexpr auto fflip = curry(fflip_t{});

    //
    // fpipe
    //

    struct fpipe_t {
        template < typename G, typename F, typename A >
        constexpr auto operator()(G&& g, F&& f, A&& a) const {
            return curry(
                std::forward<F>(f),
                curry(
                    std::forward<G>(g),
                    std::forward<A>(a)));
        }
    };
    inline constexpr auto fpipe = curry(fpipe_t{});

    //
    // fcompose
    //

    struct fcompose_t {
        template < typename G, typename F, typename A >
        constexpr auto operator()(G&& g, F&& f, A&& a) const {
            return curry(
                std::forward<G>(g),
                curry(
                    std::forward<F>(f),
                    std::forward<A>(a)));
        }
    };
    inline constexpr auto fcompose = curry(fcompose_t{});

    //
    // fpipe operators
    //

    template < typename G, typename F
             , std::enable_if_t<is_curried_v<std::decay_t<G>>, int> = 0
             , std::enable_if_t<is_curried_v<std::decay_t<F>>, int> = 0 >
    constexpr auto operator|(G&& g, F&& f) {
        return fpipe(
            std::forward<G>(g),
            std::forward<F>(f));
    }

    template < typename F, typename A
             , std::enable_if_t< is_curried_v<std::decay_t<F>>, int> = 0
             , std::enable_if_t<!is_curried_v<std::decay_t<A>>, int> = 0 >
    constexpr auto operator|(F&& f, A&& a) {
        return std::forward<F>(f)(std::forward<A>(a));
    }

    template < typename A, typename F
             , std::enable_if_t<!is_curried_v<std::decay_t<A>>, int> = 0
             , std::enable_if_t< is_curried_v<std::decay_t<F>>, int> = 0 >
    constexpr auto operator|(A&& a, F&& f) {
        return std::forward<F>(f)(std::forward<A>(a));
    }

    //
    // fcompose operators
    //

    template < typename G, typename F
             , std::enable_if_t<is_curried_v<std::decay_t<G>>, int> = 0
             , std::enable_if_t<is_curried_v<std::decay_t<F>>, int> = 0 >
    constexpr auto operator*(G&& g, F&& f) {
        return fcompose(
            std::forward<G>(g),
            std::forward<F>(f));
    }

    template < typename F, typename A
             , std::enable_if_t< is_curried_v<std::decay_t<F>>, int> = 0
             , std::enable_if_t<!is_curried_v<std::decay_t<A>>, int> = 0 >
    constexpr auto operator*(F&& f, A&& a) {
        return std::forward<F>(f)(std::forward<A>(a));
    }

    template < typename A, typename F
             , std::enable_if_t<!is_curried_v<std::decay_t<A>>, int> = 0
             , std::enable_if_t< is_curried_v<std::decay_t<F>>, int> = 0 >
    constexpr auto operator*(A&& a, F&& f) {
        return std::forward<F>(f)(std::forward<A>(a));
    }
}

namespace kari_hpp::underscore
{
    struct us_t {};
    inline constexpr us_t _{};

    //
    // is_underscore, is_underscore_v
    //

    template < typename T >
    struct is_underscore
    : std::bool_constant<std::is_same_v<us_t, std::remove_cv_t<T>>> {};

    template < typename T >
    inline constexpr bool is_underscore_v = is_underscore<T>::value;

    //
    // unary operators
    //

    #define KARI_HPP_DEFINE_UNDERSCORE_UNARY_OP(op, func)\
        constexpr auto operator op (us_t) {\
            return curry(func);\
        }

        KARI_HPP_DEFINE_UNDERSCORE_UNARY_OP(-, std::negate<>())
        KARI_HPP_DEFINE_UNDERSCORE_UNARY_OP(~, std::bit_not<>())
        KARI_HPP_DEFINE_UNDERSCORE_UNARY_OP(!, std::logical_not<>())
    #undef KARI_HPP_DEFINE_UNDERSCORE_UNARY_OP

    //
    // binary operators
    //

    #define KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(op, func)\
        constexpr auto operator op (us_t, us_t) {\
            return curry(func);\
        }\
        \
        template < typename A, std::enable_if_t<!is_underscore_v<std::decay_t<A>>, int> = 0 >\
        constexpr auto operator op (A&& a, us_t) {\
            return curry(func, std::forward<A>(a));\
        }\
        \
        template < typename B, std::enable_if_t<!is_underscore_v<std::decay_t<B>>, int> = 0 >\
        constexpr auto operator op (us_t, B&& b) {\
            return fflip(func, std::forward<B>(b));\
        }

        KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(+ , std::plus<>())
        KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(- , std::minus<>())
        KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(* , std::multiplies<>())
        KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(/ , std::divides<>())
        KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(% , std::modulus<>())

        KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(< , std::less<>())
        KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(> , std::greater<>())
        KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(<=, std::less_equal<>())
        KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(>=, std::greater_equal<>())

        KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(==, std::equal_to<>())
        KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(!=, std::not_equal_to<>())

        KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(| , std::bit_or<>())
        KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(& , std::bit_and<>())
        KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(^ , std::bit_xor<>())

        KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(||, std::logical_or<>())
        KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(&&, std::logical_and<>())
    #undef KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP
}
