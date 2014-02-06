#ifndef FILE_76EAA636_6326_4054_B166_9C36FCBFEA18_H
#define FILE_76EAA636_6326_4054_B166_9C36FCBFEA18_H
#include "smo_any/types.hpp"

namespace smo_any {

struct concept_move_construct : concepts<> {
  template <class T>
  static void type_move_construct(T *d, T *s) {
    new (d) T(std::move(*s));
  }

  struct Table {
    void (*move_construct)(void *d, void *s);

    template <class T>
    void reification() {
      move_construct =
          reinterpret_cast<void (*)(void *, void *)>(type_move_construct<T>);
    }
  };
};

}  // namespace smo_any
#endif // FILE_76EAA636_6326_4054_B166_9C36FCBFEA18_H
