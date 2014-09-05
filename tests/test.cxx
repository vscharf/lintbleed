#include "InputEntities.h"
#include <cassert>
#include <typeinfo>
#include <iostream>

using namespace LintBleed;

template<typename T1, typename T2>
struct SameType {
  bool operator()() { return false; }
};

template <typename T>
struct SameType<T, T> {
  bool operator()() { return true; }
};

namespace clang {
  class ValueDecl {} ;
}

int main() {
  auto v = make_unique<InputValue>();
  auto p = v->addrof();
  auto pp = p->addrof();

  static_assert(std::is_nothrow_move_constructible<InputValue>::value,
                "InputValue should move construct without an exception");
  static_assert(std::is_nothrow_move_constructible<PointerTo<InputValue>>::value,
                "PointerTo<InputValue> should move construct without an exception");
  static_assert(std::is_nothrow_move_constructible<InputLValue>::value,
                "InputLValue should move construct without an exception");

  static_assert(std::is_move_assignable<InputLValue>::value,
                "InputLValue should move assign");
  static_assert(std::is_nothrow_move_assignable<InputLValue>::value,
                "InputLValue should move assign without an exception");

  static_assert(std::is_nothrow_copy_constructible<InputLValue>::value,
                "InputLValue should copy construct without an exception");
  static_assert(std::is_nothrow_copy_assignable<InputLValue>::value,
                "InputLValue should copy assign without an exception");

  InputLValue lv1 {v->addrof(), nullptr}, lv2 {p->deref(), nullptr};
  lv2 = std::move(lv1);

  assert( (dynamic_cast<InputValue*>(p->deref().get())) );
  assert( (dynamic_cast<PointerTo<PointerTo<InputValue>>*>(p->addrof().get())) );
  assert( (dynamic_cast<PointerTo<PointerTo<InputValue>>*>(pp.get())) );
}
