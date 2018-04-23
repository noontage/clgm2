#include <glue/mruby_vm.h>
#include <glue/mruby_impl.h>
namespace CLGM2
{
namespace MRubyImpl
{

//
// _event_on
//
mrb_value rb_event_on(mrb_state *mrb, mrb_value self)
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
// initialize
//
void initialize(mrb_state *mrb)
{
  // Core
  auto KCore = mrb_define_module(mrb, "CLGM2");
  mrb_define_class_method(mrb, KCore, "event_on", rb_event_on, MRB_ARGS_REQ(2));
  // mrb_define_class_method(mrb, KCore, "sleep", rb_sleep, MRB_ARGS_REQ(1));
  // mrb_define_class_method(mrb, KCore, "fullgc", rb_fullgc, MRB_ARGS_NONE());
  // mrb_define_class_method(mrb, KCore, "shutdown", rb_shutdown, MRB_ARGS_NONE());

  // Connection
  MRB::Tbl[mrb]->KConnection = mrb_define_class(mrb, "Connection", mrb->object_class);

  // Constant
  mrb_define_const(mrb, KCore, "Environment", mrb_str_new(mrb, "", 0));
}

} // RubyFunction
} // namespace CLGM2