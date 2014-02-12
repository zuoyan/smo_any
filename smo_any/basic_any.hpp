#ifndef FILE_5AC56FE0_6593_4543_A585_8E581AF78516_H
#define FILE_5AC56FE0_6593_4543_A585_8E581AF78516_H

#include <cassert>
#include <utility>
#include <new>
#include <string>
#include <memory>
#include <memory>

#include "smo_any/concept/any.hpp"
#include "smo_any/dynamic.hpp"
#include "smo_any/type_dict.hpp"
#include "smo_any/concept/copy_construct.hpp"

namespace smo_any {

template <class Store, class... Concept>
struct basic_any
    : bases<typename model_extends<basic_any<Store, Concept...>, vector<empty>,
                                   concept_any<Store>, Concept...>::type> {

  typedef vector<concept_any<Store>, Concept...> Concepts;
  typedef table_bases<typename table_extends<vector<>, concept_any<Store>,
                                             Concept...>::type> Table;
  const Table *table_;
  Store store_;

  static bool stored_in_heap(size_t size, size_t align) {
    return !(size <= sizeof(Store) && align <= alignof(Store));
  }

  inline bool stored_in_heap() const {
    // return stored_in_heap(table_->size, table_->align);
    return !table_->any_in_local;
  }

  inline bool stored_in_local() const { return table_->any_in_local; }

  void *&heap_data() const {
    return reinterpret_cast<void *&>(const_cast<Store &>(store_));
  }

  void *local_data() const {
    return reinterpret_cast<void *>(const_cast<Store *>(&store_));
  }

  inline explicit operator bool() const { return table_; }

  inline bool empty() const { return !table_; }

  inline const Table *table() const { return table_; }

  inline void *data() {
    assert(!empty());
    return stored_in_local() ? local_data() : heap_data();
  }

  inline const void *data() const {
    return const_cast<basic_any *>(this)->data();
  }

  template <class T>
  T *get() {
    assert(!empty());
    if (*table_->type_info == typeid(T)) {
      return static_cast<T *>(data());
    }
    return nullptr;
  }

  template <class T>
  const T *get() const {
    return const_cast<basic_any *>(this)->get<T>();
  }

  void clear() {
    if (table_) {
      if (stored_in_heap()) {
        table_->destruct(heap_data());
        ::operator delete(heap_data());
      } else {
        table_->destruct(local_data());
      }
    }
    table_ = nullptr;
  }

  ~basic_any() { clear(); }
  basic_any() { table_ = nullptr; }

  basic_any raw_like() const {
    basic_any t;
    t.table_ = table_;
    assert(table_);
    if (stored_in_heap()) {
      t.heap_data() = ::operator new(table_->size);
    }
    return std::move(t);
  }

  template <class U>
  basic_any(U &&value) {
    typedef typename std::remove_reference<
        typename std::remove_cv<U>::type>::type T;
    static static_reify_and_register<Table, T, concept_any<Store>, Concept...>
        s;
    table_ = s.table;
    if (sizeof(Store) >= sizeof(T) && alignof(Store) >= alignof(T)) {
      new (local_data()) T(std::forward<U>(value));
    } else {
      heap_data() = ::operator new(sizeof(T));
      new (heap_data()) T(std::forward<U>(value));
    }
  }

  basic_any(const basic_any &o) {
    table_ = o.table_;
    if (table_) {
      if (stored_in_heap()) {
        heap_data() = ::operator new(table_->size);
        table_->copy_construct(heap_data(), o.heap_data());
      } else {
        table_->copy_construct(local_data(), o.local_data());
      }
    }
  }

  basic_any(basic_any &o) {
    new (this) basic_any(const_cast<const basic_any &>(o));
  }

  basic_any(basic_any &&o) {
    table_ = o.table_;
    if (table_) {
      if (stored_in_heap()) {
        heap_data() = o.heap_data();
        o.table_ = nullptr;
      } else {
        table_->move_construct(local_data(), o.local_data());
      }
    }
  }

  template <class OS, class... OC,
            class Enable = typename std::enable_if<table_is_sub<
                Table, typename basic_any<OS, OC...>::Table>::value>::type>
  basic_any(const basic_any<OS, OC...> &o) {
    if (o.table_) {
      auto dict = singleton_get<type_dict<Table>>();
      auto p = dict->find(o.table_->type_info);
      if (!p) {
        std::unique_ptr<Table> ptr(new Table);
        *ptr = *o.table_;
        p = dict->add(o.table_->type_info, ptr).first;
      }
      table_ = p;
      if (stored_in_heap()) {
        heap_data() = ::operator new(table_->size);
        o.table_->copy_construct(heap_data(), o.data());
      } else {
        o.table_->copy_construct(local_data(), o.data());
      }
    }
  }

  template <class OS, class... OC,
            class Enable = typename std::enable_if<table_is_sub<
                Table, typename basic_any<OS, OC...>::Table>::value>::type>
  basic_any(basic_any<OS, OC...> &o) {
    new (this) basic_any(const_cast<const basic_any<OS, OC...> &>(o));
  }

  template <class OS, class... OC,
            class Enable = typename std::enable_if<table_is_sub<
                Table, typename basic_any<OS, OC...>::Table>::value>::type>
  basic_any(basic_any<OS, OC...> &&o) {
    if (o.table_) {
      auto dict = singleton_get<type_dict<Table>>();
      auto p = dict->find(o.table_->type_info);
      if (!p) {
        std::unique_ptr<Table> ptr(new Table);
        *ptr = *o.table_;
        p = dict->add(o.table_->type_info, ptr).first;
      }
      table_ = p;
      if (stored_in_heap()) {
        if (o.stored_in_heap()) {
          heap_data() = o.heap_data();
          o.table_ = nullptr;
        } else {
          heap_data() = ::operator new(table_->size);
          o.table_->move_construct(heap_data(), o.local_data());
        }
      } else {
        o.table_->move_construct(local_data(), o.data());
      }
    }
  }

  template <class U>
  basic_any &operator=(U &&value) {
    if (this != (basic_any *)&value) {
      this->~basic_any();
      new (this) basic_any(std::forward<U>(value));
    }
    return *this;
  }
};

template <class S, class ...C>
const void* data(const basic_any<S, C...> &d) {
  return d ? d.data() : nullptr;
}

template <class S, class ...C>
void* data(basic_any<S, C...> &d) {
  return d ? d.data() : nullptr;
}

template <class S, class ...C>
const std::type_info* type_info(const basic_any<S, C...> &d) {
  return d ? d.table()->type_info : &typeid(void);
}

}  // namespace smo_any

#endif  // FILE_5AC56FE0_6593_4543_A585_8E581AF78516_H
