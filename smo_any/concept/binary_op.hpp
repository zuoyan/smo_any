#ifndef FILE_07878E04_C090_4281_8646_74C4D0515790_H
#define FILE_07878E04_C090_4281_8646_74C4D0515790_H
#include "smo_any/types.hpp"

namespace smo_any {

#define SMO_ANY_BINARY_OP(NAME, OP, CONST)                                    \
  template <class R>                                                          \
  struct NAME : smo_any::concepts<> {                                         \
    template <class T>                                                        \
    static inline R type_func(CONST T &a, CONST T &b) {                       \
      return a OP b;                                                          \
    }                                                                         \
                                                                              \
    struct Table {                                                            \
      R (*func)(CONST void *, CONST void *);                                  \
                                                                              \
      template <class T>                                                      \
      void reify() {                                                          \
        func = reinterpret_cast<decltype(func)>(type_func<T>);                \
      }                                                                       \
    };                                                                        \
                                                                              \
    template <class Derived>                                                  \
    struct Model {                                                            \
      template <class A>                                                      \
      inline friend typename std::enable_if<!std::is_same<A, Derived>::value, \
                                            R>::type                          \
      operator OP(CONST A &a, CONST Derived &b) {                             \
        assert(type_info(a) == b.table()->type_info);                         \
        const Table *t = b.table();                                           \
        return t->func(a.data(), b.data());                                   \
      }                                                                       \
                                                                              \
      template <class B>                                                      \
      inline friend R operator OP(CONST Derived &a, CONST B &b) {             \
        assert(a.table()->type_info == type_info(b));                         \
        const Table *t = a.table();                                           \
        return t->func(a.data(), b.data());                                   \
      }                                                                       \
    };                                                                        \
  };

}  // namespace smo_any
#endif // FILE_07878E04_C090_4281_8646_74C4D0515790_H
