#include <clgm2.h>

using namespace CLGM2;

int main()
{
  auto cm = std::make_shared<ConnectionManager>();
  auto VM = std::make_shared<MRB>();

  cm->cb_connect = ([&](Sptr<Connection> con) {
    // create 'Connection' class
    mrb_value rb_connection;
    if (mrb_nil_p(VM->custom_connection_class))
    {
      rb_connection = VM->obj_new<Connection *>(VM->KConnection, &VM->DS_CONNECTION, con.get());
    }
    else
    {
      rb_connection = mrb_funcall(VM->mrb, VM->custom_connection_class, "new", 0, nullptr);
      DATA_PTR(rb_connection) = con.get();
    }

    // register system
    mrb_gc_register(VM->mrb, rb_connection); //  protect GC
    VM->tbl_connections.set(con, rb_connection);
    VM->call_event_proc("connected", rb_connection);
  });

  cm->cb_message = ([&](Sptr<Connection> con, const String &message) {
    VM->call_event_proc("message", VM->tbl_connections.get(con), mrb_str_new(VM->mrb, message.c_str(), message.length()));
  });

  cm->cb_disconnect = ([&](Sptr<Connection> con, int code, const String &r) {
    mrb_value rb_connection = VM->tbl_connections.get(con);
    VM->call_event_proc("disconnected", rb_connection, mrb_fixnum_value(code), mrb_str_new(VM->mrb, r.c_str(), r.length()));
    mrb_gc_unregister(VM->mrb, rb_connection);
    VM->tbl_connections.remove(con);
  });

  std::thread th_net([&]() {
    cm->listen(3000);
  });

  if (VM->exec_script("boot.rb"))
  {
    th_net.join();
  }

  exit(1);
}