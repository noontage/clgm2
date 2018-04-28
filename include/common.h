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
// mruby
//
#include <mruby.h>

namespace CLGM2
{

//
// Basic Typedef
//
using String = std::string;
template <class T>
using Sptr = std::shared_ptr<T>;
template <class T>
using Function = std::function<T>;
template <class T>
using Array = std::vector<T>;
template <class T1, class T2, class... ARGS>
using Hash = std::unordered_map<T1, T2, ARGS...>;

//
// Util
//
namespace Math
{
const uint32_t MB = 1048576;
const uint32_t GB = 1073741824;
const uint32_t SECOUND_US = 1000000;
}

//
// Config
//
namespace Config
{
const uint64_t net_data_receive_rate = 60; //  60update per second
}

//
// Version
//
namespace Version
{
const String Name = "CLGM";
const int Major = 2;
const int Minor = 0;
const String Type = "dev"; // 'dev' / 'stable' / 'beta' / 'rc'
const String Pretty = "" + Name + " " + std::to_string(Major) + "." + std::to_string(Minor) + " " + Type;
}

} // namespace CLGM2