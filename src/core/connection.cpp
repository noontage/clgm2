#include <common.h>
#include <core/connection.h>
#include <uWS.h>
namespace CLGM2 {

//
// constructor
//

Connection::Connection(){}

//
// send
//
void Connection::send(const String& message)
{
  send(message.c_str(), message.length());
}

//
// send
//
void Connection::send(const char* message, size_t length)
{
  auto ws = static_cast<uWS::WebSocket<uWS::SERVER>*>(rawptr);
  ws->send(message, length, uWS::OpCode::TEXT);
}

}  // namespace CLGM2