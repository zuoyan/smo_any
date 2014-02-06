#ifndef FILE_F07A45A1_FF3E_4DC1_BD97_9DB6EE78C856_H
#define FILE_F07A45A1_FF3E_4DC1_BD97_9DB6EE78C856_H

#include "smo_any/concept/typebase.hpp"

namespace smo_any {

struct concept_typeinfo : concepts<concept_typebase> {
  template <class Derived>
  struct Model {
    const std::type_info &typeinfo() const {
      auto derived = static_cast<const Derived *>(this);
      return *derived->table()->type_info;
    }
  };
};

}  // namespace smo_any
#endif // FILE_F07A45A1_FF3E_4DC1_BD97_9DB6EE78C856_H
