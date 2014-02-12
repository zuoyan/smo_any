#ifndef FILE_083D38D6_B243_42CA_B364_8607317DA3C4_H
#define FILE_083D38D6_B243_42CA_B364_8607317DA3C4_H
#include "smo_any/concept/member.hpp"

namespace smo_any {

template <class B=self>
struct concept_minus_assign : concepts<> {
  template <class T>
  static void type_func(T &a, const B &b) {
    a -= b;
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
    Derived &operator-=(const B &b) {
      Derived &a = static_cast<Derived &>(*this);
      assert(!a.empty());
      const Table *table = a.table();
      table->func(a.data(), b);
      return *this;
    }
  };
};

template <>
struct concept_minus_assign<self> : concepts<> {
  template <class T>
  static void type_func(T &a, const T &b) {
    a -= b;
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
    Derived &operator-=(const Other &b) {
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
struct concept_minus : concepts<> {
  template <class T>
  static R type_func(const T &a, const B &b) {
    return a - b;
  }

  struct Table {
    R (*func)(void *, const B &);
    template <class T>
    void reify() {
      func = reinterpret_cast<decltype(func)>(type_func<T>);
    }
  };

  template <class Derived>
  struct Model {
    R operator-(const B &b) const {
      const Derived &a = static_cast<const Derived &>(*this);
      assert(!a.empty());
      const Table *table = a.table();
      return table->func(a.data(), b);
    }
  };
};

template <class B>
struct concept_minus<B, self> : concepts<> {
  template <class T>
  static void type_func(const T &a, const B &b, T *c) {
    new (c) T(a - b);
  }

  struct Table {
    void (*func)(void*, const B&, void*);
    template <class T>
    void reify() {
      func = reinterpret_cast<decltype(func)>(type_func<T>);
    }
  };

  template <class Derived>
  struct Model {
    Derived operator-(const B &b) const {
      const Derived &a = static_cast<const Derived &>(*this);
      assert(!a.empty());
      auto r = a.raw_like();
      const Table *table = a.table();
      table->func(a.data(), b, r.data());
      return a;
    }
  };
};

template <class R>
struct concept_minus<self, R> : concepts<> {
  template <class T>
  static R type_func(const T &a, const T &b) {
    return a - b;
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
    R operator-(const Other &b) const {
      const Derived &a = static_cast<const Derived &>(*this);
      assert(!a.empty());
      assert(a.table()->type_info == type_info(b));
      const Table *table = a.table();
      return table->func(a.data(), data(b));
    }
  };
};

template <>
struct concept_minus<self, self> : concepts<> {
  template <class T>
  static void type_func(const T &a, const T &b, T *c) {
    new (c) T(a - b);
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
    Derived operator-(const Other &b) const {
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
#endif // FILE_083D38D6_B243_42CA_B364_8607317DA3C4_H
