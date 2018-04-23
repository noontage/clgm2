/*
** timer.h - Chrono Wrapper.
**
*/
#pragma once
#include <common.h>

namespace CLGM2 {
class Timer
{
  std::chrono::system_clock::time_point _ss;
public:
  Timer();
  ~Timer();
  void begin();
  void reset();
  uint64_t watch_ms();
  uint64_t watch_s();
  static void sleep_s(time_t _t);
  static void sleep_u(uint64_t _t);
};

}  // namespace CLGM2