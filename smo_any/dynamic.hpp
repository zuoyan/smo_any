#ifndef FILE_23476497_86F0_4719_91FB_25433B26AE9F_H
#define FILE_23476497_86F0_4719_91FB_25433B26AE9F_H
#include "smo_any/types.hpp"
#include "smo_any/type_dict.hpp"

namespace smo_any {
template <class Table, class T>
struct static_reify {
  Table *table;

  static_reify() {
    table = singleton_get<Table, T>();
    table->template reify<T>();
  }
};

template <class ...Concept>
struct run_concepts {
  template <class Func>
  static int run(Func &&func) {
    return 0;  // ignored
  }
};

template <class Concepts>
struct run_concept_vector;

template <class... Concept>
struct run_concept_vector<vector<Concept...>> : run_concepts<Concept...> {};

template <class Concept>
struct run_concept {
  template <class Func>
  static int run(Func &&func) {
    func(Concept{});
    return run_concept_vector<typename get_concept_bases<Concept>::type>::run(
        func);
  }
};

template <class H, class ...Concept>
struct run_concepts<H, Concept...> {
  template <class Func>
  static int run(Func && func) {
    run_concept<H>::run(func);
    return run_concepts<Concept...>::run(func);
  }
};

template <class Table, class T, class... Concept>
struct static_reify_and_register {
  const Table *table;

  struct register_func {
    const Table *table;

    template <class UC>
    int operator()(UC) {
      const typename UC::Table *t = table;
      auto dict = singleton_get<type_dict<typename UC::Table>>();
      dict->add(&typeid(T), (Table *)t);
      return 0;
    }
  };

  static_reify_and_register() {
    table = singleton_get<static_reify<Table, T>>()->table;
    register_func f{table};
    run_concepts<Concept...>::run(f);
    auto dict = singleton_get<type_dict<Table>>();
    dict->add(&typeid(T), (Table*)table);
  }
};

}  // namespace smo_any
#endif // FILE_23476497_86F0_4719_91FB_25433B26AE9F_H
