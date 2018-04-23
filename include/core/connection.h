#pragma once
#include <common.h>
namespace CLGM2
{

class ConnectionManager;

class Connection
{
  void *rawptr;

public:
  Connection();
  Connection(const Connection &) = delete;
  friend ConnectionManager;
  void send(const String &message);
  void send(const char *message, size_t length);

  uint64_t id;
  struct
  {
    String url;
    String origin;
    String host;
    String user_agent;
    bool established;
  } request;
};

} // namespace CLGM2