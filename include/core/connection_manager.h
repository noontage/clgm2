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
  Function<void(Sptr<Connection>)> cb_connect;
  Function<void(Sptr<Connection>, int, String)> cb_disconnect;
  Function<void(Sptr<Connection>, const String &)> cb_message;

public:
  ConnectionManager();
  ~ConnectionManager();
  void listen(uint16_t port);
  void set_listner_on_connect(Function<void(Sptr<Connection>)> cb);
  void set_listner_on_disconnect(Function<void(Sptr<Connection>, int, String)> cb);
  void set_listner_on_message(Function<void(Sptr<Connection>, const String &)> cb);
};

} // namespace CLGM2
