#include <clgm2.h>

using namespace CLGM2;

int main()
{
  auto cm = std::make_shared<ConnectionManager>();
  auto vm = std::make_shared<MRB>();

  cm->cb_connect = ([&](Sptr<Connection> con) {
    auto rb_connection = vm->obj_new<Connection *>(vm->KConnection, &vm->DS_CONNECTION, con.get());
    mrb_gc_register(vm->mrb, rb_connection); //  protect GC
    vm->tbl_connections.set(con, rb_connection);
    vm->call_event_proc("connected", rb_connection);
  });

  cm->cb_message = ([&](Sptr<Connection> con, const String &message) {
    vm->call_event_proc("message", vm->tbl_connections.get(con), mrb_str_new(vm->mrb, message.c_str(), message.length()));
  });

  cm->cb_disconnect = ([&](Sptr<Connection> con, int code, const String &r) {
    mrb_value rb_connection = vm->tbl_connections.get(con);
    vm->call_event_proc("disconnected", rb_connection, mrb_fixnum_value(code), mrb_str_new(vm->mrb, r.c_str(), r.length()));
    mrb_gc_unregister(vm->mrb, rb_connection);
    vm->tbl_connections.remove(con);
  });

  std::thread th_net([&]() {
    cm->listen(3000);
  });

  if (vm->exec_script("boot.rb"))
  {
    th_net.join();
  }

  exit(1);
}