/*******************************************************************************
 * This file is part of the "https://github.com/BlackMATov/kari.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2017-2020, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace kari_hpp
{
    template < std::size_t N, typename F, typename... Args >
    struct curry_t;

    namespace detail
    {
        template < std::size_t N, typename F, typename... Args >
        constexpr auto make_curry(F&& f, std::tuple<Args...>&& args) {
            if constexpr ( !N && std::is_invocable_v<std::decay_t<F>, Args...> ) {
                return std::apply(std::forward<F>(f), std::move(args));
            } else {
                return curry_t<N, std::decay_t<F>, Args...>(std::forward<F>(f), std::move(args));
            }
        }

        template < std::size_t N, typename F >
        constexpr auto make_curry(F&& f) {
            return make_curry<N>(std::forward<F>(f), std::make_tuple());
        }
    }

    template < std::size_t N, typename F, typename... Args >
    struct curry_t final {
        template < typename U >
        constexpr curry_t(U&& u, std::tuple<Args...>&& args)
        noexcept(noexcept(F(std::forward<U>(u))) && noexcept(std::tuple<Args...>(std::move(args))))
        : f_(std::forward<U>(u))
        , args_(std::move(args)) {}

        // min_arity

        constexpr std::size_t min_arity() const noexcept {
            return N;
        }

        // recurry

        template < std::size_t M >
        constexpr decltype(auto) recurry() &&
            noexcept(noexcept(
                detail::make_curry<M>(
                    std::move(std::declval<F>()),
                    std::move(std::declval<std::tuple<Args...>>()))))
        {
            return detail::make_curry<M>(
                std::move(f_),
                std::move(args_));
        }

        template < std::size_t M >
        constexpr decltype(auto) recurry() const &
            noexcept(noexcept(
                std::move(std::declval<curry_t>()).template recurry<M>()))
        {
            return std::move(curry_t(*this)).template recurry<M>();
        }

        // operator(As&&...)

        constexpr decltype(auto) operator()() &&
            noexcept(noexcept(
                std::move(std::declval<curry_t>()).template recurry<0>()))
        {
            return std::move(*this).template recurry<0>();
        }

        template < typename A >
        constexpr decltype(auto) operator()(A&& a) &&
            noexcept(noexcept(
                detail::make_curry<(N > 0 ? N - 1 : 0)>(
                std::move(std::declval<F>()),
                std::tuple_cat(
                    std::move(std::declval<std::tuple<Args...>>()),
                    std::make_tuple(std::forward<A>(a))))))
        {
            return detail::make_curry<(N > 0 ? N - 1 : 0)>(
                std::move(f_),
                std::tuple_cat(
                    std::move(args_),
                    std::make_tuple(std::forward<A>(a))));
        }

        template < typename A, typename... As >
        constexpr decltype(auto) operator()(A&& a, As&&... as) &&
            noexcept(noexcept(
                std::move(std::declval<curry_t>())(std::forward<A>(a))(std::forward<As>(as)...)))
        {
            return std::move(*this)(std::forward<A>(a))(std::forward<As>(as)...);
        }

        template < typename... As >
        constexpr decltype(auto) operator()(As&&... as) const &
            noexcept(noexcept(
                std::move(std::declval<curry_t>())(std::forward<As>(as)...)))
        {
            return std::move(curry_t(*this))(std::forward<As>(as)...);
        }
    private:
        F f_;
        std::tuple<Args...> args_;
    };

    //
    // is_curried, is_curried_v
    //

    namespace detail
    {
        template < typename F >
        struct is_curried_impl
        : std::false_type {};

        template < std::size_t N, typename F, typename... Args >
        struct is_curried_impl<curry_t<N, F, Args...>>
        : std::true_type {};
    }

    template < typename F >
    struct is_curried
    : detail::is_curried_impl<std::remove_cv_t<F>> {};

    template < typename F >
    inline constexpr bool is_curried_v = is_curried<F>::value;

    //
    // curry
    //

    template < typename F >
    constexpr auto curry(F&& f) {
        if constexpr ( is_curried_v<std::decay_t<F>> ) {
            return std::forward<F>(f);
        } else {
            return detail::make_curry<0>(std::forward<F>(f));
        }
    }

    template < typename F, typename A, typename... As >
    constexpr auto curry(F&& f, A&& a, As&&... as) {
        return curry(std::forward<F>(f))(std::forward<A>(a), std::forward<As>(as)...);
    }

    //
    // curryV
    //

    template < typename F >
    constexpr auto curryV(F&& f) {
        constexpr std::size_t max_n = std::size_t(-1);
        if constexpr ( is_curried_v<std::decay_t<F>> ) {
            return std::forward<F>(f).template recurry<max_n>();
        } else {
            return detail::make_curry<max_n>(std::forward<F>(f));
        }
    }

    template < typename F, typename A, typename... As >
    constexpr auto curryV(F&& f, A&& a, As&&... as) {
        return curryV(std::forward<F>(f))(std::forward<A>(a), std::forward<As>(as)...);
    }

    //
    // curryN
    //

    template < std::size_t N, typename F >
    constexpr auto curryN(F&& f) {
        if constexpr ( is_curried_v<std::decay_t<F>> ) {
            return std::forward<F>(f).template recurry<N>();
        } else {
            return detail::make_curry<N>(std::forward<F>(f));
        }
    }

    template < std::size_t N, typename F, typename A, typename... As >
    constexpr auto curryN(F&& f, A&& a, As&&... as) {
        constexpr std::size_t max_n = std::size_t(-1);
        constexpr std::size_t arg_n = sizeof...(As) + 1;
        static_assert(max_n - arg_n >= N, "invalid N argument");
        return curryN<N + arg_n>(std::forward<F>(f))(std::forward<A>(a), std::forward<As>(as)...);
    }
}
