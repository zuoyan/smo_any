# Small Object Optimization Any

This's a small library for type erasure, like std::function, boost::any.

## Usage

```c++
template <class Sig>
struct concept_const_call;

template <class R, class ...Args>
struct concept_const_call<R(Args...)> {
  template <class T>
  static R type_func(const T& v, Args...args) {
    return v(args...);
  }

  struct Table {
    R (*func)(const void*, Args...);

    // T is the under type, set function pointer.
    template <class T>
    void reification() {
      func = reinterpret_cast<decltype(func)>(type_fun<T>);
    }
  };

  // Derived is one instance of basic_any<...>,
  // the final type with all concepts.
  template <class Derived>
  struct Model {
    R operator()(Args...args) const {
      auto derived = static_cast<const Derived*>(this);
      return derived->table()->func(derved->data(), args...);
    }
  }
};
```

Now 'any<concept_const_call<Sig>>' is almost the same as std::function<Sig>, but
it's move constructable. 'any<concept_copy_construct, concept_const_call<Sig>>'
is copy constructable.

any<concept_const_call<Sig>> is slower than std::function<Sig>, measurable for
empty functions. I think, it due to the data function pointer, another indirect
layer, which should check the data is in place or in heap. We can erase this
indirect in reification stage. 'any<concept_any_function<Sig>>' is almost the
same speed as std::function<Sig>.

For more examples, please check examples under smo_any/concept directory.


## Dynamic Cast

As a experiment, this library also provides dynamic casting from any object to
another any, based on the underline types. It try to find every atomically
operations table, and combine them to got all operations. I hope one day, C++
standard will support dynamic inspection.

```c++
struct Object {
  int operator()(int v) const {
    return v * v;
  }
};

Object obj;

// This will register Object as one of concept_call<int(int)>.
any<concept_call<int(int)>> a = obj;

// This will register Object as one of concept_copy_construct.
any<concept_copy_construct> c = obj;

// b is really erased.
any<> b = obj;

// This will find the copy and call functions through the already registered table.
any<concept_call<int(int)>> c = any_cast_any<any<concept_call<int(int)>>>(b);

// Same as above line, but don't need copy.
c = any_cast_any<any<concept_call<int(int)>>>(std::move(b));

assert(c);
assert(c.table()->type_info == &typeid(Object));
assert(c(13) == obj(13));

any<> d = std::function<int(int)>(obj);

// This will abort. Because std::function<int(int) is not registered, we can't
// find the copy and call functions.
any<concept_call<int(int)>> c = any_cast_any<any<concept_call<int(int)>>>(d);
```

Please note that this dynamic cast resolves table just checking
current type, it doesn't check the bases. For that purpose, please
refer to [multi_method](http://github.com/zuoyan/multi_method).
