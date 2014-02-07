#ifndef FILE_A6B6BF8F_A5E1_432A_B84E_16AEBBE29336_H
#define FILE_A6B6BF8F_A5E1_432A_B84E_16AEBBE29336_H
#include "smo_any/types.hpp"

namespace smo_any {

struct concept_copy_construct : concepts<concept_move_construct> {
  template <class T>
  static void type_copy_construct(T *d, const T *s) {
    new (d) T(*s);
  }

  struct Table {
    void (*copy_construct)(void *d, const void *s);

    template <class T>
    void reify() {
      copy_construct = reinterpret_cast<void (*)(void *, const void *)>(
          type_copy_construct<T>);
    }
  };
};

}  // namespace smo_any
#endif // FILE_A6B6BF8F_A5E1_432A_B84E_16AEBBE29336_H
