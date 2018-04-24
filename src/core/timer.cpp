/**
 * @file time.cpp
 * @brief Timer Measure for Cross platform
 * @author noontage
 */
#include <core/timer.h>
#include <chrono>

namespace CLGM2
{

Timer::Timer()
{
  reset();
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
  _ss = _ss - std::chrono::hours(24);
}

//
// watch
// return: ms
//
uint64_t Timer::watch_ms()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - _ss).count();
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
//  sleep_ms
//
void Timer::sleep_ms(time_t _t)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(_t));
}

//
//  sleep_us
//
void Timer::sleep_us(uint64_t _t)
{
  std::this_thread::sleep_for(std::chrono::microseconds(_t));
}

} // namespace CLGM