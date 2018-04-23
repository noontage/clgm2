#include <glue/mruby_vm.h>
#include <glue/mruby_impl.h>

#include <core/connection.h>
#include <core/connection_manager.h>
#include <core/channel.h>

namespace CLGM2
{
namespace MRubyImpl
{

//
// Data Type
//
void rbf_channel_fin(mrb_state *mrb, void *p);
const mrb_data_type DS_CHANNEL = {"Channel", rbf_channel_fin};

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
  int ai = mrb_gc_arena_save(mrb);
  //---
  auto connection = MRB::Tbl[mrb]->tbl_connections.get(self);
  if (connection == nullptr)
  {
    printf("not found");
    return mrb_nil_value();
  }
  mrb_value hash = mrb_hash_new_capa(mrb, 4);
  mrb_hash_set(mrb, hash, mrb_str_new(mrb, "id", 2), mrb_fixnum_value(connection->id));
  mrb_hash_set(mrb, hash, mrb_str_new(mrb, "url", 3), mrb_str_new(mrb, connection->request.url.c_str(), connection->request.url.size()));
  mrb_hash_set(mrb, hash, mrb_str_new(mrb, "host", 4), mrb_str_new(mrb, connection->request.host.c_str(), connection->request.host.size()));
  mrb_hash_set(mrb, hash, mrb_str_new(mrb, "origin", 6), mrb_str_new(mrb, connection->request.origin.c_str(), connection->request.origin.size()));
  mrb_hash_set(mrb, hash, mrb_str_new(mrb, "user_agent", 10), mrb_str_new(mrb, connection->request.user_agent.c_str(), connection->request.user_agent.size()));
  //---
  mrb_gc_arena_restore(mrb, ai);
  return hash;
}

//
// rbf_channel_init
//
mrb_value rbf_channel_init(mrb_state *mrb, mrb_value self)
{
  DATA_TYPE(self) = &DS_CHANNEL;
  DATA_PTR(self) = nullptr;

  int ai = mrb_gc_arena_save(mrb);
  //---
  char *room_name;
  mrb_get_args(mrb, "z", &room_name);
  DATA_PTR(self) = new Channel(String(room_name));
  //---
  mrb_gc_arena_restore(mrb, ai);

  return self;
}

//
// rbf_channel_fin
//
void rbf_channel_fin(mrb_state *mrb, void *p)
{
  auto p_channel = static_cast<Channel *>(p);
  delete p_channel;
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
  mrb_define_class_method(mrb, KCore, "event_on", rbf_event_on, MRB_ARGS_REQ(2));
  // mrb_define_class_method(mrb, KCore, "sleep", rb_sleep, MRB_ARGS_REQ(1));
  // mrb_define_class_method(mrb, KCore, "fullgc", rb_fullgc, MRB_ARGS_NONE());
  // mrb_define_class_method(mrb, KCore, "shutdown", rb_shutdown, MRB_ARGS_NONE());

  // Connection
  MRB::Tbl[mrb]->KConnection = mrb_define_class(mrb, "Connection", mrb->object_class);
  mrb_define_method(mrb, MRB::Tbl[mrb]->KConnection, "send", rbf_connection_send, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, MRB::Tbl[mrb]->KConnection, "info", rbf_connection_info, MRB_ARGS_NONE());

  // Channel
  MRB::Tbl[mrb]->KChannel = mrb_define_class(mrb, "Channel", mrb->object_class);
  mrb_define_method(mrb, MRB::Tbl[mrb]->KChannel, "initialize", rbf_channel_init, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, MRB::Tbl[mrb]->KChannel, "initialize_copy", rbf_channel_init_copy, MRB_ARGS_NONE());
  mrb_define_method(mrb, MRB::Tbl[mrb]->KChannel, "join", rbf_channel_join, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, MRB::Tbl[mrb]->KChannel, "leave", rbf_connection_send, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, MRB::Tbl[mrb]->KChannel, "name", rbf_channel_name, MRB_ARGS_NONE());
  mrb_define_method(mrb, MRB::Tbl[mrb]->KChannel, "send", rbf_channel_send, MRB_ARGS_REQ(1));

  // Constant
  mrb_define_const(mrb, KCore, "Environment", mrb_str_new(mrb, "", 0));
}

} // RubyFunction
} // namespace CLGM2