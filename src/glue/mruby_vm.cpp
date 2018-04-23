#include <glue/mruby_vm.h>
namespace CLGM2
{

//
// constructor
//
MRuby::MRuby()
{
  mrb = mrb_open();
  // ----------- ARENA BEGN  -----------
  int ai = mrb_gc_arena_save(mrb);
  initialize();
  custom_connection_class = mrb_nil_value();
  mrb_gc_arena_restore(mrb, ai);
  // ----------- ARENA END  -----------
}

//
// destructor
//
MRuby::~MRuby()
{
  mrb_close(mrb);
}

//
// initialize
//
void MRuby::initialize()
{
}

//
// exec_script
//
bool MRuby::exec_script(const String &f)
{
  // loadfile
  FILE *mrb_file;
  if ((mrb_file = fopen(f.c_str(), "r")) == nullptr)
  {
    fclose(mrb_file);
    return false;
  }
  auto parser_state = mrb_parse_file(mrb, mrb_file, nullptr);
  fclose(mrb_file);

  // procedure
  auto rproc = mrb_generate_code(mrb, parser_state);
  if (rproc == nullptr)
  {
    mrb_pool_close(parser_state->pool);
    return false;
  }
  mrb_pool_close(parser_state->pool);

  // run
  mrb_run(mrb, rproc, mrb_top_self(mrb));
  if (is_error())
  {
    return false;
  }
  return true;
}

bool MRuby::is_error()
{
  if (mrb->exc)
  {
    //struct RString *rstr = mrb_str_ptr(mrb_funcall(mrb, mrb_obj_value(mrb->exc), "inspect", 0));
    mrb_print_error(mrb);
    return true;
  }
  return false;
}

} // namespace CLGM2