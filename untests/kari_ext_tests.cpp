/*******************************************************************************
 * This file is part of the "https://github.com/BlackMATov/kari.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2017-2020, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <kari.hpp/kari_ext.hpp>
#include "doctest/doctest.hpp"

#include "kari_tests.hpp"

using namespace kari_hpp;
using namespace kari_tests;

TEST_CASE("kari_ext") {
    SUBCASE("fid") {
        REQUIRE(fid(box(10)).v() == 10);
    }

    SUBCASE("fconst") {
        REQUIRE(fconst(box(10), 20).v() == 10);
        {
            auto b10 = fconst(box(10));
            REQUIRE(b10(20).v() == 10);
            REQUIRE(std::move(b10)(30).v() == 10);
            REQUIRE(b10(20).v() == 100500);
        }
    }

    SUBCASE("fflip") {
        REQUIRE(fflip(curry(std::minus<>()))(10, 20) == 10);
        REQUIRE(fflip(minus3_gl)(10,20,50) == -40);
    }

    SUBCASE("fpipe") {
        using namespace underscore;
        REQUIRE(fpipe(_+2, _*2, 4) == 12);
        REQUIRE(((_+2) | (_*2) | 4) == 12);
        REQUIRE((4 | (_+2) | (_*2)) == 12);
    }

    SUBCASE("fcompose") {
        using namespace underscore;
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

    SUBCASE("underscore") {
        using namespace underscore;
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
}
