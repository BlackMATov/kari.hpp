/*******************************************************************************
 * This file is part of the "https://github.com/BlackMATov/kari.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2017-2020, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <kari.hpp/kari_ext.hpp>
#include "doctest/doctest.hpp"

using namespace kari_hpp;

TEST_CASE("kari_feature") {
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
