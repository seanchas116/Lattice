#pragma once

#include <optional>

#define LATTICE_OPTIONAL_GUARD(NAME, EXPR, ...) \
    auto&& NAME##_maybe = EXPR; \
    if (!NAME##_maybe) { \
        __VA_ARGS__ \
    } \
    auto NAME = *NAME##_maybe;
