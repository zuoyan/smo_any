# Small Object Optimization Any

This's a small library for type erasure, like std::function, boost::any.

# Usage

```c++
template <class Sig>
struct concept_const_call;

template <class R, class ...Args>
struct concept_const_call<R(Args...)> {
  template <class T>
  static R type_func(const T& ptr, Args...args) {
    return ptr(args...);
  }

  struct Table {
    R (*func)(const void*, Args...);

    // T is the under type, set function pointer.
    template <class T>
	void reification() {
	  func = reinterpret_cast<decltype(func)>(type_fun<T>);
	}
  };

  // Derived are the instance of basic_any<...>,
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
