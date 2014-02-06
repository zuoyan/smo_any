#ifndef FILE_7CC6A802_9E1E_450F_974C_67C95F149651_H
#define FILE_7CC6A802_9E1E_450F_974C_67C95F149651_H
#include "smo_any/concept/member.hpp"

#define SMO_ANY_METHOD_ACCESS(NAME, METHOD)                       \
  struct NAME {                                                     \
    template <class T, class... U>                                  \
    auto operator()(T *ptr, U &&... args)                           \
        const -> decltype(METHOD(*ptr, std::forward<U>(args)...)) { \
      return METHOD(*ptr, std::forward<U>(args)...);                \
    }                                                               \
  };

#define SMO_ANY_METHOD_TABLE(SIG, METHOD, ISCONST) \
  SMO_ANY_METHOD_ACCESS(access_func, METHOD);      \
  using Table =                                      \
      typename smo_any::access_table<SIG, access_func, ISCONST>::Table;

#define SMO_ANY_METHOD_MODEL_GEN(NAME, METHOD, CONST)                     \
  template <class Derived>                                                  \
  struct NAME {                                                             \
    template <class... U>                                                   \
    inline friend auto METHOD(CONST Derived &self, U &&... args)            \
        -> decltype(std::declval<Table>().func((void *) nullptr,            \
                                               std::forward<U>(args)...)) { \
      const Table *table = self.table();                                    \
      return table->func((void *)self.data(), args...);                     \
    }                                                                       \
  };

#define SMO_ANY_METHOD_MODEL(METHOD, ISCONST)                      \
  SMO_ANY_METHOD_MODEL_GEN(Model_C, METHOD, const);                \
  SMO_ANY_METHOD_MODEL_GEN(Model_N, METHOD, );                     \
  template <class Derived>                                           \
  using Model = typename std::conditional<ISCONST, Model_C<Derived>, \
                                          Model_N<Derived>>::type;
#define SMO_ANY_METHOD(CONCEPT, METHOD, ISCONST) \
  template <class Sig>                             \
  struct CONCEPT {                                 \
    SMO_ANY_METHOD_TABLE(Sig, METHOD, ISCONST);  \
    SMO_ANY_METHOD_MODEL(METHOD, ISCONST);       \
  };

#endif // FILE_7CC6A802_9E1E_450F_974C_67C95F149651_H
