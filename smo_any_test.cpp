#include "smo_any/any.hpp"
#include "smo_any/concept/copy_construct.hpp"
#include "smo_any/concept/member.hpp"
#include "smo_any/concept/method.hpp"
#include "smo_any/concept/equal.hpp"
#include "smo_any/concept/typeinfo.hpp"
#include "smo_any/concept/any_function.hpp"
#include "smo_any/concept/container.hpp"
#include "smo_any/concept/call.hpp"
#include "smo_any/any_cast.hpp"

#include <cassert>
#include <utility>
#include <new>
#include <string>
#include <typeinfo>
#include <cstddef>

#include <iostream>

#include "clog/clog.hpp"

#include <unistd.h>

// test

struct show_type_tag;

template <class A, class B=show_type_tag>
void show_type() {
  static_assert(std::is_same<A, B>::value, "...");
}

template <class A, class B>
void check_type() {
  static_assert(std::is_same<A, B>::value, "...");
}

namespace smo_any {

template <class... Concept>
using concepts_table =
    table_bases<typename table_extends<vector<>, Concept...>::type>;

template <class... Concept>
using concepts_model =
    bases<typename model_extends<empty, vector<>, Concept...>::type>;

void static_check_func() {
  typedef int Store;
  static_assert(
      std::is_same<
          concepts_table<concept_any<Store>>,
          concepts_table<concept_any<Store>, concept_move_construct>>::value,
      "...");

  static_assert(table_is_sub<concepts_table<concept_move_construct>,
                             concepts_table<concept_any<>>>::value,
                "...");

  // show_type<concepts_table<concept_any_function<int(int)>>>();

  static_assert(
      table_is_sub<
          concepts_table<concept_any<int>>,
          concepts_table<concept_any<int>, concept_copy_construct>>::value,
      "...");
}

}  // namespace smo_any

#include <iostream>
#include <chrono>
#include <functional>
#include <vector>
#include <list>

struct More {
  char data[128];
  std::string mesg;
};

struct Call {
  int value_;

  Call(int v) : value_(v) {
    // std::cerr << "Call(" << v << ")\n";
  }

  Call(const Call &o) : value_(o.value_) {
    // std::cerr << "Call(Call(" << value_ << "))\n";
  }

  Call(Call &&o) : value_(o.value_) {
    // std::cerr << "Call(Call&&(" << value_ << "))\n";
  }

  int operator()(int a, int b) const { return a + b + value_; }
  double operator()(double a, double b) const { return a + b + value_; }
};

template <class Func>
void test_func(const std::string &name, Func &&func) {
#ifdef NDEBUG
  int n = 1.e8;
#else
  int n = 1.e5;
#endif
  int v = 0;
  auto start = std::chrono::steady_clock::now();
  for (int i = 0; i < n; ++i) {
    v = func(i, v);
  }
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double, std::nano> elapsed = end - start;
  CLOG(INFO, name, "result", v, "n=" << n);
  CLOG(INFO, name, elapsed.count() / n, "nano-seconds/call");
  CLOG(INFO, name, n * 1.e9 / elapsed.count(), "#calls/second");
}

using namespace smo_any;

int main(int argc, char *argv[]) {
  static_check_func();
  CLOG(INFO, "start ...");
  {
    any<> a;
    any<> b(10);
    CLOG_CHECK(!a);
    CLOG_CHECK(b);
    CLOG_CHECK_EQ(b.table()->type_info, &typeid(int));
    CLOG_CHECK(!b.get<double>());
    CLOG_CHECK(b.get<int>());
    CLOG_CHECK_EQ(*b.get<int>(), 10);
  }

  {
    std::vector<int> vi{{11,12,13}};
    {
      any<concept_random_access_iterator<int>> it(vi.begin());
      CLOG_CHECK_EQ(*it, 11);
      CLOG_CHECK_EQ(it[0], 11);
      CLOG_CHECK_EQ(it[1], 12);
      CLOG_CHECK_EQ(it[2], 13);
      CLOG_CHECK(it == it);
      CLOG_CHECK(it + 1 == it + 1);
      CLOG_CHECK_EQ((it + 2) - it, 2);
      CLOG_CHECK(it < (it + 1));
      CLOG_CHECK(it <= (it + 1));
      CLOG_CHECK((it + 1) <= (it + 1));
      CLOG_CHECK((it + 2) > (it + 1));
      CLOG_CHECK((it + 2) >= (it + 1));
    }

    begin(vi);
    any<concept_const_free_begin<any_forward_iterator<int>(void)>> a =
        std::move(vi);
    auto it = begin(a);
    CLOG_CHECK_EQ(*it, 11);
    any<> g = std::move(a);
    CLOG_CHECK(g.table_->type_info == &typeid(vi));
  }

  {
    any<concept_equal, concept_copy_construct> ea = 10, eb = 11;
    any<> a = 10, b = 11;
    CLOG_CHECK(ea == ea);
    CLOG_CHECK(ea != eb);
    CLOG_CHECK(ea == a);
    CLOG_CHECK(a == ea);
    CLOG_CHECK(ea != b);
    CLOG_CHECK(b != ea);
    a = ea;
    CLOG_CHECK_EQ(a.table()->type_info, ea.table()->type_info);
    auto t = any_cast_any<decltype(ea)>(a);
    CLOG_CHECK_EQ((void *)t.table(), (void *)ea.table());
  }

  {
    any<concept_copy_construct> a(10);
    any<concept_copy_construct> b;
    b = a;
    CLOG_CHECK(*a.get<int>() == *b.get<int>());
    any<> c;
    c = std::move(b);
    CLOG_CHECK(c.table()->type_info == &typeid(int));
    CLOG_CHECK_EQ(*c.get<int>(), 10);
  }

  {
    More m;
    m.mesg = "hello";
    any<> a(std::move(m));
    CLOG_CHECK_EQ(a.get<More>()->mesg, "hello");
    any<> b = std::move(a);
    CLOG_CHECK_EQ(b.get<More>()->mesg, "hello");
  }

  {
    any<concept_typeinfo> a(10);
    any<concept_typeinfo> b = std::move(a);
    CLOG_CHECK(&b.typeinfo() == &typeid(10));
  }

  {
    any<concept_call<int(int, int)>> a([](int a, int b) { return a + b; });
    a = Call(13);
    CLOG_CHECK_EQ(a(11, 12), Call(13)(11, 12));
  }

  {
    typedef any_forward_iterator<int> int_iterator;
    std::vector<int> vi;
    any<concept_push_back<void(int)>, concept_const_begin<int_iterator(void)>,
        concept_const_end<int_iterator(void)>> vs = vi;
    CLOG_CHECK(vs.get<std::vector<int>>() != &vi);
    for (int i = 0; i < 10; ++i) {
      int v = random();
      vs.push_back(v);
      vi.push_back(v);
    }
    CLOG_CHECK_EQ(distance(begin(vs), end(vs)), (int)vi.size());
    size_t x = 0;
    for (auto v : vs) {
      CLOG_CHECK_EQ(v, vi[x], "diff at", x);
      ++x;
    }
  }

  {
    std::vector<int> vi;
    for (int i = 0; i < 10; ++i) { vi.push_back(random()); }
    any_range<int> vs = vi;
    size_t x = 0;
    for (auto v : vs) {
      CLOG_CHECK_EQ(v, vi[x], "diff at", x);
      ++x;
    }
  }

  {
    auto func = [](int a, int b) {return a + b; };
    test_func("lambda", func);
  }

  {
    std::function<int(int, int)> func = [](int a, int b) {return a + b; };
    test_func("std::function", func);
  }

  {
    any<concept_call<int(int, int)>> func([](int a, int b) { return a + b;
      });
    test_func("any", func);
  }

  {
    test_func("Call", Call(0));
  }

  {
    any<concept_call<int(int, int)>> func = Call(0);
    test_func("any Call", func);
  }


  {
    any_function_c<int(int, int)> func = Call(0);
    test_func("any_function Call", func);
  }

  {
    std::function<int(int, int)> func = Call(0);
    test_func("std::function Call", func);
  }

  // {
  //   any<concept_call<int(int, int)>, concept_call<double(double, double)>> func = Call(0);
  //   std::cerr << func(11, 13) << std::endl;
  //   std::cerr << func(2.78128, 3.1415926) << std::endl;
  // }
}
