#include "core/channel.h"
#include "core/connection.h"
namespace CLGM2
{

Array<Channel *> Channel::channels;

//***********************************************************************************************************************************
// @constructor
//***********************************************************************************************************************************
Channel::Channel()
{
  channels.push_back(this);
}

Channel::Channel(const String &n)
{
  this->name = n;
  channels.push_back(this);
}

Channel::~Channel()
{
  channels.erase(std::find(channels.begin(), channels.end(), this));
}

//***********************************************************************************************************************************
// @join
//***********************************************************************************************************************************
bool Channel::join(const Sptr<Connection> &connection)
{
  if (!find_by_connection(connection))
  {
    connections.push_back(connection);
    return true;
  }
  return false;
}

//***********************************************************************************************************************************
// @leave
//***********************************************************************************************************************************
bool Channel::leave(const Sptr<Connection> &connection)
{
  if (find_by_connection(connection))
  {
    connections.erase(std::find(connections.begin(), connections.end(), connection));
    return true;
  }
  return false;
}

//***********************************************************************************************************************************
// @leave_all
//***********************************************************************************************************************************
void Channel::leave_from_all_channel(const Sptr<Connection> &connection)
{
  for (auto channel : channels)
  {
    if (channel->find_by_connection(connection))
    {
      std::cout << "Connection: shared_ptr<Connection:" + std::to_string(connection->id) + "> auto left in channel" << std::endl;
      channel->leave(connection);
    }
  }
}

//***********************************************************************************************************************************
// @find_by_connection
//***********************************************************************************************************************************
Sptr<Connection> Channel::find_by_connection(const Sptr<Connection> &connection)
{
  auto result = std::find(connections.begin(), connections.end(), connection);
  if (result == connections.end())
  {
    return nullptr;
  }
  return *result;
}

//***********************************************************************************************************************************
// @send
//***********************************************************************************************************************************
void Channel::send(const String &message)
{
  send(message.c_str(), message.length());
}

//***********************************************************************************************************************************
// @send
//***********************************************************************************************************************************
void Channel::send(const char *message, size_t length)
{
  for (auto connection : connections)
  {
    connection->send(message, length);
  }
}

} // namespace CLGM2