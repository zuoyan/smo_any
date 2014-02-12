#ifndef FILE_083B78A4_0E33_4FE4_84A3_15B8E4B42D94_H
#define FILE_083B78A4_0E33_4FE4_84A3_15B8E4B42D94_H
#include "smo_any/concept/member.hpp"

namespace smo_any {

// SMO_ANY_MEMBER(concept_dereference, operator->, false);
// SMO_ANY_MEMBER(concept_const_dereference, operator->, true);

template <class Sig>
struct concept_dereference;

template <class R>
struct concept_dereference<R()> : concepts<> {
  template <class T>
  static R type_func(T *p) {
    return (*p).operator->();
  }

  struct Table {
    R (*func)(void *);
    template <class T>
    void reify() {
      func = reinterpret_cast<decltype(func)>(type_func<T>);
    }
  };

  template <class Derived>
  struct Model {
    R operator->() {
      auto a = static_cast<Derived &>(*this);
      const Table *table = a.table();
      return table->func(a.data());
    }
  };
};

template <class Sig>
struct concept_const_dereference;

template <class R>
struct concept_const_dereference<R()> : concepts<> {
  template <class T>
  static R type_func(const T *p) {
    return (*p).operator->();
  }

  struct Table {
    R (*func)(const void *);
    template <class T>
    void reify() {
      func = reinterpret_cast<decltype(func)>(type_func<T>);
    }
  };

  template <class Derived>
  struct Model {
    R operator->() const {
      auto a = static_cast<Derived &>(*this);
      const Table *table = a.table();
      return table->func(a.data());
    }
  };
};

SMO_ANY_MEMBER(concept_indirect, operator*, false);
SMO_ANY_MEMBER(concept_const_indirect, operator*, true);

}  // namespace smo_any

#endif // FILE_083B78A4_0E33_4FE4_84A3_15B8E4B42D94_H
