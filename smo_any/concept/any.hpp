#ifndef FILE_7C86B3CC_1A3C_4466_988B_E908C7B1BD31_H
#define FILE_7C86B3CC_1A3C_4466_988B_E908C7B1BD31_H
#include "smo_any/concept/move_construct.hpp"
#include "smo_any/concept/destruct.hpp"
#include "smo_any/concept/typebase.hpp"

namespace smo_any {

template <class Store = any_store>
struct concept_any
    : concepts<concept_typebase, concept_destruct, concept_move_construct> {
  template <class T>
  static void *type_any_data(Store *store) {
    if (sizeof(Store) >= sizeof(T) && alignof(Store) >= alignof(T)) {
      return static_cast<void *>(store);
    }
    return reinterpret_cast<void *&>(*store);
  }

  struct Table {
    bool any_in_local;
    void *(*any_data)(Store *store);

    template <class T>
    void reification() {
      any_in_local = sizeof(Store) >= sizeof(T) && alignof(Store) >= alignof(T);
      any_data = reinterpret_cast<void *(*)(Store *)>(type_any_data<T>);
    }
  };
};

}  // namespace smo_any
#endif // FILE_7C86B3CC_1A3C_4466_988B_E908C7B1BD31_H
