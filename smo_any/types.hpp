#ifndef FILE_10DDD1E6_BB97_43BE_B1E4_A9DEFA060C2E_H
#define FILE_10DDD1E6_BB97_43BE_B1E4_A9DEFA060C2E_H
#include <type_traits>
#include <cstdint>
#include <typeinfo>

namespace smo_any {

struct empty {};

template <class... T>
struct vector {
  typedef vector type;
};

constexpr intmax_t amount() { return 0; }

template <class... T>
constexpr intmax_t amount(intmax_t h, T... v) {
  return h + amount(v...);
}

template <class V, class T>
struct unique_base_add;

template <class... Bases, class T>
struct unique_base_add<vector<Bases...>, T> {
  template <class V, class... BS>
  struct check;

  template <class V>
  struct check<V> {
    typedef V type;
  };

  template <class... V, class H, class... BS>
  struct check<vector<V...>, H, BS...> {
    typedef typename std::conditional<
        (std::is_base_of<H, T>::value || std::is_same<H, T>::value),
        check<vector<V...>, BS...>,
        check<vector<V..., H>, BS...>>::type::type type;
  };

  typedef typename std::conditional<
      (amount(std::is_base_of<T, Bases>::value...) ||
       amount(std::is_same<T, Bases>::value...)),
      vector<Bases...>, check<vector<T>, Bases...>>::type::type type;
};

template <class V, class... Ts>
struct unique_base_adds {
  typedef V type;
};

template <class V, class H, class... Ts>
struct unique_base_adds<V, H, Ts...> : unique_base_adds<
                                           typename unique_base_add<V, H>::type,
                                           Ts...> {};
template <class V, class Ts>
struct unique_base_add_vector;

template <class V, class... Ts>
struct unique_base_add_vector<V, vector<Ts...>> : unique_base_adds<V, Ts...> {};

template <class V>
struct bases;

template <class... T>
struct bases<vector<T...>> : T... {};

struct table_empty {
  template <class T>
  void reification() {}
};

template <class... T>
struct concepts {
  typedef vector<T...> concept_bases;

  using Table = table_empty;

  template <class Derived>
  using Model = empty;
};

template <class Concept>
struct get_concept_bases {
  template <class U>
  static auto deduce(U *) -> typename U::concept_bases;

  template <class U>
  static vector<> deduce(...);

  typedef decltype(deduce<Concept>(static_cast<Concept *>(nullptr))) type;
};

template <class V>
struct table_bases;

template <class... T>
struct table_bases<vector<T...>> : T... {
 private:
  template <class H, class U>
  int reification_help() {
    H::template reification<U>();
    return 1;
  }

  template <class TT, class U>
  int copy(const U &v) {
    static_cast<TT &>(*this) = v;
    return 1;
  }

 public:
  template <class U>
  void operator=(const U &v) {
    amount(copy<T>(v)...);
  }

  template <class U>
  void reification() {
    amount(reification_help<T, U>()...);
  }
};

template <class S, class E>
struct table_is_sub;

template <class... ST, class... ET>
struct table_is_sub<table_bases<vector<ST...>>, table_bases<vector<ET...>>> {
  template <class U>
  static constexpr bool check() {
    return amount(std::is_base_of<U, ET>::value...);
  }

  static constexpr bool value = amount(check<ST>()...) == sizeof...(ST);
  typedef std::integral_constant<bool, value> type;
};

template <class TableBases, class... Concept>
struct table_extends {
  typedef TableBases type;
};

template <class TableBases, class Concepts>
struct table_extend_vector;

template <class TableBases, class... Concept>
struct table_extend_vector<
    TableBases, vector<Concept...>> : table_extends<TableBases, Concept...> {};

template <class TableBases, class Concept>
struct table_extend {
  typedef typename unique_base_add<TableBases, typename Concept::Table>::type
      type1;
  typedef typename table_extend_vector<
      type1, typename get_concept_bases<Concept>::type>::type type;
};

template <class TableBase, class Concept, class... Rest>
struct table_extends<
    TableBase, Concept,
    Rest...> : table_extends<typename table_extend<TableBase, Concept>::type,
                             Rest...> {};

template <class Derived, class ModelBases, class... Concept>
struct model_extends {
  typedef ModelBases type;
};

template <class Derived, class ModelBases, class Concepts>
struct model_extend_vector;

template <class Derived, class ModelBases, class... Concept>
struct model_extend_vector<
    Derived, ModelBases,
    vector<Concept...>> : model_extends<Derived, ModelBases, Concept...> {};

template <class Derived, class ModelBases, class Concept>
struct model_extend {
  typedef typename unique_base_add<
      ModelBases, typename Concept::template Model<Derived>>::type type1;
  typedef typename model_extend_vector<
      Derived, type1, typename get_concept_bases<Concept>::type>::type type;
};

template <class Derived, class ModelBases, class Concept, class... Rest>
struct model_extends<
    Derived, ModelBases, Concept,
    Rest...> : model_extends<Derived, typename model_extend<Derived, ModelBases,
                                                            Concept>::type,
                             Rest...> {};

using any_store = std::aligned_storage<sizeof(void *) * 3>::type;

}  // namespace smo_any
#endif // FILE_10DDD1E6_BB97_43BE_B1E4_A9DEFA060C2E_H
