#include <clgm2.h>

using namespace CLGM2;

MRB *vm;

int main()
{
  auto cm = std::make_shared<ConnectionManager>();
  vm = new MRB();
  auto global = new Channel("Global");

  cm->cb_connect = ([&](Sptr<Connection> con) {
    global->join(con);

    auto rb_connection = vm->obj_new<Connection *>(vm->KConnection, &vm->DS_CONNECTION, con.get());
    mrb_gc_register(vm->mrb, rb_connection); //  protect GC
    vm->call_event_proc("connected", rb_connection);
    vm->tbl_connections.set(con, rb_connection);
  });

  cm->cb_message = ([&](Sptr<Connection> con, const String &message) {
    global->send(message);
  });

  cm->cb_disconnect = ([&](Sptr<Connection> con, int code, const String &) {
    global->leave(con);

    mrb_value rb_connection = vm->tbl_connections.get(con);
    vm->call_event_proc("disconnected", rb_connection, mrb_fixnum_value(code));
    mrb_gc_unregister(vm->mrb, rb_connection);
    vm->tbl_connections.remove(con);
  });

  std::thread net([&]() {
    cm->listen(3000);
    delete vm;
  });

  vm->exec_script("boot.rb");
  net.join();
}