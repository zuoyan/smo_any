#ifndef FILE_EF57AD14_CB37_4B3E_80D3_FC5EE248EFC2_H
#define FILE_EF57AD14_CB37_4B3E_80D3_FC5EE248EFC2_H
#include "smo_any/types.hpp"
#include "smo_any/concept/increment.hpp"
#include "smo_any/concept/equal.hpp"
#include "smo_any/concept/copy_construct.hpp"
#include "smo_any/concept/dereference.hpp"

namespace smo_any {

template <class D = std::ptrdiff_t>
struct concept_distance : smo_any::concepts<> {
  template <class T>
  static inline D type_func(const T &a, const T &b) {
    return std::distance(a, b);
  }

  struct Table {
    D (*func)(const void *a, const void *b);
    template <class T>
    void reify() {
      func = reinterpret_cast<D (*)(const void *, const void *)>(type_func<T>);
    }
  };

  template <class Derived>
  struct Model {
    template <class A>
    inline friend typename std::enable_if<!std::is_same<A, Derived>::value,
                                          D>::type
    distance(const A &a, const Derived &b) {
      assert(a.table()->type_info == b.table()->type_info);
      const Table *at = a.table();
      return at->func(a.data(), b.data());
    }

    template <class B>
    inline friend D distance(const Derived &a, const B &b) {
      assert(a.table()->type_info == b.table()->type_info);
      const Table *at = a.table();
      return at->func(a.data(), b.data());
    }
  };
};

template <class V, class R = const V &>
struct concept_forward_iterator
    : concepts<concept_const_dereference<R(void)>, concept_increment,
               concept_distance<>, concept_copy_construct, concept_equal> {};

template <class V, class R = const V &>
using any_forward_iterator = any<concept_forward_iterator<V, R>>;

}  // namespace smo_any
#endif // FILE_EF57AD14_CB37_4B3E_80D3_FC5EE248EFC2_H
