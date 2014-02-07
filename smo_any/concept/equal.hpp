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
      auto at = a.table();
      const concept_typebase::Table *bt = b.table();
      if (!at && !bt) return true;
      if (!at || !bt) return false;
      if (at->type_info == bt->type_info) {
        return static_cast<const Table *>(at)
            ->func((const void *)a.data(), (const void *)b.data());
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
