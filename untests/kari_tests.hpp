/*******************************************************************************
 * This file is part of the "https://github.com/BlackMATov/kari.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2017-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <kari.hpp/kari.hpp>
#include <doctest/doctest.h>

#define STATIC_CHECK(...)\
    static_assert(__VA_ARGS__, #__VA_ARGS__);\
    CHECK(__VA_ARGS__)

#define STATIC_CHECK_FALSE(...)\
    static_assert(!(__VA_ARGS__), "!(" #__VA_ARGS__ ")");\
    CHECK(!(__VA_ARGS__))
