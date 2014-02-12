#ifndef FILE_C1E783D6_C57F_49A4_AB2F_87BE5DD498C2_H
#define FILE_C1E783D6_C57F_49A4_AB2F_87BE5DD498C2_H
#include <type_traits>

#include "smo_any/concept/typebase.hpp"

namespace smo_any {

struct concept_equal : concepts<concept_typebase> {
  template <class T>
  static inline bool type_equal(const T *a, const T *b) {
    return *a == *b;
  }

  struct Table {
    bool (*func)(const void *a, const void *b);

    template <class T>
    void reify() {
      func =
          reinterpret_cast<bool (*)(const void *, const void *)>(type_equal<T>);
    }
  };

  template <class Derived>
  struct Model {
    template <class Other>
    inline friend bool operator==(const Derived &a, const Other &b) {
      const Table *at = a.table();
      auto bti = type_info(b);
      if (!at) return bti == &typeid(void);
      auto ati = a.table()->type_info;
      if (ati == bti) {
        return at->func(a.data(), data(b));
      }
      return false;
    }

    template <class Other>
    inline friend typename std::enable_if<!std::is_same<Other, Derived>::value,
                                          bool>::type
    operator==(const Other &b, const Derived &a) {
      return a == b;
    }

    template <class Other>
    inline friend bool operator!=(const Derived &a, const Other &b) {
      return !(a == b);
    }

    template <class Other>
    inline friend typename std::enable_if<!std::is_same<Other, Derived>::value,
                                          bool>::type
    operator!=(const Other &b, const Derived &a) {
      return !(a == b);
    }
  };
};

}  // namespace smo_any
#endif // FILE_C1E783D6_C57F_49A4_AB2F_87BE5DD498C2_H
