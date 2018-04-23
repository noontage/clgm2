#pragma once
#include <common.h>
#include <core/connection.h>
#include <core/channel.h>

namespace CLGM2
{

class ConnectionManager
{
  void *rawptr;
  Hash<Connection *, Sptr<Connection>> connections;

public:
  ConnectionManager();
  ~ConnectionManager();
  void listen(uint16_t port);
  
  Function<void(Sptr<Connection>)> cb_connect;
  Function<void(Sptr<Connection>, int, const String&)> cb_disconnect;
  Function<void(Sptr<Connection>, const String &)> cb_message;
};

} // namespace CLGM2
