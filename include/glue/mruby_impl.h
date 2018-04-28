#pragma once
#include <mruby.h>
namespace CLGM2
{

namespace MRubyImpl
{
  void rbf_channel_fin(mrb_state *mrb, void *p);
  const mrb_data_type DS_CHANNEL = {"Channel", rbf_channel_fin};  
  void initialize(mrb_state *mrb);
}

} // namespace CLGM2