#ifndef FILE_7189E144_26DC_4574_A419_4641C5514C7C_H
#define FILE_7189E144_26DC_4574_A419_4641C5514C7C_H
#include "smo_any/basic_any.hpp"
#include "smo_any/concept/copy_construct.hpp"

#include <iostream>
#include <cstdlib>

namespace smo_any {

template <class D, class S, class Enable = typename std::enable_if<
                                std::is_constructible<D, S>::value>::type>
inline D any_cast_any_static(const S &o) {
  return D(o);
}

template <class D, class S, class Enable = void>
struct any_cast_any_help;

template <class D, class S>
struct any_cast_any_help<
    D, S, typename std::enable_if<table_is_sub<
              typename D::Table, typename S::Table>::value>::type> {
  template <class U>
  static inline D cast(U &&o) {
    return D(std::forward<U>(o));
  }
};

template <class D, class S>
struct any_cast_any_help<
    D, S,
    typename std::enable_if<
        !table_is_sub<typename D::Table, typename S::Table>::value>::type> {

  template <class Table>
  static const Table *get_table(const std::type_info *ti) {
    auto dict = singleton_get<type_dict<Table>>();
    return dict->find(ti);
  }

  template <class Table, class H, class... U>
  static typename std::enable_if<
      (std::is_base_of<Table, H>::value || std::is_same<Table, H>::value),
      const Table *>::type
  get_table(const std::type_info *ti, const H *h, U... u) {
    return h;
  }

  template <class Table, class H, class... U>
  typename std::enable_if<
      !(std::is_base_of<Table, H>::value || std::is_same<Table, H>::value),
      const Table *>::type static get_table(const std::type_info *ti,
                                            const H *h, U... u) {
    return get_table<Table>(ti, u...);
  }

  struct copy_table_func {
    typename D::Table *table;
    const S *src;

    template <class Concept>
    int operator()(Concept) {
      auto t = get_table<typename Concept::Table>(src->table()->type_info, src);
      if (!t) {
        std::cerr << "get concept " << typeid(Concept).name()
                  << " table for type " << src->table()->type_info->name()
                  << " failed when castting from " << typeid(S).name() << " to "
                  << typeid(D).name() << std::endl;
        abort();
      }
      *static_cast<typename Concept::Table *>(table) = *t;
      return 0;
    }
  };

  static const typename D::Table *get_dtable(const S &o) {
    auto ti = o.table()->type_info;
    auto dict = singleton_get<type_dict<typename D::Table>>();
    auto ret = dict->find(ti);
    if (ret) return ret;
    std::unique_ptr<typename D::Table> ptr(new typename D::Table);
    copy_table_func f{ptr.get(), &o};
    run_concept_vector<typename D::Concepts>::template run(f);
    ret = dict->add(ti, ptr).first;
    return ret;
  }

  template <class U>
  static inline D cast(U &&o) {
    D d;
    if (!o) return d;
    auto t = get_dtable(o);
    d.table_ = t;
    if (std::is_reference<U>::value) {
      auto c = get_table<concept_copy_construct::Table>(
          t->type_info, t, o.table());
      if (d.stored_in_heap()) {
        d.heap_data() = ::operator new(o.table()->size);
      }
      c->copy_construct(d.data(), o.data());
    } else {
      if (d.stored_in_heap()) {
        if (o.stored_in_heap()) {
          d.heap_data() = o.heap_data();
          ((S *)&o)->table_ = nullptr;
        } else {
          d.heap_data() = ::operator new(o.table()->size);
        }
      }
      if (o) {
        t->move_construct(d.data(), (void *)o.data());
      }
    }
    return d;
  }
};

template <class D, class S>
inline D any_cast_any(const S &o) {
  any_cast_any_help<D, S> f;
  return f.cast(o);
}

}  // namespace smo_any
#endif // FILE_7189E144_26DC_4574_A419_4641C5514C7C_H
