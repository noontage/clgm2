#include <glue/mruby_vm.h>
#include <glue/mruby_impl.h>
namespace CLGM2
{

Hash<mrb_state *, MRB *> MRB::Tbl;

//
// constructor
//
MRB::MRB()
{
  mrb = mrb_open();
  Tbl[mrb] = this;
  custom_connection_class = mrb_nil_value();
  // ----------- ARENA BEGN  -----------
  int ai = mrb_gc_arena_save(mrb);
  initialize();
  mrb_gc_arena_restore(mrb, ai);
  // ----------- ARENA END  -----------
}

//
// destructor
//
MRB::~MRB()
{
  mrb_close(mrb);
  Tbl.erase(mrb);
}

//
// exec_script
//
int MRB::exec_script(const String &f)
{
  // loadfile
  FILE *mrb_file;
  if ((mrb_file = fopen(f.c_str(), "r")) == nullptr)
  {
    fclose(mrb_file);
    return 1;
  }
  auto parser_state = mrb_parse_file(mrb, mrb_file, nullptr);
  fclose(mrb_file);

  // procedure
  auto rproc = mrb_generate_code(mrb, parser_state);
  if (rproc == nullptr)
  {
    mrb_pool_close(parser_state->pool);
    return 1;
  }
  mrb_pool_close(parser_state->pool);

  // run
  mrb_run(mrb, rproc, mrb_top_self(mrb));
  if(is_error()){
    return 1;
  }
  return 0;
}

bool MRB::is_error()
{
  if (mrb->exc)
  {
    //struct RString *rstr = mrb_str_ptr(mrb_funcall(mrb, mrb_obj_value(mrb->exc), "inspect", 0));
    mrb_print_error(mrb);
    return true;
  }
  return false;
}

//
// initialize
//
void MRB::initialize()
{
  MRubyImpl::initialize(mrb);
}

} // namespace CLGM2