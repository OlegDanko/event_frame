#pragma once
#include <tuple>
#include <stddef.h>

template<typename ... Args>
struct event {
    std::tuple<Args...> args;
    event(const Args& ... a) : args{a...} {}
};
