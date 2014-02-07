#ifndef FILE_36808483_F41B_4D4E_BED0_A18C164A29AB_H
#define FILE_36808483_F41B_4D4E_BED0_A18C164A29AB_H
#include "smo_any/types.hpp"

namespace smo_any {

struct concept_destruct : concepts<> {
  template <class T>
  static void type_destruct(T *d) {
    d->~T();
  }

  struct Table {
    void (*destruct)(void *d);

    template <class T>
    void reify() {
      destruct = reinterpret_cast<void (*)(void *)>(type_destruct<T>);
    }
  };
};

}  // namespace smo_any
#endif // FILE_36808483_F41B_4D4E_BED0_A18C164A29AB_H
