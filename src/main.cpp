#include <clgm2.h>

using namespace CLGM2;

int main()
{
  auto cm = std::make_shared<ConnectionManager>();

  auto room = new Channel("test");

  cm->set_listner_on_connect([&](Sptr<Connection> con) {
    room->join(con);
  });

  cm->set_listner_on_message([&](Sptr<Connection> con, const String &message) {
    room->send(message);
  });

  cm->listen(3000);
}