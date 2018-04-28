#include <glue/mruby_vm.h>
#include <glue/mruby_impl.h>

#include <core/connection.h>
#include <core/connection_manager.h>
#include <core/channel.h>
#include <core/timer.h>

namespace CLGM2
{
namespace MRubyImpl
{

/**************************************************************************************************************************************
 * Core
 **************************************************************************************************************************************/

//
// rbf_event_on
//
mrb_value rbf_event_on(mrb_state *mrb, mrb_value self)
{
  mrb_value event;
  mrb_value block;
  mrb_get_args(mrb, "n&", &event, &block);
  uint32_t sym = mrb_fixnum(event);

  // registered
  mrb_gc_register(mrb, block); //  GC protect 'block'
  MRB::Tbl[mrb]->rb_core_event_proc[sym] = block;
  return self;
}

//
// rbf_sleep
//
mrb_value rbf_sleep(mrb_state *mrb, mrb_value self)
{
  mrb_int st;
  mrb_sym type;
  mrb_get_args(mrb, "i|n", &st, &type);

  if (type == mrb_intern_lit(mrb, "ms"))
  {
    Timer::sleep_ms(st);
  }
  else if (type == mrb_intern_lit(mrb, "us"))
  {
    Timer::sleep_us(st);
  }
  else
  {
    Timer::sleep_s(st);
  }

  return self;
}

//
// rbf_fullgc
//
mrb_value rbf_fullgc(mrb_state *mrb, mrb_value self)
{
  mrb_full_gc(mrb);
  return self;
}

/**************************************************************************************************************************************
 * Connection
 **************************************************************************************************************************************/

//
// rbf_connection_send
//
mrb_value rbf_connection_send(mrb_state *mrb, mrb_value self)
{
  char *message;
  mrb_get_args(mrb, "z", &message);

  auto connection = MRB::Tbl[mrb]->tbl_connections.get(self);
  connection->send(String(message));
  return self;
}

//
// rbf_connection_info
//
mrb_value rbf_connection_info(mrb_state *mrb, mrb_value self)
{
  auto connection = MRB::Tbl[mrb]->tbl_connections.get(self);
  if (connection == nullptr)
  {
    return mrb_nil_value();
  }
  mrb_value hash = mrb_hash_new_capa(mrb, 4);
  mrb_hash_set(mrb, hash, mrb_str_new(mrb, "id", 2), mrb_fixnum_value(connection->id));
  mrb_hash_set(mrb, hash, mrb_str_new(mrb, "url", 3), mrb_str_new(mrb, connection->request.url.c_str(), connection->request.url.size()));
  mrb_hash_set(mrb, hash, mrb_str_new(mrb, "host", 4), mrb_str_new(mrb, connection->request.host.c_str(), connection->request.host.size()));
  mrb_hash_set(mrb, hash, mrb_str_new(mrb, "origin", 6), mrb_str_new(mrb, connection->request.origin.c_str(), connection->request.origin.size()));
  mrb_hash_set(mrb, hash, mrb_str_new(mrb, "user_agent", 10), mrb_str_new(mrb, connection->request.user_agent.c_str(), connection->request.user_agent.size()));
  return hash;
}

/**************************************************************************************************************************************
 * ConnectionManager
 **************************************************************************************************************************************/

//
// rbf_cm_connection_class_eq
//
mrb_value rbf_cm_connection_class_eq(mrb_state *mrb, mrb_value self)
{
  auto VM = MRB::Tbl[mrb];
  mrb_value rb_class;
  mrb_get_args(mrb, "C", &rb_class);

  // check kind of Class object
  if (!mrb_obj_is_kind_of(mrb, rb_class, mrb->class_class))
  {
    mrb_raise(mrb, E_TYPE_ERROR, "wrong argument class");
  }

  // check kind of Connection object
  if (!mrb_obj_is_kind_of(mrb, mrb_funcall(mrb, rb_class, "new", 0), mrb_class_get_under(mrb, MRB::Tbl[mrb]->KCore, "Connection")))
  {
    mrb_raise(mrb, E_TYPE_ERROR, "wrong argument class (require: kind of 'CLGM2::Connection' class)");
  }

  // GC unregister if before set custom connection class
  if (!mrb_nil_p(VM->custom_connection_class))
  {
    mrb_gc_unregister(mrb, VM->custom_connection_class);
  }
  VM->custom_connection_class = rb_class;
  mrb_gc_register(mrb, rb_class);

  return self;
}

//
// rbf_cm_listen
//
mrb_value rbf_cm_listen(mrb_state *mrb, mrb_value self)
{
  mrb_int port;
  mrb_int type;
  mrb_get_args(mrb, "i|n", &port, &type);

  if (port > 0 && port < 65536)
  {
    auto VM = MRB::Tbl[mrb];
    std::thread th_net([&]() {
      VM->connection_manager->listen(port);
    });
    if (type == mrb_intern_lit(mrb, "no_blocking"))
    {
      th_net.detach();
    }
    else
    {
      th_net.join();
    }
  }
  else
  {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "wrong argument range of number");
  }

  return self;
}

/**************************************************************************************************************************************
 * Channel
 **************************************************************************************************************************************/

//
// rbf_channel_init
//
mrb_value rbf_channel_init(mrb_state *mrb, mrb_value self)
{
  DATA_TYPE(self) = &DS_CHANNEL;
  DATA_PTR(self) = nullptr;

  //---
  char *room_name;
  mrb_get_args(mrb, "z", &room_name);
  DATA_PTR(self) = new Channel(String(room_name));
  //---

  return self;
}

//
// rbf_channel_init_copy
//
mrb_value rbf_channel_init_copy(mrb_state *mrb, mrb_value copy)
{
  mrb_raise(mrb, E_RUNTIME_ERROR, "This object not permited to duplicate");
  return copy;
}

//
// rbf_channel_fin
//
void rbf_channel_fin(mrb_state *mrb, void *p)
{
  auto p_channel = static_cast<Channel *>(p);
  std::cout << "Channel: Ruby<Channel:" + String(p_channel->name) + "> deleted" << std::endl;
  delete p_channel;
}

//
// rbf_channel_join
//
mrb_value rbf_channel_join(mrb_state *mrb, mrb_value self)
{
  mrb_value rb_connection;
  mrb_get_args(mrb, "o", &rb_connection);
  if (!mrb_obj_is_kind_of(mrb, rb_connection, MRB::Tbl[mrb]->KConnection))
  {
    mrb_raise(mrb, E_TYPE_ERROR, "wrong argument class");
  }
  auto channel = static_cast<Channel *>(DATA_PTR(self));
  auto connection = MRB::Tbl[mrb]->tbl_connections.get(rb_connection);
  channel->join(connection);

  return self;
}

//
// rbf_channel_leave
//
mrb_value rbf_channel_leave(mrb_state *mrb, mrb_value self)
{
  mrb_value rb_connection;
  mrb_get_args(mrb, "o", &rb_connection);
  if (!mrb_obj_is_kind_of(mrb, rb_connection, MRB::Tbl[mrb]->KConnection))
  {
    mrb_raise(mrb, E_TYPE_ERROR, "wrong argument class");
  }
  auto channel = static_cast<Channel *>(DATA_PTR(self));
  auto connection = MRB::Tbl[mrb]->tbl_connections.get(rb_connection);
  channel->leave(connection);

  return self;
}

//
// rbf_channel_name
//
mrb_value rbf_channel_name(mrb_state *mrb, mrb_value self)
{
  auto channel = static_cast<Channel *>(DATA_PTR(self));
  return mrb_str_new(mrb, channel->name.c_str(), channel->name.length());
}

//
// rbf_channel_send
//
mrb_value rbf_channel_send(mrb_state *mrb, mrb_value self)
{
  char *message;
  mrb_get_args(mrb, "z", &message);

  auto channel = static_cast<Channel *>(DATA_PTR(self));
  channel->send(String(message));

  return self;
}

//
// initialize
//
void initialize(mrb_state *mrb)
{
  // Core
  auto KCore = mrb_define_module(mrb, "CLGM2");
  MRB::Tbl[mrb]->KCore = KCore;
  mrb_define_class_method(mrb, KCore, "event_on", rbf_event_on, MRB_ARGS_REQ(2));
  mrb_define_class_method(mrb, KCore, "sleep", rbf_sleep, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, KCore, "fullgc", rbf_fullgc, MRB_ARGS_NONE());
  //mrb_define_const(mrb, KCore, "Environment", mrb_str_new(mrb, "", 0));
  mrb_define_const(mrb, KCore, "VERSION", mrb_str_new(mrb, Version::Pretty.c_str(), Version::Pretty.length()));
  MRB_SET_INSTANCE_TT(KCore, MRB_TT_DATA); // for GC

  // ConnectionManager
  auto KConnectionManager = mrb_define_class_under(mrb, KCore, "ConnectionManager", mrb->object_class);
  MRB::Tbl[mrb]->KConnectionManager = KConnectionManager;
  mrb_define_class_method(mrb, KConnectionManager, "connection_class=", rbf_cm_connection_class_eq, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, KConnectionManager, "listen", rbf_cm_listen, MRB_ARGS_REQ(1));

  // Connection
  auto KConnection = mrb_define_class_under(mrb, KCore, "Connection", mrb->object_class);
  MRB::Tbl[mrb]->KConnection = KConnection;
  mrb_define_method(mrb, KConnection, "send", rbf_connection_send, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, KConnection, "info", rbf_connection_info, MRB_ARGS_NONE());
  MRB_SET_INSTANCE_TT(KConnection, MRB_TT_DATA); // for GC

  // Channel
  auto KChannel = mrb_define_class_under(mrb, KCore, "Channel", mrb->object_class);
  MRB::Tbl[mrb]->KChannel = KChannel;
  mrb_define_method(mrb, KChannel, "initialize", rbf_channel_init, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, KChannel, "initialize_copy", rbf_channel_init_copy, MRB_ARGS_NONE());
  mrb_define_method(mrb, KChannel, "join", rbf_channel_join, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, KChannel, "leave", rbf_channel_leave, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, KChannel, "name", rbf_channel_name, MRB_ARGS_NONE());
  mrb_define_method(mrb, KChannel, "send", rbf_channel_send, MRB_ARGS_REQ(1));
  MRB_SET_INSTANCE_TT(KChannel, MRB_TT_DATA); // for GC
}

} // RubyFunction
} // namespace CLGM2