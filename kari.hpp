#pragma once

#include <tuple>
#include <limits>
#include <utility>
#include <functional>
#include <type_traits>

#define KARI_HPP_NOEXCEPT_RETURN(...) \
    noexcept(noexcept(__VA_ARGS__)) -> decltype (__VA_ARGS__) { return __VA_ARGS__; }

namespace kari
{
    namespace detail
    {
        namespace std_ext
        {
            //
            // void_t
            //

            namespace detail
            {
                template < typename... Ts >
                struct make_void {
                    using type = void;
                };
            }

            template < typename... Ts >
            using void_t = typename detail::make_void<Ts...>::type;

            //
            // is_reference_wrapper, is_reference_wrapper_v
            //

            namespace detail
            {
                template < typename T >
                struct is_reference_wrapper_impl
                : public std::false_type {};

                template < typename T >
                struct is_reference_wrapper_impl<std::reference_wrapper<T>>
                : public std::true_type {};
            }

            template < typename T >
            struct is_reference_wrapper
            : public detail::is_reference_wrapper_impl<std::remove_cv_t<T>> {};

            template < typename T >
            constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

            //
            // invoke
            //

            namespace detail
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
                KARI_HPP_NOEXCEPT_RETURN(std::forward<Derived>(ref).*f)

                template
                <
                    typename Base, typename F, typename RefWrap,
                    typename std::enable_if_t<is_reference_wrapper<std::decay_t<RefWrap>>::value, int> = 0
                >
                constexpr auto invoke_member_object_impl(F Base::* f, RefWrap&& ref)
                KARI_HPP_NOEXCEPT_RETURN(ref.get().*f)

                template
                <
                    typename Base, typename F, typename Pointer,
                    typename std::enable_if_t<
                        !std::is_base_of<Base, std::decay_t<Pointer>>::value &&
                        !is_reference_wrapper_v<std::decay_t<Pointer>>
                    , int> = 0
                >
                constexpr auto invoke_member_object_impl(F Base::* f, Pointer&& ptr)
                KARI_HPP_NOEXCEPT_RETURN((*std::forward<Pointer>(ptr)).*f)

                //
                // invoke_member_function_impl
                //

                template
                <
                    typename Base, typename F, typename Derived, typename... Args,
                    typename std::enable_if_t<std::is_base_of<Base, std::decay_t<Derived>>::value, int> = 0
                >
                constexpr auto invoke_member_function_impl(F Base::* f, Derived&& ref, Args&&... args)
                KARI_HPP_NOEXCEPT_RETURN((std::forward<Derived>(ref).*f)(std::forward<Args>(args)...))

                template
                <
                    typename Base, typename F, typename RefWrap, typename... Args,
                    typename std::enable_if_t<is_reference_wrapper<std::decay_t<RefWrap>>::value, int> = 0
                >
                constexpr auto invoke_member_function_impl(F Base::* f, RefWrap&& ref, Args&&... args)
                KARI_HPP_NOEXCEPT_RETURN((ref.get().*f)(std::forward<Args>(args)...))

                template
                <
                    typename Base, typename F, typename Pointer, typename... Args,
                    typename std::enable_if_t<
                        !std::is_base_of<Base, std::decay_t<Pointer>>::value &&
                        !is_reference_wrapper_v<std::decay_t<Pointer>>
                    , int> = 0
                >
                constexpr auto invoke_member_function_impl(F Base::* f, Pointer&& ptr, Args&&... args)
                KARI_HPP_NOEXCEPT_RETURN(((*std::forward<Pointer>(ptr)).*f)(std::forward<Args>(args)...))
            }

            template
            <
                typename F, typename... Args,
                typename std::enable_if_t<!std::is_member_pointer<std::decay_t<F>>::value, int> = 0
            >
            constexpr auto invoke(F&& f, Args&&... args)
            KARI_HPP_NOEXCEPT_RETURN(std::forward<F>(f)(std::forward<Args>(args)...))

            template
            <
                typename F, typename T,
                typename std::enable_if_t<std::is_member_object_pointer<std::decay_t<F>>::value, int> = 0
            >
            constexpr auto invoke(F&& f, T&& t)
            KARI_HPP_NOEXCEPT_RETURN(detail::invoke_member_object_impl(std::forward<F>(f), std::forward<T>(t)))

            template
            <
                typename F, typename... Args,
                typename std::enable_if_t<std::is_member_function_pointer<std::decay_t<F>>::value, int> = 0
            >
            constexpr auto invoke(F&& f, Args&&... args)
            KARI_HPP_NOEXCEPT_RETURN(detail::invoke_member_function_impl(std::forward<F>(f), std::forward<Args>(args)...))

            //
            // invoke_result, invoke_result_t
            //

            namespace detail
            {
                struct invoke_result_impl_tag {};

                template < typename Void, typename F, typename... Args >
                struct invoke_result_impl {};

                template < typename F, typename... Args >
                struct invoke_result_impl<void_t<invoke_result_impl_tag, decltype(std_ext::invoke(std::declval<F>(), std::declval<Args>()...))>, F, Args...> {
                    using type = decltype(std_ext::invoke(std::declval<F>(), std::declval<Args>()...));
                };
            }

            template < typename F, typename... Args >
            struct invoke_result
            : detail::invoke_result_impl<void, F, Args...> {};

            template < typename F, typename... Args >
            using invoke_result_t = typename invoke_result<F, Args...>::type;

            //
            // is_invocable, is_invocable_v
            //

            namespace detail
            {
                struct is_invocable_impl_tag {};

                template < typename Void, typename F, typename... Args >
                struct is_invocable_impl
                : std::false_type {};

                template < typename F, typename... Args >
                struct is_invocable_impl<void_t<is_invocable_impl_tag, invoke_result_t<F, Args...>>, F, Args...>
                : std::true_type {};
            }

            template < typename F, typename... Args >
            struct is_invocable
            : detail::is_invocable_impl<void, F, Args...> {};

            template < typename F, typename... Args >
            constexpr bool is_invocable_v = is_invocable<F, Args...>::value;

            //
            // is_nothrow_invocable, is_nothrow_invocable_v
            //

            namespace detail
            {
                template < bool Invocable, typename F, typename... Args >
                struct is_nothrow_invocable_impl
                : std::false_type {};

                template < typename F, typename... Args >
                struct is_nothrow_invocable_impl<true, F, Args...>
                : std::integral_constant<
                    bool,
                    noexcept(std_ext::invoke(std::declval<F>(), std::declval<Args>()...))> {};
            }

            template < typename F, typename... Args >
            struct is_nothrow_invocable
            : detail::is_nothrow_invocable_impl<is_invocable_v<F, Args...>, F, Args...> {};

            template < typename F, typename... Args >
            constexpr bool is_nothrow_invocable_v = is_nothrow_invocable<F, Args...>::value;

            //
            // apply
            //

            namespace detail
            {
                template < typename F, typename Tuple, std::size_t... I >
                constexpr auto apply_impl(F&& f, Tuple&& args, std::index_sequence<I...>)
                KARI_HPP_NOEXCEPT_RETURN(
                    std_ext::invoke(
                        std::forward<F>(f),
                        std::get<I>(std::forward<Tuple>(args))...))
            }

            template < typename F, typename Tuple >
            constexpr auto apply(F&& f, Tuple&& args)
            KARI_HPP_NOEXCEPT_RETURN(
                detail::apply_impl(
                    std::forward<F>(f),
                    std::forward<Tuple>(args),
                    std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>()))
        }
    }
}

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
                std_ext::is_invocable_v<std::decay_t<F>, Args...>
            , int> = 0
        >
        constexpr auto make_curry(F&& f, std::tuple<Args...>&& args) {
            return std_ext::apply(std::forward<F>(f), std::move(args));
        }

        template
        <
            std::size_t N, typename F, typename... Args,
            typename std::enable_if_t<
                (N > 0) ||
                !std_ext::is_invocable_v<std::decay_t<F>, Args...>
            , int> = 0
        >
        constexpr decltype(auto) make_curry(F&& f, std::tuple<Args...>&& args) {
            return curry_t<
                N,
                std::decay_t<F>,
                Args...
            >(std::forward<F>(f), std::move(args));
        }

        template < std::size_t N, typename F >
        constexpr decltype(auto) make_curry(F&& f) {
            return make_curry<N>(std::forward<F>(f), std::make_tuple());
        }
    }

    template < std::size_t N, typename F, typename... Args >
    struct curry_t final {
        template < typename U >
        constexpr curry_t(U&& u, std::tuple<Args...>&& args)
        : f_(std::forward<U>(u))
        , args_(std::move(args)) {}

        // min_arity

        constexpr std::size_t min_arity() const noexcept {
            return N;
        }

        // recurry

        template < std::size_t M >
        constexpr decltype(auto) recurry() && {
            return detail::make_curry<M>(
                std::move(f_),
                std::move(args_));
        }

        template < std::size_t M >
        constexpr decltype(auto) recurry() const & {
            auto self_copy = *this;
            return std::move(self_copy).template recurry<M>();
        }

        // operator(As&&...)

        constexpr decltype(auto) operator()() && {
            return std::move(*this).template recurry<0>();
        }

        template < typename A >
        constexpr decltype(auto) operator()(A&& a) && {
            return detail::make_curry<(N > 0 ? N - 1 : 0)>(
                std::move(f_),
                std::tuple_cat(
                    std::move(args_),
                    std::make_tuple(std::forward<A>(a))));
        }

        template < typename A, typename... As >
        constexpr decltype(auto) operator()(A&& a, As&&... as) && {
            return std::move(*this)(std::forward<A>(a))(std::forward<As>(as)...);
        }

        template < typename... As >
        constexpr decltype(auto) operator()(As&&... as) const & {
            auto self_copy = *this;
            return std::move(self_copy)(std::forward<As>(as)...);
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
    constexpr decltype(auto) curry(F&& f) {
        return std::forward<F>(f);
    }

    template
    <
        typename F,
        typename std::enable_if_t<!is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr decltype(auto) curry(F&& f) {
        return detail::make_curry<0>(std::forward<F>(f));
    }

    template < typename F, typename... Args >
    constexpr decltype(auto) curry(F&& f, Args&&... args) {
        return curry(std::forward<F>(f))(std::forward<Args>(args)...);
    }

    //
    // curryV
    //

    template
    <
        typename F,
        typename std::enable_if_t<is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr decltype(auto) curryV(F&& f) {
        constexpr auto n = std::numeric_limits<std::size_t>::max();
        return std::forward<F>(f).template recurry<n>();
    }

    template
    <
        typename F,
        typename std::enable_if_t<!is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr decltype(auto) curryV(F&& f) {
        constexpr auto n = std::numeric_limits<std::size_t>::max();
        return detail::make_curry<n>(std::forward<F>(f));
    }

    template < typename F, typename... Args >
    constexpr decltype(auto) curryV(F&& f, Args&&... args) {
        return curryV(std::forward<F>(f))(std::forward<Args>(args)...);
    }

    //
    // curryN
    //

    template
    <
        std::size_t N, typename F,
        typename std::enable_if_t<is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr decltype(auto) curryN(F&& f) {
        return std::forward<F>(f).template recurry<N>();
    }

    template
    <
        std::size_t N, typename F,
        typename std::enable_if_t<!is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr decltype(auto) curryN(F&& f) {
        return detail::make_curry<N>(std::forward<F>(f));
    }

    template < std::size_t N, typename F, typename... Args >
    constexpr decltype(auto) curryN(F&& f, Args&&... args) {
        static_assert(
            std::numeric_limits<std::size_t>::max() - sizeof...(Args) >= N,
            "N too big");
        return curryN<N + sizeof...(Args)>(std::forward<F>(f))(std::forward<Args>(args)...);
    }
}

namespace kari
{
    //
    // fid
    //

    struct fid_t {
        template < typename A >
        constexpr decltype(auto) operator()(A&& a) const {
            return std::forward<A>(a);
        }
    };
    constexpr auto fid = curry(fid_t{});

    //
    // fconst
    //

    struct fconst_t {
        template < typename A, typename B >
        decltype(auto) operator()(A&& a, B&& b) const {
            (void)b;
            return std::forward<A>(a);
        }
    };
    constexpr auto fconst = curry(fconst_t{});

    //
    // fflip
    //

    struct fflip_t {
        template < typename F, typename A, typename B >
        constexpr decltype(auto) operator()(F&& f, A&& a, B&& b) const {
            return curry(
                std::forward<F>(f),
                std::forward<B>(b),
                std::forward<A>(a));
        }
    };
    constexpr auto fflip = curry(fflip_t{});

    //
    // fpipe
    //

    struct fpipe_t {
        template < typename G, typename F, typename A >
        constexpr decltype(auto) operator()(G&& g, F&& f, A&& a) const {
            return curry(
                std::forward<F>(f),
                curry(
                    std::forward<G>(g),
                    std::forward<A>(a)));
        }
    };
    constexpr auto fpipe = curry(fpipe_t{});

    //
    // fcompose
    //

    struct fcompose_t {
        template < typename G, typename F, typename A >
        constexpr decltype(auto) operator()(G&& g, F&& f, A&& a) const {
            return curry(
                std::forward<G>(g),
                curry(
                    std::forward<F>(f),
                    std::forward<A>(a)));
        }
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
    constexpr decltype(auto) operator|(G&& g, F&& f) {
        return fpipe(
            std::forward<G>(g),
            std::forward<F>(f));
    }

    template
    <
        typename F, typename A,
        typename std::enable_if_t< is_curried_v<std::decay_t<F>>, int> = 0,
        typename std::enable_if_t<!is_curried_v<std::decay_t<A>>, int> = 0
    >
    constexpr decltype(auto) operator|(F&& f, A&& a) {
        return std::forward<F>(f)(std::forward<A>(a));
    }

    template
    <
        typename A, typename F,
        typename std::enable_if_t<!is_curried_v<std::decay_t<A>>, int> = 0,
        typename std::enable_if_t< is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr decltype(auto) operator|(A&& a, F&& f) {
        return std::forward<F>(f)(std::forward<A>(a));
    }

    //
    // fcompose operators
    //

    template
    <
        typename G, typename F,
        typename std::enable_if_t<is_curried_v<std::decay_t<G>>, int> = 0,
        typename std::enable_if_t<is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr decltype(auto) operator*(G&& g, F&& f) {
        return fcompose(
            std::forward<G>(g),
            std::forward<F>(f));
    }

    template
    <
        typename F, typename A,
        typename std::enable_if_t< is_curried_v<std::decay_t<F>>, int> = 0,
        typename std::enable_if_t<!is_curried_v<std::decay_t<A>>, int> = 0
    >
    constexpr decltype(auto) operator*(F&& f, A&& a) {
        return std::forward<F>(f)(std::forward<A>(a));
    }

    template
    <
        typename A, typename F,
        typename std::enable_if_t<!is_curried_v<std::decay_t<A>>, int> = 0,
        typename std::enable_if_t< is_curried_v<std::decay_t<F>>, int> = 0
    >
    constexpr decltype(auto) operator*(A&& a, F&& f) {
        return std::forward<F>(f)(std::forward<A>(a));
    }
}

namespace kari
{
    namespace underscore
    {
        struct us_t {};
        constexpr us_t _{};

        // `op` _

        constexpr auto operator - (us_t) { return curry(std::negate<>()); }
        constexpr auto operator ~ (us_t) { return curry(std::bit_not<>()); }
        constexpr auto operator ! (us_t) { return curry(std::logical_not<>()); }

        // _ `op` _

        constexpr auto operator +  (us_t, us_t) { return curry(std::plus<>()); }
        constexpr auto operator -  (us_t, us_t) { return curry(std::minus<>()); }
        constexpr auto operator *  (us_t, us_t) { return curry(std::multiplies<>()); }
        constexpr auto operator /  (us_t, us_t) { return curry(std::divides<>()); }
        constexpr auto operator %  (us_t, us_t) { return curry(std::modulus<>()); }

        constexpr auto operator <  (us_t, us_t) { return curry(std::less<>()); }
        constexpr auto operator >  (us_t, us_t) { return curry(std::greater<>()); }
        constexpr auto operator <= (us_t, us_t) { return curry(std::less_equal<>()); }
        constexpr auto operator >= (us_t, us_t) { return curry(std::greater_equal<>()); }
        constexpr auto operator == (us_t, us_t) { return curry(std::equal_to<>()); }
        constexpr auto operator != (us_t, us_t) { return curry(std::not_equal_to<>()); }

        constexpr auto operator |  (us_t, us_t) { return curry(std::bit_or<>()); }
        constexpr auto operator &  (us_t, us_t) { return curry(std::bit_and<>()); }
        constexpr auto operator ^  (us_t, us_t) { return curry(std::bit_xor<>()); }

        constexpr auto operator || (us_t, us_t) { return curry(std::logical_or<>()); }
        constexpr auto operator && (us_t, us_t) { return curry(std::logical_and<>()); }

        // A `op` _

        template < typename A > constexpr auto operator +  (A&& a, us_t) { return (_ +  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator -  (A&& a, us_t) { return (_ -  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator *  (A&& a, us_t) { return (_ *  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator /  (A&& a, us_t) { return (_ /  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator %  (A&& a, us_t) { return (_ %  _)(std::forward<A>(a)); }

        template < typename A > constexpr auto operator <  (A&& a, us_t) { return (_ <  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator >  (A&& a, us_t) { return (_ >  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator <= (A&& a, us_t) { return (_ <= _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator >= (A&& a, us_t) { return (_ >= _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator == (A&& a, us_t) { return (_ == _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator != (A&& a, us_t) { return (_ != _)(std::forward<A>(a)); }

        template < typename A > constexpr auto operator |  (A&& a, us_t) { return (_ |  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator &  (A&& a, us_t) { return (_ &  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator ^  (A&& a, us_t) { return (_ ^  _)(std::forward<A>(a)); }

        template < typename A > constexpr auto operator || (A&& a, us_t) { return (_ || _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator && (A&& a, us_t) { return (_ && _)(std::forward<A>(a)); }

        // _ `op` A

        template < typename A > constexpr auto operator +  (us_t, A&& a) { return fflip(_ +  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator -  (us_t, A&& a) { return fflip(_ -  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator *  (us_t, A&& a) { return fflip(_ *  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator /  (us_t, A&& a) { return fflip(_ /  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator %  (us_t, A&& a) { return fflip(_ %  _)(std::forward<A>(a)); }

        template < typename A > constexpr auto operator <  (us_t, A&& a) { return fflip(_ <  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator >  (us_t, A&& a) { return fflip(_ >  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator <= (us_t, A&& a) { return fflip(_ <= _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator >= (us_t, A&& a) { return fflip(_ >= _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator == (us_t, A&& a) { return fflip(_ == _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator != (us_t, A&& a) { return fflip(_ != _)(std::forward<A>(a)); }

        template < typename A > constexpr auto operator |  (us_t, A&& a) { return fflip(_ |  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator &  (us_t, A&& a) { return fflip(_ &  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator ^  (us_t, A&& a) { return fflip(_ ^  _)(std::forward<A>(a)); }

        template < typename A > constexpr auto operator || (us_t, A&& a) { return fflip(_ || _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator && (us_t, A&& a) { return fflip(_ && _)(std::forward<A>(a)); }
    }
}

#undef KARI_HPP_NOEXCEPT_RETURN

// Local Variables:
// indent-tabs-mode: nil
// End:
