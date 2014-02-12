#ifndef FILE_EF57AD14_CB37_4B3E_80D3_FC5EE248EFC2_H
#define FILE_EF57AD14_CB37_4B3E_80D3_FC5EE248EFC2_H
#include "smo_any/types.hpp"
#include "smo_any/concept/increment.hpp"
#include "smo_any/concept/decrement.hpp"
#include "smo_any/concept/equal.hpp"
#include "smo_any/concept/copy_construct.hpp"
#include "smo_any/concept/dereference.hpp"
#include "smo_any/concept/plus.hpp"
#include "smo_any/concept/minus.hpp"
#include "smo_any/concept/offset_dereference.hpp"

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

template <class D = std::ptrdiff_t>
struct concept_advance : concepts<> {
  template <class T>
  static void type_func(const T &a, const D &n, T *d) {
    new (d) T(a + n);
  };


  struct Table {
    void (*func)(const void *a, const D &, void *);
    template <class T>
    void reify() {
      func = reinterpret_cast<decltype(func)>(type_func<T>);
    }
  };

  template <class Derived>
  struct Model {
    inline friend Derived advance(const Derived &a, const D &n) {
      auto r = a.raw_like();
      const Table *table = a.table();
      table->func(a.data(), n, r.data());
      return r;
    }
  };
};

template <class V, class R = const V &>
struct concept_forward_iterator
    : concepts<concept_const_indirect<R(void)>,
               concept_const_dereference<
                   typename std::remove_reference<R>::type *(void)>,
               concept_increment, concept_distance<>, concept_copy_construct,
               concept_equal> {};

template <class V, class R = const V &>
using any_forward_iterator = any<concept_forward_iterator<V, R>>;

template <class V, class R = const V &>
struct concept_bidirectional_iterator
    : concepts<concept_forward_iterator<V, R>, concept_decrement> {};

template <class V, class R = const V &>
using any_bidirectional_iterator = any<concept_bidirectional_iterator<V, R>>;

template <class V, class R = const V &, class D = std::ptrdiff_t>
struct concept_random_access_iterator
    : concepts<concept_bidirectional_iterator<V, R>, concept_plus<D>,
               concept_plus_assign<D>, concept_minus<D, self>,
               concept_minus_assign<D>,
               concept_const_offset_dereference<R(D)>> {};

template <class V, class R = const V &, class D = std::ptrdiff_t>
using any_random_access_iterator = any<concept_random_access_iterator<V, R, D>>;

}  // namespace smo_any
#endif // FILE_EF57AD14_CB37_4B3E_80D3_FC5EE248EFC2_H
