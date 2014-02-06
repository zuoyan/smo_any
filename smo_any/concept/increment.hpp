#ifndef FILE_125BFFD6_0B86_407B_94E1_2799EF514C92_H
#define FILE_125BFFD6_0B86_407B_94E1_2799EF514C92_H
#include <cassert>

#include "smo_any/concept/member.hpp"

namespace smo_any {

struct concept_increment : smo_any::concepts<> {
  SMO_ANY_MEMBER_TABLE(void(void), operator++, false);

  template <class Derived>
  struct Model {
    inline Derived &operator++() {
      Derived &a = static_cast<Derived &>(*this);
      assert(!a.empty());
      const Table *table = a.table();
      table->func(a.data());
      return a;
    }
  };
};

}  // namespace smo_any
#endif // FILE_125BFFD6_0B86_407B_94E1_2799EF514C92_H
