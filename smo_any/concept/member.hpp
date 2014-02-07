#ifndef FILE_45D14DC3_3341_4488_B50E_00F6F9DEEA37_H
#define FILE_45D14DC3_3341_4488_B50E_00F6F9DEEA37_H
#include "smo_any/types.hpp"

namespace smo_any {

template <class Sig, class AccessFunc, bool is_const>
struct access_table;

template <class R, class... Args, class AccessFunc, bool is_const>
struct access_table<R(Args...), AccessFunc, is_const> {
  template <class RR, class T>
  static inline typename std::enable_if<std::is_void<RR>::value, void>::type
  type_func_h(T *self, Args... args) {
    AccessFunc func;
    func(self, args...);
  }

  template <class RR, class T>
  static inline typename std::enable_if<!std::is_void<RR>::value, RR>::type
  type_func_h(T *self, Args... args) {
    AccessFunc func;
    return func(self, args...);
  }

  template <class T>
  static inline R type_func(T *self, Args... args) {
    return type_func_h<R>(self, args...);
  }

  struct Table {
    R (*func)(void *self, Args...);

    template <class T>
    void reify() {
      func = reinterpret_cast<R (*)(void *, Args...)>(
          type_func<typename std::conditional<is_const, const T, T>::type>);
    };
  };
};

}  // namespace smo_any

#define SMO_ANY_MEMBER_ACCESS(NAME, MEMBER)                        \
  struct NAME {                                                      \
    template <class T, class... U>                                   \
    auto operator()(T *ptr, U &&... args)                            \
        const -> decltype(ptr -> MEMBER(std::forward<U>(args)...)) { \
      return ptr->MEMBER(std::forward<U>(args)...);                  \
    }                                                                \
  };

#define SMO_ANY_MEMBER_TABLE(SIG, MEMBER, ISCONST) \
  SMO_ANY_MEMBER_ACCESS(access_func, MEMBER);      \
  using Table =                                      \
      typename smo_any::access_table<SIG, access_func, ISCONST>::Table;

#define SMO_ANY_MEMBER_MODEL_GEN(NAME, MEMBER, CONST)                     \
  template <class Derived>                                                  \
  struct NAME {                                                             \
    template <class... U>                                                   \
    inline auto MEMBER(U &&... args) CONST                                  \
        -> decltype(std::declval<Table>().func((void *) nullptr,            \
                                               std::forward<U>(args)...)) { \
      CONST Derived *derived = (Derived *)(this);                           \
      const Table *table = derived->table();                                \
      return table->func((void *)derived->data(), args...);                 \
    }                                                                       \
  };

#define SMO_ANY_MEMBER_MODEL(MEMBER, ISCONST)                      \
  SMO_ANY_MEMBER_MODEL_GEN(Model_C, MEMBER, const);                \
  SMO_ANY_MEMBER_MODEL_GEN(Model_N, MEMBER, );                     \
  template <class Derived>                                           \
  using Model = typename std::conditional<ISCONST, Model_C<Derived>, \
                                          Model_N<Derived>>::type;

#define SMO_ANY_MEMBER(CONCEPT, MEMBER, ISCONST) \
  template <class Sig>                             \
  struct CONCEPT {                                 \
    SMO_ANY_MEMBER_TABLE(Sig, MEMBER, ISCONST);  \
    SMO_ANY_MEMBER_MODEL(MEMBER, ISCONST);       \
  };

#endif // FILE_45D14DC3_3341_4488_B50E_00F6F9DEEA37_H
