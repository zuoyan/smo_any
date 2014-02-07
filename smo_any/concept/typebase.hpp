#ifndef FILE_EDF2DEC2_0068_4ECB_8DF7_270A625C4374_H
#define FILE_EDF2DEC2_0068_4ECB_8DF7_270A625C4374_H
#include <typeinfo>

#include "smo_any/types.hpp"

namespace smo_any {

struct concept_typebase : concepts<> {
  struct Table {
    template <class T>
    void reify() {
      size = sizeof(T);
      align = sizeof(T);
      type_info = &typeid(T);
    }

    size_t size;
    size_t align;
    const std::type_info *type_info;
  };
};

}  // namespace smo_any
#endif // FILE_EDF2DEC2_0068_4ECB_8DF7_270A625C4374_H
