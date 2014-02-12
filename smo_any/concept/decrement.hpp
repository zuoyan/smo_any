#ifndef FILE_51CB5D17_4C95_48D9_A500_681DE328D4DD_H
#define FILE_51CB5D17_4C95_48D9_A500_681DE328D4DD_H
namespace smo_any {

struct concept_decrement : concepts<> {
  SMO_ANY_MEMBER_TABLE(void(void), operator--, false);

  template <class Derived>
  struct Model {
    inline Derived &operator--() {
      Derived &a = static_cast<Derived &>(*this);
      assert(!a.empty());
      const Table *table = a.table();
      table->func(a.data());
      return a;
    }

    inline Derived operator--(int) {
      Derived a = static_cast<Derived>(*this);
      --*this;
      return a;
    }
  };
};

}  // namespace smo_any
#endif // FILE_51CB5D17_4C95_48D9_A500_681DE328D4DD_H
