#ifndef FILE_60CEA141_A52C_4695_95D1_62C3FB8B6E5A_H
#define FILE_60CEA141_A52C_4695_95D1_62C3FB8B6E5A_H
#include <atomic>
#include <mutex>
#include <typeinfo>
#include <utility>
#include <memory>
#include <vector>

namespace smo_any {

template <class T, class ...>
inline T *singleton_get() {
  static T t;
  return &t;
}

// we don't own/manage any the pointer
template <class V>
struct type_dict {
  typedef V *Value;
  typedef const std::type_info *Key;

  struct State {
    State *old;
    size_t size;
    size_t buckets;
    std::pair<Key, Value> table[0];
  };
  std::atomic<State *> state_;
  std::mutex add_mutex_;
  std::vector<std::shared_ptr<void>> ptrs_;

  type_dict() {
    const size_t buckets = 8;
    auto state = (State *)calloc(
        sizeof(State) + sizeof(state_.load()->table[0]) * buckets, 1);
    state->buckets = buckets;
    state_.store(state);
  }

  ~type_dict() {
    auto state = state_.load();
    while (state) {
      auto old = state->old;
      free(state);
      state = old;
    }
  }

  static size_t hash(const Key &k) {
    return reinterpret_cast<intptr_t>(k) >> 2;
  }

  Value find(const Key &k) {
    auto state = state_.load(std::memory_order_relaxed);
    size_t b = hash(k) & (state->buckets - 1);
    size_t idx = 0;
    while (1) {
      auto &o = state->table[b];
      if (o.first == k) {
        return o.second;
      }
      if (o.first == Key()) break;
      b = (b + ++idx) & (state->buckets - 1);
    }
    return Value();
  }

  void resize(int dir) {
    assert(dir == 1);
    auto old = state_.load(std::memory_order_acquire);
    size_t buckets = old->buckets * 2;
    State *state =
        (State *)malloc(sizeof(State) + sizeof(old->table[0]) * buckets);
    state->old = old;
    state->size = old->size;
    state->buckets = buckets;
    for (size_t i = 0; i < buckets; ++i) {
      state->table[i].first = Key();
    }
    for (size_t i = 0; i < old->buckets; ++i) {
      auto &o = old->table[i];
      if (o.first == Key()) continue;
      size_t b = hash(o.first) & (state->buckets - 1);
      size_t idx = 0;
      while (state->table[b].first != Key()) {
        b = (b + ++idx) & (state->buckets - 1);
      }
      state->table[b] = o;
    }
    state_.store(state, std::memory_order_release);
  }

  std::pair<Value &, bool> add(const Key &k, std::unique_ptr<V> &value) {
    auto v_b = add(k, value.get());
    if (v_b.second) {
      assert(value.get() == v_b.first);
      ptrs_.emplace_back(std::move(value));
      return v_b;
    }
    return v_b;
  }

  std::pair<Value &, bool> add(const Key &k, const Value &value) {
    std::lock_guard<std::mutex> lk(add_mutex_);
    auto state = state_.load(std::memory_order_acquire);
    if ((state->size + 1) * 5 >= 4 * state->buckets) {
      resize(1);
      state = state_.load(std::memory_order_relaxed);
    }
    size_t b = hash(k) & (state->buckets - 1);
    size_t idx = 0;
    while (1) {
      auto &o = state->table[b];
      if (o.first == k) {
        return std::pair<Value &, bool>{o.second, false};
      }
      if (o.first == Key()) {
        o.second = value;
        o.first = k;
        ++state->size;
        return std::pair<Value &, bool>{o.second, true};
      }
      b = (b + ++idx) & (state->buckets - 1);
    }
  }
};


}  // namespace smo_any
#endif // FILE_60CEA141_A52C_4695_95D1_62C3FB8B6E5A_H
