#include <core/connection.h>
#include <core/connection_manager.h>
#include <uWS.h>
namespace CLGM2
{

ConnectionManager::ConnectionManager()
{
  auto h = new uWS::Hub();
  cb_connect = nullptr;
  cb_disconnect = nullptr;
  cb_message = nullptr;

  // onConnection
  h->onConnection([&](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req) {
    auto connection = Sptr<Connection>(new Connection(), [&](Connection *p) {
      std::cout << "Connection: shared_ptr<Connection:" + std::to_string(p->id) + "> deleted" << std::endl;
      delete p;
    });

    // set connection info
    connection->id = reinterpret_cast<uint64_t>(connection.get());
    connection->request.url = req.getUrl().toString();
    connection->request.origin = req.getHeader("origin", 6).toString();
    connection->request.host = req.getHeader("host", 4).toString();
    connection->request.user_agent = req.getHeader("user-agent", 10).toString();

    // remember
    connection->rawptr = static_cast<void *>(ws);
    ws->setUserData(static_cast<void *>(connection.get()));

    // callback
    if (cb_connect)
    {
      cb_connect(connection);
    }

    connections[connection.get()] = connection;
  });

  // onDisconnect
  h->onDisconnection([&](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length) {
    auto connection = connections[static_cast<Connection *>(ws->getUserData())];

    if (cb_disconnect)
    {
      cb_disconnect(connection, code, String(message));
    }

    // remove connection from all channel
    Channel::leave_from_all_channel(connection);

    connections.erase(connection.get());
  });

  // onMessage
  h->onMessage([&](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
    auto connection = connections[static_cast<Connection *>(ws->getUserData())];
    auto accept = true;
    auto watch = connection->last_update.watch_ms();

    if (watch < 1000 / (Config::net_data_receive_rate))
    {
      std::cout << "Too many request: elapsed " + std::to_string(watch) + "ms yet" << std::endl;
      connection->send("{\"code\": 429}");
      accept = false;
    }

    if (opCode != uWS::OpCode::TEXT)
    {
      std::cout << "Protocol Error" << std::endl;
      connection->send("{\"code\": 400}");
      accept = false;
    }
    {
      if (accept && cb_message)
      {
        cb_message(connection, String(message).substr(0, length));
        connection->last_update.begin();
      }
    }
  });

  // onHttpRequest
  h->onHttpRequest([&](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t length, size_t remainingBytes) { res->end("OK", 0); });

  rawptr = static_cast<void *>(h);
}

ConnectionManager::~ConnectionManager()
{
  delete static_cast<uWS::Hub *>(rawptr);
}

void ConnectionManager::listen(uint16_t port)
{
  auto h = static_cast<uWS::Hub *>(rawptr);

  std::cout << "Listen: port(" << port << ")" << std::endl;
  if (h->listen(port))
  {
    h->run();
  }
}

void ConnectionManager::set_listner_on_connect(Function<void(Sptr<Connection>)> cb)
{
  cb_connect = cb;
}

void ConnectionManager::set_listner_on_disconnect(Function<void(Sptr<Connection>, int, String)> cb)
{
  cb_disconnect = cb;
}

void ConnectionManager::set_listner_on_message(Function<void(Sptr<Connection>, const String &)> cb)
{
  cb_message = cb;
}

} // namespace CLGM2