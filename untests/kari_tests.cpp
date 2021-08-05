/*******************************************************************************
 * This file is part of the "https://github.com/BlackMATov/kari.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2017-2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <kari.hpp/kari.hpp>
#include "doctest/doctest.hpp"

#include <functional>

using namespace kari_hpp;

TEST_CASE("kari") {
    SUBCASE("ctor") {
        {
            constexpr auto l = [](){
                return 1;
            };

            constexpr curry_t f = l;

            STATIC_REQUIRE(f() == 1);
        }
        {
            constexpr auto l = [](auto a){
                return a;
            };

            constexpr curry_t f = l;

            STATIC_REQUIRE(f(1) == 1);
            STATIC_REQUIRE(f()(1) == 1);
        }
        {
            constexpr auto l = [](auto a, auto b){
                return a + b;
            };

            constexpr curry_t f = l;

            STATIC_REQUIRE(f(1,2) == 3);
            STATIC_REQUIRE(f()(1,2) == 3);

            STATIC_REQUIRE(f(1)(2) == 3);
            STATIC_REQUIRE(f()(1)(2) == 3);
            STATIC_REQUIRE(f(1)()(2) == 3);
            STATIC_REQUIRE(f()(1)()(2) == 3);
        }
        {
            constexpr auto l = [](auto a, auto b, auto c){
                return a + b + c;
            };

            constexpr curry_t f = l;

            STATIC_REQUIRE(f(1,2,3) == 6);
            STATIC_REQUIRE(f()(1,2,3) == 6);

            STATIC_REQUIRE(f(1)(2,3) == 6);
            STATIC_REQUIRE(f()(1)(2,3) == 6);
            STATIC_REQUIRE(f(1)()(2,3) == 6);
            STATIC_REQUIRE(f()(1)()(2,3) == 6);

            STATIC_REQUIRE(f(1,2)(3) == 6);
            STATIC_REQUIRE(f()(1,2)(3) == 6);
            STATIC_REQUIRE(f(1,2)()(3) == 6);
            STATIC_REQUIRE(f()(1,2)()(3) == 6);

            STATIC_REQUIRE(f(1)(2)(3) == 6);
            STATIC_REQUIRE(f()(1)(2)(3) == 6);
            STATIC_REQUIRE(f(1)()(2)(3) == 6);
            STATIC_REQUIRE(f(1)(2)()(3) == 6);
            STATIC_REQUIRE(f()(1)()(2)(3) == 6);
            STATIC_REQUIRE(f(1)()(2)()(3) == 6);
            STATIC_REQUIRE(f()(1)()(2)()(3) == 6);
        }
    }

    SUBCASE("nested ctor") {
        {
            constexpr auto l = [](auto a){
                return curry_t([a](auto b, auto c){
                    return a + b + c;
                });
            };

            constexpr curry_t f = l;

            STATIC_REQUIRE(f(1,2,3) == 6);
            STATIC_REQUIRE(f()(1,2,3) == 6);

            STATIC_REQUIRE(f(1)(2,3) == 6);
            STATIC_REQUIRE(f()(1)(2,3) == 6);
            STATIC_REQUIRE(f(1)()(2,3) == 6);
            STATIC_REQUIRE(f()(1)()(2,3) == 6);

            STATIC_REQUIRE(f(1,2)(3) == 6);
            STATIC_REQUIRE(f()(1,2)(3) == 6);
            STATIC_REQUIRE(f(1,2)()(3) == 6);
            STATIC_REQUIRE(f()(1,2)()(3) == 6);

            STATIC_REQUIRE(f(1)(2)(3) == 6);
            STATIC_REQUIRE(f()(1)(2)(3) == 6);
            STATIC_REQUIRE(f(1)()(2)(3) == 6);
            STATIC_REQUIRE(f(1)(2)()(3) == 6);
            STATIC_REQUIRE(f()(1)()(2)(3) == 6);
            STATIC_REQUIRE(f(1)()(2)()(3) == 6);
            STATIC_REQUIRE(f()(1)()(2)()(3) == 6);
        }
        {
            constexpr auto l = [](auto a, auto b){
                return curry_t([a, b](auto c){
                    return a + b + c;
                });
            };

            constexpr curry_t f = l;

            STATIC_REQUIRE(f(1,2,3) == 6);
            STATIC_REQUIRE(f()(1,2,3) == 6);

            STATIC_REQUIRE(f(1)(2,3) == 6);
            STATIC_REQUIRE(f()(1)(2,3) == 6);
            STATIC_REQUIRE(f(1)()(2,3) == 6);
            STATIC_REQUIRE(f()(1)()(2,3) == 6);

            STATIC_REQUIRE(f(1,2)(3) == 6);
            STATIC_REQUIRE(f()(1,2)(3) == 6);
            STATIC_REQUIRE(f(1,2)()(3) == 6);
            STATIC_REQUIRE(f()(1,2)()(3) == 6);

            STATIC_REQUIRE(f(1)(2)(3) == 6);
            STATIC_REQUIRE(f()(1)(2)(3) == 6);
            STATIC_REQUIRE(f(1)()(2)(3) == 6);
            STATIC_REQUIRE(f(1)(2)()(3) == 6);
            STATIC_REQUIRE(f()(1)()(2)(3) == 6);
            STATIC_REQUIRE(f(1)()(2)()(3) == 6);
            STATIC_REQUIRE(f()(1)()(2)()(3) == 6);
        }
    }

    SUBCASE("curry") {
        {
            constexpr auto f = curry([](){
                return 1;
            });

            STATIC_REQUIRE(f == 1);
        }
        {
            {
                constexpr auto f = curry([](auto a){
                    return a;
                });

                STATIC_REQUIRE(f(1) == 1);
                STATIC_REQUIRE(f()(1) == 1);
            }
            {
                constexpr auto f = curry([](auto a){
                    return a;
                }, 1);

                STATIC_REQUIRE(f == 1);
            }
        }
        {
            {
                constexpr auto f = curry([](auto a, auto b){
                    return a + b;
                });

                STATIC_REQUIRE(f(1,2) == 3);
                STATIC_REQUIRE(f()(1,2) == 3);

                STATIC_REQUIRE(f(1)(2) == 3);
                STATIC_REQUIRE(f()(1)(2) == 3);
                STATIC_REQUIRE(f(1)()(2) == 3);
                STATIC_REQUIRE(f()(1)()(2) == 3);
            }
            {
                constexpr auto f = curry([](auto a, auto b){
                    return a + b;
                }, 1);

                STATIC_REQUIRE(f(2) == 3);
                STATIC_REQUIRE(f()(2) == 3);
            }
            {
                constexpr auto f = curry([](auto a, auto b){
                    return a + b;
                }, 1, 2);

                STATIC_REQUIRE(f == 3);
            }
        }
        {
            {
                constexpr auto f = curry([](auto a, auto b, auto c){
                    return a + b + c;
                });

                STATIC_REQUIRE(f(1,2,3) == 6);
                STATIC_REQUIRE(f()(1,2,3) == 6);

                STATIC_REQUIRE(f(1)(2,3) == 6);
                STATIC_REQUIRE(f()(1)(2,3) == 6);
                STATIC_REQUIRE(f(1)()(2,3) == 6);
                STATIC_REQUIRE(f()(1)()(2,3) == 6);

                STATIC_REQUIRE(f(1,2)(3) == 6);
                STATIC_REQUIRE(f()(1,2)(3) == 6);
                STATIC_REQUIRE(f(1,2)()(3) == 6);
                STATIC_REQUIRE(f()(1,2)()(3) == 6);

                STATIC_REQUIRE(f(1)(2)(3) == 6);
                STATIC_REQUIRE(f()(1)(2)(3) == 6);
                STATIC_REQUIRE(f(1)()(2)(3) == 6);
                STATIC_REQUIRE(f(1)(2)()(3) == 6);
                STATIC_REQUIRE(f()(1)()(2)(3) == 6);
                STATIC_REQUIRE(f(1)()(2)()(3) == 6);
                STATIC_REQUIRE(f()(1)()(2)()(3) == 6);
            }
            {
                constexpr auto f = curry([](auto a, auto b, auto c){
                    return a + b + c;
                }, 1);

                STATIC_REQUIRE(f(2,3) == 6);
                STATIC_REQUIRE(f()(2,3) == 6);

                STATIC_REQUIRE(f(2)(3) == 6);
                STATIC_REQUIRE(f()(2)(3) == 6);
                STATIC_REQUIRE(f(2)()(3) == 6);
                STATIC_REQUIRE(f()(2)()(3) == 6);
            }
            {
                constexpr auto f = curry([](auto a, auto b, auto c){
                    return a + b + c;
                }, 1, 2);

                STATIC_REQUIRE(f(3) == 6);
                STATIC_REQUIRE(f()(3) == 6);
            }
            {
                constexpr auto f = curry([](auto a, auto b, auto c){
                    return a + b + c;
                }, 1, 2, 3);

                STATIC_REQUIRE(f == 6);
            }
        }
    }

    SUBCASE("nested curry") {
        {
            constexpr auto f = curry([](auto a){
                return curry([a](auto b, auto c){
                    return a + b + c;
                });
            });

            STATIC_REQUIRE(f(1,2,3) == 6);
            STATIC_REQUIRE(f()(1,2,3) == 6);

            STATIC_REQUIRE(f(1)(2,3) == 6);
            STATIC_REQUIRE(f()(1)(2,3) == 6);
            STATIC_REQUIRE(f(1)()(2,3) == 6);
            STATIC_REQUIRE(f()(1)()(2,3) == 6);

            STATIC_REQUIRE(f(1,2)(3) == 6);
            STATIC_REQUIRE(f()(1,2)(3) == 6);
            STATIC_REQUIRE(f(1,2)()(3) == 6);
            STATIC_REQUIRE(f()(1,2)()(3) == 6);

            STATIC_REQUIRE(f(1)(2)(3) == 6);
            STATIC_REQUIRE(f()(1)(2)(3) == 6);
            STATIC_REQUIRE(f(1)()(2)(3) == 6);
            STATIC_REQUIRE(f(1)(2)()(3) == 6);
            STATIC_REQUIRE(f()(1)()(2)(3) == 6);
            STATIC_REQUIRE(f(1)()(2)()(3) == 6);
            STATIC_REQUIRE(f()(1)()(2)()(3) == 6);
        }
        {
            constexpr auto f = curry([](auto a, auto b){
                return curry([a, b](auto c){
                    return a + b + c;
                });
            });

            STATIC_REQUIRE(f(1,2,3) == 6);
            STATIC_REQUIRE(f()(1,2,3) == 6);

            STATIC_REQUIRE(f(1)(2,3) == 6);
            STATIC_REQUIRE(f()(1)(2,3) == 6);
            STATIC_REQUIRE(f(1)()(2,3) == 6);
            STATIC_REQUIRE(f()(1)()(2,3) == 6);

            STATIC_REQUIRE(f(1,2)(3) == 6);
            STATIC_REQUIRE(f()(1,2)(3) == 6);
            STATIC_REQUIRE(f(1,2)()(3) == 6);
            STATIC_REQUIRE(f()(1,2)()(3) == 6);

            STATIC_REQUIRE(f(1)(2)(3) == 6);
            STATIC_REQUIRE(f()(1)(2)(3) == 6);
            STATIC_REQUIRE(f(1)()(2)(3) == 6);
            STATIC_REQUIRE(f(1)(2)()(3) == 6);
            STATIC_REQUIRE(f()(1)()(2)(3) == 6);
            STATIC_REQUIRE(f(1)()(2)()(3) == 6);
            STATIC_REQUIRE(f()(1)()(2)()(3) == 6);
        }
    }

    SUBCASE("member functions ctor") {
        struct box final {
            int v_;

            constexpr box(int v)
            : v_(v) {}

            constexpr auto add_v(int add) {
                v_ += add;
                return v_;
            }

            constexpr int get_v() const {
                return v_;
            }
        };
        {
            constexpr auto get_v = curry_t(&box::get_v);
            constexpr auto add_v = curry_t(&box::add_v);

            STATIC_REQUIRE(get_v(box(1)) == 1);
            STATIC_REQUIRE(add_v(box(1), 2) == 3);
        }
        {
            constexpr auto get_v = curry_t(&box::get_v);
            constexpr auto add_v = curry_t(&box::add_v);
            {
                box b{1};
                REQUIRE(add_v(&b, 2) == 3);
                REQUIRE(get_v(&b) == 3);
            }
            {
                box b{1};
                REQUIRE(add_v(std::ref(b), 2) == 3);
                REQUIRE(get_v(std::cref(b)) == 3);
            }
        }
    }

    SUBCASE("members functions curry") {
        struct box final {
            int v_;

            constexpr box(int v)
            : v_(v) {}

            constexpr auto add_v(int add) {
                v_ += add;
                return v_;
            }

            constexpr int get_v() const {
                return v_;
            }
        };
        {
            constexpr auto get_v = curry(&box::get_v);
            constexpr auto add_v = curry(&box::add_v);

            STATIC_REQUIRE(get_v(box(1)) == 1);
            STATIC_REQUIRE(add_v(box(1), 2) == 3);

            {
                box b{1};
                REQUIRE(add_v(std::ref(b), 2) == 3);
                REQUIRE(get_v(std::cref(b)) == 3);
                REQUIRE(add_v(std::ref(b), 2) == 5);
                REQUIRE(get_v(std::cref(b)) == 5);
            }
            {
                box b{1};
                REQUIRE(add_v(std::ref(b), 2) == 3);
                REQUIRE(get_v(std::cref(b)) == 3);
            }
        }
        {
            box b{1};
            const int get_v = curry(&box::get_v, std::cref(b));
            const auto add_v = curry(&box::add_v, std::ref(b));

            REQUIRE(get_v == 1);
            REQUIRE(add_v(2) == 3);
            REQUIRE(b.get_v() == 3);
            REQUIRE(add_v(2) == 5);
            REQUIRE(b.get_v() == 5);
        }
        {
            box b{1};
            const auto add_v = curry(&box::add_v, std::ref(b), 2);

            REQUIRE(add_v == 3);
            REQUIRE(b.get_v() == 3);
        }
    }

    SUBCASE("members objects ctor") {
        struct box final {
            int v_;
            constexpr box(int v)
            : v_(v) {}
        };
        {
            constexpr auto f = curry_t(&box::v_);
            STATIC_REQUIRE(f(box(1)) == 1);
        }
        {
            constexpr auto f = curry_t(&box::v_);
            box b{1};
            REQUIRE(f(b) == 1);
            REQUIRE(f(&b) == 1);
            REQUIRE(f(std::cref(b)) == 1);
        }
    }

    SUBCASE("members objects curry") {
        struct box final {
            int v_;
            constexpr box(int v)
            : v_(v) {}
        };
        {
            constexpr auto f = curry(&box::v_);
            STATIC_REQUIRE(f(box(1)) == 1);
        }
        {
            constexpr auto f = curry(&box::v_, box(1));
            STATIC_REQUIRE(f == 1);
        }
        {
            const box b{1};
            const auto f = curry(&box::v_, b);
            REQUIRE(f == 1);
        }
    }

    SUBCASE("ref forwarding") {
        {
            int i = 42;
            const int& g = curry([](int& v, int){
                return std::ref(v);
            }, std::ref(i), 0);
            REQUIRE(&i == &g);
        }
        {
            const int i = 42;
            const int& g = curry([](const int& v, int){
                return std::ref(v);
            }, std::cref(i), 0);
            REQUIRE(&i == &g);
        }
    }

    SUBCASE("args forwarding") {
        struct box final {
            int v_;
            constexpr box(int v)
            : v_(v) {}

            box(box&&) = default;
            box& operator=(box&&) = delete;

            box(const box&) = delete;
            box& operator=(const box&) = delete;
        };

        constexpr auto b = curry([](auto&& v1, auto&& v2, auto&& v3){
            return box(v1.v_ + v2.v_ + v3.v_);
        })(box(1),box(2))(box(3));

        STATIC_REQUIRE(b.v_ == 6);
    }
}
