#include <common.h>
#include <core/sptable.h>
#include <core/connection.h>
#include <core/channel.h>

#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/compile.h>
#include <mruby/data.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/hash.h>

namespace CLGM2
{

struct _mrb_value_hash
{
  size_t operator()(const mrb_value &m_val) const { return reinterpret_cast<size_t>(DATA_PTR(m_val)); }
};
struct _mrb_value_eq
{
  bool operator()(const mrb_value &left, const mrb_value &right) const { return DATA_PTR(left) == DATA_PTR(right); }
};

struct MRB
{
  // mruby
  mrb_state *mrb;

  // Table
  // Table Shared_ptr<Connection> <=> mrb_value
  SPTable<Connection, mrb_value, _mrb_value_hash, _mrb_value_eq> tbl_connections;
  // Table Shared_ptr<Channel>    <=> mrb_value
  SPTable<Channel, mrb_value, _mrb_value_hash, _mrb_value_eq> tbl_channels;

  //  custon 'Connection' ruby class
  mrb_value custom_connection_class;

  //
  // constant data type
  //
  const mrb_data_type DS_CONNECTION = {"Connection", mrb_free};

  //
  // member var
  //
  static Hash<mrb_state *, MRB *> Tbl;
  Hash<mrb_sym, mrb_value> rb_core_event_proc;
  RClass *KCore;
  RClass *KConnection;
  RClass *KChannel;

  //
  // obj_new
  // T(something pointor) => rb_object(new)
  //
  template <class T>
  mrb_value obj_new(RClass *klass, const mrb_data_type *type, T p)
  {
    mrb_value value = mrb_obj_new(mrb, klass, 0, NULL);
    DATA_PTR(value) = p;
    DATA_TYPE(value) = type;
    return value;
  }

  //
  // call_event_proc
  //
  template <class... ARGS>
  mrb_value call_event_proc(const String &event, ARGS... args)
  {
    mrb_value ret;
    int ai = mrb_gc_arena_save(mrb);
    mrb_value rb_event = rb_core_event_proc[mrb_intern_static(mrb, event.c_str(), event.length())];
    mrb_gc_arena_restore(mrb, ai);
    if (!mrb_nil_p(rb_event))
    {
      ret = mrb_funcall(mrb, rb_event, "call", sizeof...(args), args...);
      if (is_error())
      {
        std::cout << "Event Call Error" << std::endl;
      }
    }
    else
    {
      std::cout << "Not defined: " + event << std::endl;
    }
    return ret;
  }

  //
  // member func
  //
  MRB();
  ~MRB();
  void initialize();
  bool exec_script(const String &f);
  bool is_error();
};

} // namespace CLGM2