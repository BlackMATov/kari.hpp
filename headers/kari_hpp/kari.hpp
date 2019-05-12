/*******************************************************************************
 * This file is part of the "https://github.com/BlackMATov/kari.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2017-2019, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include <tuple>
#include <limits>
#include <utility>
#include <functional>
#include <type_traits>

// -----------------------------------------------------------------------------
//
// https://github.com/BlackMATov/invoke.hpp
//
// -----------------------------------------------------------------------------

#define INVOKE_HPP_NOEXCEPT_RETURN(...) \
    noexcept(noexcept(__VA_ARGS__)) { return __VA_ARGS__; }

#define INVOKE_HPP_NOEXCEPT_DECLTYPE_RETURN(...) \
    noexcept(noexcept(__VA_ARGS__)) -> decltype (__VA_ARGS__) { return __VA_ARGS__; }

//
// void_t
//

namespace invoke_hpp
{
    namespace impl
    {
        template < typename... Args >
        struct make_void {
            using type = void;
        };
    }

    template < typename... Args >
    using void_t = typename impl::make_void<Args...>::type;
}

//
// is_reference_wrapper
//

namespace invoke_hpp
{
    namespace impl
    {
        template < typename T >
        struct is_reference_wrapper_impl
        : std::false_type {};

        template < typename U >
        struct is_reference_wrapper_impl<std::reference_wrapper<U>>
        : std::true_type {};
    }

    template < typename T >
    struct is_reference_wrapper
    : impl::is_reference_wrapper_impl<std::remove_cv_t<T>> {};
}

//
// invoke
//

namespace invoke_hpp
{
    namespace impl
    {
        //
        // invoke_member_object_impl
        //

        template
        <
            typename Base, typename F, typename Derived,
            typename std::enable_if_t<std::is_base_of<Base, std::decay_t<Derived>>::value, int> = 0
        >
        constexpr auto invoke_member_object_impl(F Base::* f, Derived&& ref)
        INVOKE_HPP_NOEXCEPT_DECLTYPE_RETURN(
            std::forward<Derived>(ref).*f)

        template
        <
            typename Base, typename F, typename RefWrap,
            typename std::enable_if_t<is_reference_wrapper<std::decay_t<RefWrap>>::value, int> = 0
        >
        constexpr auto invoke_member_object_impl(F Base::* f, RefWrap&& ref)
        INVOKE_HPP_NOEXCEPT_DECLTYPE_RETURN(
            ref.get().*f)

        template
        <
            typename Base, typename F, typename Pointer,
            typename std::enable_if_t<
                !std::is_base_of<Base, std::decay_t<Pointer>>::value &&
                !is_reference_wrapper<std::decay_t<Pointer>>::value
            , int> = 0
        >
        constexpr auto invoke_member_object_impl(F Base::* f, Pointer&& ptr)
        INVOKE_HPP_NOEXCEPT_DECLTYPE_RETURN(
            (*std::forward<Pointer>(ptr)).*f)

        //
        // invoke_member_function_impl
        //

        template
        <
            typename Base, typename F, typename Derived, typename... Args,
            typename std::enable_if_t<std::is_base_of<Base, std::decay_t<Derived>>::value, int> = 0
        >
        constexpr auto invoke_member_function_impl(F Base::* f, Derived&& ref, Args&&... args)
        INVOKE_HPP_NOEXCEPT_DECLTYPE_RETURN(
            (std::forward<Derived>(ref).*f)(std::forward<Args>(args)...))

        template
        <
            typename Base, typename F, typename RefWrap, typename... Args,
            typename std::enable_if_t<is_reference_wrapper<std::decay_t<RefWrap>>::value, int> = 0
        >
        constexpr auto invoke_member_function_impl(F Base::* f, RefWrap&& ref, Args&&... args)
        INVOKE_HPP_NOEXCEPT_DECLTYPE_RETURN(
            (ref.get().*f)(std::forward<Args>(args)...))

        template
        <
            typename Base, typename F, typename Pointer, typename... Args,
            typename std::enable_if_t<
                !std::is_base_of<Base, std::decay_t<Pointer>>::value &&
                !is_reference_wrapper<std::decay_t<Pointer>>::value
            , int> = 0
        >
        constexpr auto invoke_member_function_impl(F Base::* f, Pointer&& ptr, Args&&... args)
        INVOKE_HPP_NOEXCEPT_DECLTYPE_RETURN(
            ((*std::forward<Pointer>(ptr)).*f)(std::forward<Args>(args)...))
    }

    template
    <
        typename F, typename... Args,
        typename std::enable_if_t<!std::is_member_pointer<std::decay_t<F>>::value, int> = 0
    >
    constexpr auto invoke(F&& f, Args&&... args)
    INVOKE_HPP_NOEXCEPT_DECLTYPE_RETURN(
        std::forward<F>(f)(std::forward<Args>(args)...))

    template
    <
        typename F, typename T,
        typename std::enable_if_t<std::is_member_object_pointer<std::decay_t<F>>::value, int> = 0
    >
    constexpr auto invoke(F&& f, T&& t)
    INVOKE_HPP_NOEXCEPT_DECLTYPE_RETURN(
        impl::invoke_member_object_impl(std::forward<F>(f), std::forward<T>(t)))

    template
    <
        typename F, typename... Args,
        typename std::enable_if_t<std::is_member_function_pointer<std::decay_t<F>>::value, int> = 0
    >
    constexpr auto invoke(F&& f, Args&&... args)
    INVOKE_HPP_NOEXCEPT_DECLTYPE_RETURN(
        impl::invoke_member_function_impl(std::forward<F>(f), std::forward<Args>(args)...))
}

//
// invoke_result
//

namespace invoke_hpp
{
    namespace impl
    {
        struct invoke_result_impl_tag {};

        template < typename Void, typename F, typename... Args >
        struct invoke_result_impl {};

        template < typename F, typename... Args >
        struct invoke_result_impl<void_t<invoke_result_impl_tag, decltype(invoke_hpp::invoke(std::declval<F>(), std::declval<Args>()...))>, F, Args...> {
            using type = decltype(invoke_hpp::invoke(std::declval<F>(), std::declval<Args>()...));
        };
    }

    template < typename F, typename... Args >
    struct invoke_result
    : impl::invoke_result_impl<void, F, Args...> {};

    template < typename F, typename... Args >
    using invoke_result_t = typename invoke_result<F, Args...>::type;
}

//
// is_invocable
//

namespace invoke_hpp
{
    namespace impl
    {
        struct is_invocable_r_impl_tag {};

        template < typename Void, typename R, typename F, typename... Args >
        struct is_invocable_r_impl
        : std::false_type {};

        template < typename R, typename F, typename... Args >
        struct is_invocable_r_impl<void_t<is_invocable_r_impl_tag, invoke_result_t<F, Args...>>, R, F, Args...>
        : std::conditional_t<
            std::is_void<R>::value,
            std::true_type,
            std::is_convertible<invoke_result_t<F, Args...>, R>> {};
    }

    template < typename R, typename F, typename... Args >
    struct is_invocable_r
    : impl::is_invocable_r_impl<void, R, F, Args...> {};

    template < typename F, typename... Args >
    using is_invocable = is_invocable_r<void, F, Args...>;
}

//
// apply
//

namespace invoke_hpp
{
    namespace impl
    {
        template < typename F, typename Tuple, std::size_t... I >
        constexpr decltype(auto) apply_impl(F&& f, Tuple&& args, std::index_sequence<I...>)
        INVOKE_HPP_NOEXCEPT_RETURN(
            invoke_hpp::invoke(
                std::forward<F>(f),
                std::get<I>(std::forward<Tuple>(args))...))
    }

    template < typename F, typename Tuple >
    constexpr decltype(auto) apply(F&& f, Tuple&& args)
    INVOKE_HPP_NOEXCEPT_RETURN(
        impl::apply_impl(
            std::forward<F>(f),
            std::forward<Tuple>(args),
            std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>()))
}

#undef INVOKE_HPP_NOEXCEPT_RETURN
#undef INVOKE_HPP_NOEXCEPT_DECLTYPE_RETURN

// -----------------------------------------------------------------------------
//
// https://github.com/BlackMATov/kari.hpp
//
// -----------------------------------------------------------------------------

#define KARI_HPP_NOEXCEPT_RETURN(...) \
    noexcept(noexcept(__VA_ARGS__)) { return __VA_ARGS__; }

#define KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(...) \
    noexcept(noexcept(__VA_ARGS__)) -> decltype (__VA_ARGS__) { return __VA_ARGS__; }

namespace kari
{
    template < std::size_t N, typename F, typename... Args >
    struct curry_t;

    namespace detail
    {
        template
        <
            std::size_t N, typename F, typename... Args,
            typename std::enable_if_t<
                (N == 0) &&
                invoke_hpp::is_invocable<std::decay_t<F>, Args...>::value
            , int> = 0
        >
        constexpr auto make_curry(F&& f, std::tuple<Args...>&& args)
        KARI_HPP_NOEXCEPT_RETURN(
            invoke_hpp::apply(std::forward<F>(f), std::move(args)))

        template
        <
            std::size_t N, typename F, typename... Args,
            typename std::enable_if_t<
                (N > 0) ||
                !invoke_hpp::is_invocable<std::decay_t<F>, Args...>::value
            , int> = 0
        >
        constexpr auto make_curry(F&& f, std::tuple<Args...>&& args)
        KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
            curry_t<
                N,
                std::decay_t<F>,
                Args...
            >(std::forward<F>(f), std::move(args)))

        template < std::size_t N, typename F >
        constexpr auto make_curry(F&& f)
        KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
            make_curry<N>(std::forward<F>(f), std::make_tuple()))
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
    constexpr bool is_curried_v = is_curried<F>::value;

    //
    // curry
    //

    template
    <
        typename F,
        typename std::enable_if_t<is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr auto curry(F&& f)
    KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
        std::forward<F>(f))

    template
    <
        typename F,
        typename std::enable_if_t<!is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr auto curry(F&& f)
    KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
        detail::make_curry<0>(std::forward<F>(f)))

    template < typename F, typename A, typename... As >
    constexpr auto curry(F&& f, A&& a, As&&... as)
    KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
        curry(std::forward<F>(f))(std::forward<A>(a), std::forward<As>(as)...))

    //
    // curryV
    //

    template
    <
        typename F,
        std::size_t MaxN = std::numeric_limits<std::size_t>::max(),
        typename std::enable_if_t<is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr auto curryV(F&& f)
    KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
        std::forward<F>(f).template recurry<MaxN>())

    template
    <
        typename F,
        std::size_t MaxN = std::numeric_limits<std::size_t>::max(),
        typename std::enable_if_t<!is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr auto curryV(F&& f)
    KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
        detail::make_curry<MaxN>(std::forward<F>(f)))

    template < typename F, typename A, typename... As >
    constexpr auto curryV(F&& f, A&& a, As&&... as)
    KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
        curryV(std::forward<F>(f))(std::forward<A>(a), std::forward<As>(as)...))

    //
    // curryN
    //

    template
    <
        std::size_t N, typename F,
        typename std::enable_if_t<is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr auto curryN(F&& f)
    KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
        std::forward<F>(f).template recurry<N>())

    template
    <
        std::size_t N, typename F,
        typename std::enable_if_t<!is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr auto curryN(F&& f)
    KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
        detail::make_curry<N>(std::forward<F>(f)))

    template
    <
        std::size_t N, typename F, typename A, typename... As,
        std::size_t Args = sizeof...(As) + 1,
        std::size_t MaxN = std::numeric_limits<std::size_t>::max(),
        typename std::enable_if_t<(MaxN - Args >= N), int> = 0
    >
    constexpr auto curryN(F&& f, A&& a, As&&... as)
    KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
        curryN<N + Args>(std::forward<F>(f))(std::forward<A>(a), std::forward<As>(as)...))
}

namespace kari
{
    //
    // fid
    //

    struct fid_t {
        template < typename A >
        constexpr auto operator()(A&& a) const
        KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
            std::forward<A>(a))
    };
    constexpr auto fid = curry(fid_t{});

    //
    // fconst
    //

    struct fconst_t {
        template < typename A, typename B >
        constexpr auto operator()(A&& a, B&&) const
        KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
            std::forward<A>(a))
    };
    constexpr auto fconst = curry(fconst_t{});

    //
    // fflip
    //

    struct fflip_t {
        template < typename F, typename A, typename B >
        constexpr auto operator()(F&& f, A&& a, B&& b) const
        KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
            curry(
                std::forward<F>(f),
                std::forward<B>(b),
                std::forward<A>(a)))
    };
    constexpr auto fflip = curry(fflip_t{});

    //
    // fpipe
    //

    struct fpipe_t {
        template < typename G, typename F, typename A >
        constexpr auto operator()(G&& g, F&& f, A&& a) const
        KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
            curry(
                std::forward<F>(f),
                curry(
                    std::forward<G>(g),
                    std::forward<A>(a))))
    };
    constexpr auto fpipe = curry(fpipe_t{});

    //
    // fcompose
    //

    struct fcompose_t {
        template < typename G, typename F, typename A >
        constexpr auto operator()(G&& g, F&& f, A&& a) const
        KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
            curry(
                std::forward<G>(g),
                curry(
                    std::forward<F>(f),
                    std::forward<A>(a))))
    };
    constexpr auto fcompose = curry(fcompose_t{});

    //
    // fpipe operators
    //

    template
    <
        typename G, typename F,
        typename std::enable_if_t<is_curried_v<std::decay_t<G>>, int> = 0,
        typename std::enable_if_t<is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr auto operator|(G&& g, F&& f)
    KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
        fpipe(
            std::forward<G>(g),
            std::forward<F>(f)))

    template
    <
        typename F, typename A,
        typename std::enable_if_t< is_curried_v<std::decay_t<F>>, int> = 0,
        typename std::enable_if_t<!is_curried_v<std::decay_t<A>>, int> = 0
    >
    constexpr auto operator|(F&& f, A&& a)
    KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
        std::forward<F>(f)(std::forward<A>(a)))

    template
    <
        typename A, typename F,
        typename std::enable_if_t<!is_curried_v<std::decay_t<A>>, int> = 0,
        typename std::enable_if_t< is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr auto operator|(A&& a, F&& f)
    KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
        std::forward<F>(f)(std::forward<A>(a)))

    //
    // fcompose operators
    //

    template
    <
        typename G, typename F,
        typename std::enable_if_t<is_curried_v<std::decay_t<G>>, int> = 0,
        typename std::enable_if_t<is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr auto operator*(G&& g, F&& f)
    KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
        fcompose(
            std::forward<G>(g),
            std::forward<F>(f)))

    template
    <
        typename F, typename A,
        typename std::enable_if_t< is_curried_v<std::decay_t<F>>, int> = 0,
        typename std::enable_if_t<!is_curried_v<std::decay_t<A>>, int> = 0
    >
    constexpr auto operator*(F&& f, A&& a)
    KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
        std::forward<F>(f)(std::forward<A>(a)))

    template
    <
        typename A, typename F,
        typename std::enable_if_t<!is_curried_v<std::decay_t<A>>, int> = 0,
        typename std::enable_if_t< is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr auto operator*(A&& a, F&& f)
    KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(
        std::forward<F>(f)(std::forward<A>(a)))
}

namespace kari
{
    namespace underscore
    {
        struct us_t {};
        constexpr us_t _{};

        //
        // is_underscore, is_underscore_v
        //

        template < typename T >
        struct is_underscore
        : std::integral_constant<bool, std::is_same<us_t, std::remove_cv_t<T>>::value> {};

        template < typename T >
        constexpr bool is_underscore_v = is_underscore<T>::value;

        //
        // unary operators
        //

        #define KARI_HPP_DEFINE_UNDERSCORE_UNARY_OP(op, func) \
            constexpr auto operator op (us_t) KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(curry(func))

            KARI_HPP_DEFINE_UNDERSCORE_UNARY_OP(-, std::negate<>())
            KARI_HPP_DEFINE_UNDERSCORE_UNARY_OP(~, std::bit_not<>())
            KARI_HPP_DEFINE_UNDERSCORE_UNARY_OP(!, std::logical_not<>())
        #undef KARI_HPP_DEFINE_UNDERSCORE_UNARY_OP

        //
        // binary operators
        //

        #define KARI_HPP_DEFINE_UNDERSCORE_BINARY_OP(op, func) \
            constexpr auto operator op (us_t, us_t) \
            KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(curry(func)) \
            \
            template < typename A, typename std::enable_if_t<!is_underscore_v<std::decay_t<A>>, int> = 0 > \
            constexpr auto operator op (A&& a, us_t) \
            KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(curry(func, std::forward<A>(a))) \
            \
            template < typename B, typename std::enable_if_t<!is_underscore_v<std::decay_t<B>>, int> = 0 > \
            constexpr auto operator op (us_t, B&& b) \
            KARI_HPP_NOEXCEPT_DECLTYPE_RETURN(fflip(func, std::forward<B>(b)))

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
}

#undef KARI_HPP_NOEXCEPT_RETURN
#undef KARI_HPP_NOEXCEPT_DECLTYPE_RETURN
