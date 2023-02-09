#pragma once
#include <tuple>
#include <stddef.h>

template<typename ... Args>
struct event {
    size_t id;
    std::tuple<Args...> args;
    event(const Args& ... a) : args{a...} {}
};
