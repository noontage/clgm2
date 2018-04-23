#pragma once
#include <common.h>
#include <core/timer.h>

namespace CLGM2
{

class ConnectionManager;

class Connection
{
  void *rawptr;

public:
  uint64_t id;
  struct
  {
    String url;
    String origin;
    String host;
    String user_agent;
    bool established;
  } request;
  Timer last_update;

  Connection();
  Connection(const Connection &) = delete;
  friend ConnectionManager;
  void send(const String &message);
  void send(const char *message, size_t length);
};

} // namespace CLGM2