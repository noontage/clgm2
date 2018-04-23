#include <common.h>
#include <core/sptable.h>
#include <core/connection.h>
#include <core/channel.h>

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/compile.h>
#include <mruby/data.h>
#include <mruby/string.h>
#include <mruby/variable.h>

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

class MRuby
{
  mrb_state *mrb;
  Hash<mrb_sym, mrb_value> rb_core_event_proc;                                    // Event Procedur
  SPTable<Connection, mrb_value, _mrb_value_hash, _mrb_value_eq> tbl_connections; // Table Shared_ptr<Connection> <=> mrb_value
  SPTable<Channel, mrb_value, _mrb_value_hash, _mrb_value_eq> tbl_channels;       // Table Shared_ptr<Channel>    <=> mrb_value
  mrb_value custom_connection_class;                                              //  custon 'Connection' ruby class

  void initialize();
public:
  MRuby();
  ~MRuby();
  bool exec_script(const String &f);
  bool is_error();
};

} // namespace CLGM2