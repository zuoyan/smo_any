#ifndef FILE_0B404C23_FD6C_4120_B8CA_9AAC2A15DBF8_H
#define FILE_0B404C23_FD6C_4120_B8CA_9AAC2A15DBF8_H
#include "smo_any/types.hpp"
#include "smo_any/concept/equal.hpp"

namespace smo_any {

struct concept_compare : smo_any::concepts<> {
  template <class T>
  static inline bool type_lt(const T &a, const T &b) {
    return a < b;
  }

  template <class T>
  static inline bool type_le(const T &a, const T &b) {
    return a <= b;
  }

  template <class T>
  static inline bool type_gt(const T &a, const T &b) {
    return a > b;
  }

  template <class T>
  static inline bool type_ge(const T &a, const T &b) {
    return a >= b;
  }

  struct Table {
    bool (*lt)(const void *a, const void *b);
    bool (*le)(const void *a, const void *b);
    bool (*gt)(const void *a, const void *b);
    bool (*ge)(const void *a, const void *b);

    template <class T>
    void reify() {
      lt = reinterpret_cast<bool (*)(const void *, const void *)>(type_lt<T>);
      le = reinterpret_cast<bool (*)(const void *, const void *)>(type_le<T>);
      gt = reinterpret_cast<bool (*)(const void *, const void *)>(type_gt<T>);
      ge = reinterpret_cast<bool (*)(const void *, const void *)>(type_ge<T>);
    }
  };

  template <class Derived>
  struct Model {
    template <class A>
    inline friend typename std::enable_if<!std::is_same<A, Derived>::value,
                                          bool>::type
    operator<(const A &a, const Derived &b) {
      assert(a.table()->type_info == b.table()->type_info);
      const Table *at = a.table();
      return at->lt(a.data(), b.data());
    }

    template <class B>
    inline friend bool operator<(const Derived &a, const B &b) {
      assert(a.table()->type_info == b.table()->type_info);
      const Table *at = a.table();
      return at->lt(a.data(), b.data());
    }

    template <class A>
    inline friend typename std::enable_if<!std::is_same<A, Derived>::value,
                                          bool>::type
    operator<=(const A &a, const Derived &b) {
      assert(a.table()->type_info == b.table()->type_info);
      const Table *at = a.table();
      return at->le(a.data(), b.data());
    }

    template <class B>
    inline friend bool operator<=(const Derived &a, const B &b) {
      assert(a.table()->type_info == b.table()->type_info);
      const Table *at = a.table();
      return at->le(a.data(), b.data());
    }

    template <class A>
    inline friend typename std::enable_if<!std::is_same<A, Derived>::value,
                                          bool>::type
    operator>(const A &a, const Derived &b) {
      assert(a.table()->type_info == b.table()->type_info);
      const Table *at = a.table();
      return at->gt(a.data(), b.data());
    }

    template <class B>
    inline friend bool operator>(const Derived &a, const B &b) {
      assert(a.table()->type_info == b.table()->type_info);
      const Table *at = a.table();
      return at->gt(a.data(), b.data());
    }

    template <class A>
    inline friend typename std::enable_if<!std::is_same<A, Derived>::value,
                                          bool>::type
    operator>=(const A &a, const Derived &b) {
      assert(a.table()->type_info == b.table()->type_info);
      const Table *at = a.table();
      return at->ge(a.data(), b.data());
    }

    template <class B>
    inline friend bool operator>=(const Derived &a, const B &b) {
      assert(a.table()->type_info == b.table()->type_info);
      const Table *at = a.table();
      return at->ge(a.data(), b.data());
    }
  };
};

}  // namespace smo_any
#endif // FILE_0B404C23_FD6C_4120_B8CA_9AAC2A15DBF8_H
