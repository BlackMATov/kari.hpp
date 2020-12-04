#pragma once

#include "doctest.h"

#define STATIC_REQUIRE(...)\
    static_assert(__VA_ARGS__, #__VA_ARGS__);\
    REQUIRE(__VA_ARGS__);

#define STATIC_REQUIRE_FALSE(...)\
    static_assert(!(__VA_ARGS__), "!(" #__VA_ARGS__ ")");\
    REQUIRE(!(__VA_ARGS__));
