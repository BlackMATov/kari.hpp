/*******************************************************************************
 * This file is part of the "https://github.com/BlackMATov/kari.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2017-2020, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <kari.hpp/kari_ext.hpp>
#include "doctest/doctest.hpp"

using namespace kari_hpp;

TEST_CASE("kari_ext") {
    struct box final {
        int v;
        constexpr box(int v): v(v) {}
    };

    SUBCASE("fid") {
        STATIC_REQUIRE(fid(box(10)).v == 10);
    }

    SUBCASE("fconst") {
        STATIC_REQUIRE(fconst(box(10), 20).v == 10);
        {
            constexpr auto f = fconst(box(10));
            STATIC_REQUIRE(f(20).v == 10);
        }
    }

    SUBCASE("fflip") {
        using namespace underscore;
        STATIC_REQUIRE(fflip(_ - _)(10, 20) == 10);
    }

    SUBCASE("fpipe") {
        using namespace underscore;
        STATIC_REQUIRE(fpipe(_+2, _*2, 4) == 12);
        STATIC_REQUIRE(((_+2) | (_*2) | 4) == 12);
        STATIC_REQUIRE((4 | (_+2) | (_*2)) == 12);
    }

    SUBCASE("fcompose") {
        using namespace underscore;
        STATIC_REQUIRE(fcompose(_+2, _*2, 4) == 10);
        STATIC_REQUIRE((_+2) * (_*2) * 4 == 10);
        STATIC_REQUIRE(4 * (_+2) * (_*2) == 12);
        {
            constexpr auto s3 = [](int v1, int v2, int v3){
                return v1 + v2 + v3;
            };
            constexpr auto c = curry(s3) * (_*2) * 10 * 20 * 30;
            STATIC_REQUIRE(c == 70);
        }
        {
            // (. (+2)) (*2) $ 10 == 24
            constexpr int i = fflip(fcompose)(_+2, _*2, 10);
            constexpr int j = (_*(_+2))(_*2)(10);
            STATIC_REQUIRE(i == 24);
            STATIC_REQUIRE(j == 24);
        }
        {
            // ((+2) .) (*2) $ 10 == 24
            constexpr int i = fcompose(_+2)(_*2, 10);
            constexpr int j = ((_+2) * _)(_*2)(10);
            STATIC_REQUIRE(i == 22);
            STATIC_REQUIRE(j == 22);
        }
    }

    SUBCASE("underscore") {
        using namespace underscore;
        STATIC_REQUIRE((-_)(40) == -40);

        STATIC_REQUIRE((_ + 40)(2) == 42);
        STATIC_REQUIRE((_ - 2)(44) == 42);
        STATIC_REQUIRE((_ * 21)(2) == 42);
        STATIC_REQUIRE((_ / 2)(84) == 42);
        STATIC_REQUIRE((_ % 100)(142) == 42);

        STATIC_REQUIRE((_ == 42)(42));
        STATIC_REQUIRE((_ != 42)(40));
        STATIC_REQUIRE_FALSE((_ == 42)(40));
        STATIC_REQUIRE_FALSE((_ != 42)(42));

        STATIC_REQUIRE((40 + _)(2) == 42);
        STATIC_REQUIRE((44 - _)(2) == 42);
        STATIC_REQUIRE((21 * _)(2) == 42);
        STATIC_REQUIRE((84 / _)(2) == 42);
        STATIC_REQUIRE((142 % _)(100) == 42);

        STATIC_REQUIRE((42 == _)(42));
        STATIC_REQUIRE((42 != _)(40));
        STATIC_REQUIRE_FALSE((42 == _)(40));
        STATIC_REQUIRE_FALSE((42 != _)(42));

        STATIC_REQUIRE((_ + _)(40,2) == 42);
        STATIC_REQUIRE((_ - _)(44,2) == 42);
        STATIC_REQUIRE((_ * _)(21,2) == 42);
        STATIC_REQUIRE((_ / _)(84,2) == 42);
        STATIC_REQUIRE((_ % _)(142,100) == 42);

        STATIC_REQUIRE((_ == _)(42,42));
        STATIC_REQUIRE((_ != _)(42,40));
        STATIC_REQUIRE_FALSE((_ == _)(42,40));
        STATIC_REQUIRE_FALSE((_ != _)(42,42));
    }
}
