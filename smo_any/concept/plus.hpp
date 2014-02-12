#ifndef FILE_5F4DD956_535D_4629_8CAB_CD2B383EFFFD_H
#define FILE_5F4DD956_535D_4629_8CAB_CD2B383EFFFD_H
#include "smo_any/concept/member.hpp"

namespace smo_any {

template <class B=self>
struct concept_plus_assign : concepts<> {
  template <class A>
  static void type_func(A &a, const B &b) {
    a += b;
  }

  struct Table {
    void (*func)(void*, const B&);
    template <class T>
    void reify() {
      func = reinterpret_cast<decltype(func)>(type_func<T>);
    }
  };

  template <class Derived>
  struct Model {
    Derived &operator+=(const B &b) {
      Derived &a = static_cast<Derived &>(*this);
      assert(!a.empty());
      const Table *table = a.table();
      table->func(a.data(), b);
      return *this;
    }
  };
};

template <>
struct concept_plus_assign<self> : concepts<> {
  template <class T>
  static void type_func(T &a, const T &b) {
    a += b;
  }

  struct Table {
    void (*func)(void *, const void *);
    template <class T>
    void reify() {
      func = reinterpret_cast<decltype(func)>(type_func<T>);
    }
  };

  template <class Derived>
  struct Model {
    template <class Other>
    Derived &operator+=(const Other &b) {
      Derived &a = static_cast<Derived &>(*this);
      assert(!a.empty());
      assert(a.table()->type_info == type_info(b));
      const Table *table = a.table();
      table->func(a.data(), data(b));
      return *this;
    }
  };
};

template <class B=self, class R=self>
struct concept_plus : concepts<> {
  template <class T>
  static R type_func(const T &a, const B &b) {
    return a + b;
  }

  struct Table {
    R (*func)(const void *, const void *);
    template <class T>
    void reify() {
      func = reinterpret_cast<decltype(func)>(type_func<T>);
    }
  };

  template <class Derived>
  struct Model {
    R operator+(const B &b) const {
      const Derived &a = static_cast<const Derived &>(*this);
      assert(!a.empty());
      const Table *table = a.table();
      return table->func(a.data(), b);
    }
  };
};

template <class B>
struct concept_plus<B, self> : concepts<> {
  template <class T>
  static void type_func(const T &a, const B &b, T *c) {
    new (c) T(a + b);
  }

  struct Table {
    void (*func)(const void*, const B&, void *);
    template <class T>
    void reify() {
      func = reinterpret_cast<decltype(func)>(type_func<T>);
    }
  };

  template <class Derived>
  struct Model {
    Derived operator+(const B &b) const {
      const Derived &a = static_cast<const Derived &>(*this);
      assert(!a.empty());
      auto r = a.raw_like();
      const Table *table = a.table();
      table->func(a.data(), b, r.data());
      return r;
    }
  };
};

template <class R>
struct concept_plus<self, R> : concepts<> {
  template <class T>
  static R type_func(const T &a, const T &b) {
    return a + b;
  }

  struct Table {
    R (*func)(const void *, const void *);
    template <class T>
    void reify() {
      func = reinterpret_cast<decltype(func)>(type_func<T>);
    }
  };

  template <class Derived>
  struct Model {
    template <class Other>
    Derived operator+(const Other &b) const {
      const Derived &a = static_cast<const Derived &>(*this);
      assert(!a.empty());
      assert(a.table()->type_info == type_info(b));
      auto r = a.raw_like();
      const Table *table = a.table();
      table->func(a.data(), data(b), r.data());
      return r;
    }
  };
};

template <>
struct concept_plus<self, self> : concepts<> {
  template <class T>
  static void type_func(const T &a, const T &b, T *c) {
    new (c) T(a + b);
  }

  struct Table {
    void (*func)(const void *, const void *, void *);
    template <class T>
    void reify() {
      func = reinterpret_cast<decltype(func)>(type_func<T>);
    }
  };

  template <class Derived>
  struct Model {
    template <class Other>
    Derived operator+(const Other &b) const {
      const Derived &a = static_cast<const Derived &>(*this);
      assert(!a.empty());
      assert(a.table()->type_info == type_info(b));
      auto r = a.raw_like();
      const Table *table = a.table();
      table->func(a.data(), data(b), r.data());
      return r;
    }
  };
};

}  // namespace smo_any
#endif // FILE_5F4DD956_535D_4629_8CAB_CD2B383EFFFD_H
