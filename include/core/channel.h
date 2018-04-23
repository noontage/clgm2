#pragma once
#include <common.h>
#include <core/connection.h>

namespace CLGM2
{

class Channel
{
  Array<Sptr<Connection>> connections;
  static Array<Channel *> channels;

public:
  String name;

  Channel();
  Channel(const String &n);
  ~Channel();
  bool join(const Sptr<Connection> &connection);
  bool leave(const Sptr<Connection> &connection);
  void send(const String &message);
  void send(const char *message, size_t length);
  Sptr<Connection> find_by_connection(const Sptr<Connection> &connection);
  static void leave_from_all_channel(const Sptr<Connection> &connection);
};

} // namespace CLGM2