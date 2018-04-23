#pragma once

//
// STL
//
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

//
// Basic Typedef
//
using String = std::string;
template <class T> using Sptr = std::shared_ptr<T>;
template <class T> using Function = std::function<T>;
template <class T> using Array = std::vector<T>;
template <class T1, class T2, class... ARGS> using Hash = std::unordered_map<T1, T2, ARGS...>;

//
// Util
//
namespace Math {
    const uint32_t MB = 1048576;
    const uint32_t GB = 1073741824;
    const uint32_t SECOUND_US = 1000000;
}