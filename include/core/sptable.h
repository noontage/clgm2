#pragma once
#include <common.h>

namespace CLGM2
{

//
// Shared pointor table
// desc: shared_ptr または T2 をキーとして相互管理するためのテーブルです。
//
template <class T1, class T2, class... ARGS>
struct SPTable
{
  Hash<Sptr<T1>, T2> obj_container;           // key:shared_ptr<T1> value:T2
  Hash<T2, Sptr<T1>, ARGS...> sptr_container; // key:T2 value:shared_ptr<T1> ...hash function...
  Hash<T1 *, Sptr<T1>> ptr_to_sptr_container; // ptr => sptr

  // get
  T2 get(const Sptr<T1> &sptr) { return obj_container[sptr]; }
  Sptr<T1> get(T2 ptr) { return sptr_container[ptr]; }
  Sptr<T1> get(T1 *sptr_ptr) { return ptr_to_sptr_container[sptr_ptr]; }

  // set
  // retrun: pair of address of container data
  std::pair<Sptr<T1> *, T2 *> set(const Sptr<T1> &sptr, T2 ptr)
  {
    sptr_container[ptr] = sptr;
    obj_container[sptr] = ptr;
    ptr_to_sptr_container[static_cast<T1 *>(sptr.get())] = sptr;
    return std::make_pair(&(sptr_container[ptr]), &(obj_container[sptr]));
  }

  // remove
  // return: true=success
  bool remove(const Sptr<T1> &sptr)
  {
    T2 ptr = get(sptr);
    return remove(sptr, ptr);
  }
  bool remove(T2 ptr)
  {
    Sptr<T1> sptr = get(ptr);
    return remove(sptr, ptr);
  }
  bool remove(const Sptr<T1> &sptr, T2 ptr) { return obj_container.erase(sptr) + sptr_container.erase(ptr) + ptr_to_sptr_container.erase(static_cast<T1 *>(sptr.get())) == 3 ? true : false; }
};

} // namespace CLGM2