#ifndef FILE_A083A81A_2671_40FF_A383_3EF5100A19DE_H
#define FILE_A083A81A_2671_40FF_A383_3EF5100A19DE_H
#include "smo_any/any.hpp"
#include "smo_any/concept/member.hpp"

namespace smo_any {

template <class Sig, bool is_const, class Store = any_store>
struct concept_any_function;

template <class R, class... Args, bool is_const, class Store>
struct concept_any_function<R(Args...), is_const,
                            Store> : concepts<concept_any<Store>> {
  typedef R(Sig)(Args...);
  SMO_ANY_MEMBER_TABLE(Sig, operator(), is_const);

  template <class Derived>
  struct Model_N {
    R operator()(Args... args) {
      auto derived = static_cast<Derived *>(this);
      const Table *t = derived->table();
      return t->func(&derived->store_, args...);
    }
  };

  template <class Derived>
  struct Model_C {
    R operator()(Args... args) const {
      auto derived = static_cast<Derived *>(const_cast<Model_C *>(this));
      const Table *t = derived->table();
      return t->func(&derived->store_, args...);
    }
  };

  template <class Derived>
  using Model = typename std::conditional<is_const, Model_C<Derived>,
                                          Model_N<Derived>>::type;
};

template <class Sig, class Store = any_store, class... Concept>
using basic_any_function_c =
    basic_any<Store, concept_any_function<Sig, true, Store>, Concept...>;

template <class Sig, class Store = any_store, class... Concept>
using basic_any_function_n =
    basic_any<Store, concept_any_function<Sig, false, Store>, Concept...>;

template <class Sig, class... Concept>
using any_function_c = basic_any<
    any_store, concept_any_function<Sig, true, any_store>, Concept...>;

template <class Sig, class... Concept>
using any_function_n = basic_any<
    any_store, concept_any_function<Sig, false, any_store>, Concept...>;
}  // namespace smo_any
#endif // FILE_A083A81A_2671_40FF_A383_3EF5100A19DE_H
