#pragma once

#include <tuple>
#include <limits>
#include <utility>
#include <functional>
#include <type_traits>

namespace kari
{
    namespace detail
    {
        namespace std_ext
        {
            //
            // void_t
            //

            template < typename... Ts >
            struct make_void {
                using type = void;
            };

            template < typename... Ts >
            using void_t = typename make_void<Ts...>::type;

            //
            // apply
            //

            template < typename F, typename Tuple, std::size_t... I >
            constexpr decltype(auto) apply_impl(F&& f, Tuple&& args, std::index_sequence<I...>) {
                return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(args))...);
            }

            template < typename F, typename Tuple >
            constexpr decltype(auto) apply(F&& f, Tuple&& args) {
                return apply_impl(
                    std::forward<F>(f),
                    std::forward<Tuple>(args),
                    std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>());
            }

            //
            // is_invocable, is_invocable_v
            //

            namespace detail
            {
                template < typename F, typename = void >
                struct is_invocable_impl
                : std::false_type {};

                template < typename F, typename... Args >
                struct is_invocable_impl<
                    F(Args...),
                    void_t<decltype(std::declval<F>()(std::declval<Args>()...))>
                > : std::true_type {};
            }

            template < typename F, typename... Args >
            struct is_invocable
            : detail::is_invocable_impl<F(Args...)> {};

            template < typename F, typename... Args >
            constexpr bool is_invocable_v = is_invocable<F, Args...>::value;

            //
            // conjunction, conjunction_v
            //

            template < typename... >
            struct conjunction
            : std::true_type {};

            template < typename B >
            struct conjunction<B>
            : B {};

            template < typename B, typename... Bs>
            struct conjunction<B, Bs...>
            : std::conditional_t<bool(B::value), conjunction<Bs...>, B> {};

            template < typename... Bs >
            constexpr bool conjunction_v = conjunction<Bs...>::value;

            //
            // disjunction, disjunction_v
            //

            template < typename... >
            struct disjunction
            : std::false_type {};

            template < typename B >
            struct disjunction<B>
            : B {};

            template < typename B, typename... Bs>
            struct disjunction<B, Bs...>
            : std::conditional_t<bool(B::value), B, disjunction<Bs...>> {};

            template < typename... Bs >
            constexpr bool disjunction_v = disjunction<Bs...>::value;

            //
            // bool_constant
            //

            template < bool B >
            using bool_constant = std::integral_constant<bool, B>;

            //
            // negation, negation_v
            //

            template < typename B >
            struct negation
            : bool_constant<!bool(B::value)> {};

            template < typename B >
            constexpr bool negation_v = negation<B>::value;
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
            typename std::enable_if<std_ext::conjunction_v<
                std_ext::bool_constant<(N == 0)>,
                std_ext::is_invocable<F, Args...>
            >, int>::type = 0
        >
        constexpr auto make_curry(F&& f, std::tuple<Args...>&& args) {
            return std_ext::apply(std::forward<F>(f), std::move(args));
        }

        template
        <
            std::size_t N, typename F, typename... Args,
            typename std::enable_if<std_ext::disjunction_v<
                std_ext::bool_constant<(N > 0)>,
                std_ext::negation<std_ext::is_invocable<F, Args...>>
            >, int>::type = 0
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
    : detail::is_curried_impl<std::remove_cv_t<std::remove_reference_t<F>>> {};

    template < typename F >
    constexpr bool is_curried_v = is_curried<F>::value;

    //
    // curry
    //

    template
    <
        typename F,
        typename std::enable_if<is_curried_v<F>, int>::type = 0
    >
    constexpr decltype(auto) curry(F&& f) {
        return std::forward<F>(f);
    }

    template
    <
        typename F,
        typename std::enable_if<!is_curried_v<F>, int>::type = 0
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
        typename std::enable_if<is_curried_v<F>, int>::type = 0
    >
    constexpr decltype(auto) curryV(F&& f) {
        constexpr auto n = std::numeric_limits<std::size_t>::max();
        return std::forward<F>(f).template recurry<n>();
    }

    template
    <
        typename F,
        typename std::enable_if<!is_curried_v<F>, int>::type = 0
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
        typename std::enable_if<is_curried_v<F>, int>::type = 0
    >
    constexpr decltype(auto) curryN(F&& f) {
        return std::forward<F>(f).template recurry<N>();
    }

    template
    <
        std::size_t N, typename F,
        typename std::enable_if<!is_curried_v<F>, int>::type = 0
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
        typename std::enable_if<is_curried_v<G>, int>::type = 0,
        typename std::enable_if<is_curried_v<F>, int>::type = 0
    >
    constexpr decltype(auto) operator|(G&& g, F&& f) {
        return fpipe(
            std::forward<G>(g),
            std::forward<F>(f));
    }

    template
    <
        typename F, typename A,
        typename std::enable_if< is_curried_v<F>, int>::type = 0,
        typename std::enable_if<!is_curried_v<A>, int>::type = 0
    >
    constexpr decltype(auto) operator|(F&& f, A&& a) {
        return std::forward<F>(f)(std::forward<A>(a));
    }

    template
    <
        typename A, typename F,
        typename std::enable_if<!is_curried_v<A>, int>::type = 0,
        typename std::enable_if< is_curried_v<F>, int>::type = 0
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
        typename std::enable_if<is_curried_v<G>, int>::type = 0,
        typename std::enable_if<is_curried_v<F>, int>::type = 0
    >
    constexpr decltype(auto) operator*(G&& g, F&& f) {
        return fcompose(
            std::forward<G>(g),
            std::forward<F>(f));
    }

    template
    <
        typename F, typename A,
        typename std::enable_if< is_curried_v<F>, int>::type = 0,
        typename std::enable_if<!is_curried_v<A>, int>::type = 0
    >
    constexpr decltype(auto) operator*(F&& f, A&& a) {
        return std::forward<F>(f)(std::forward<A>(a));
    }

    template
    <
        typename A, typename F,
        typename std::enable_if<!is_curried_v<A>, int>::type = 0,
        typename std::enable_if< is_curried_v<F>, int>::type = 0
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

        template < typename A > constexpr auto operator |  (us_t, A&& a) { return fflip(_ |  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator &  (us_t, A&& a) { return fflip(_ &  _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator ^  (us_t, A&& a) { return fflip(_ ^  _)(std::forward<A>(a)); }

        template < typename A > constexpr auto operator || (us_t, A&& a) { return fflip(_ || _)(std::forward<A>(a)); }
        template < typename A > constexpr auto operator && (us_t, A&& a) { return fflip(_ && _)(std::forward<A>(a)); }
    }
}

// Local Variables:
// indent-tabs-mode: nil
// End:
