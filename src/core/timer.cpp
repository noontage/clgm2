/**
 * @file time.cpp
 * @brief Timer Measure for Cross platform
 * @author noontage
 */
#include <core/timer.h>
#include <chrono>

namespace CLGM2 {

Timer::Timer()
{
}

Timer::~Timer()
{
}

//
// begin
//
void Timer::begin()
{
  _ss = std::chrono::system_clock::now();
}

//
// reset
//
void Timer::reset()
{
  begin();
}

//
// watch
// return: us
//
uint64_t Timer::watch()
{
  return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - _ss).count();
}

//
// watch_s
// return: sec
//
uint64_t Timer::watch_s()
{
  return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - _ss).count();
}

//
//  sleep_s
//
void Timer::sleep_s(time_t _t)
{
  std::this_thread::sleep_for(std::chrono::seconds(_t));
}

//
//  sleep_s
//
void Timer::sleep_u(uint64_t _t)
{
  std::this_thread::sleep_for(std::chrono::microseconds(_t));  
}

}  // namespace CLGM