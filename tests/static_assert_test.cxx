#include "model/InputEntities.h"
#include <cassert>
#include <typeinfo>
#include <iostream>

using namespace LintBleed;

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
  static_assert(std::is_nothrow_move_assignable<InputLValue>::value,
                "InputLValue should move construct without an exception");


  static_assert(std::is_move_assignable<InputLValue>::value,
                "InputLValue should move assign");

  InputLValue lv1 {v->addrof(), (clang::ValueDecl*)0xDEADBABE}, lv2 {p->deref(), (clang::ValueDecl*)0xCAFEBABE};
  assert( llvm::isa<PointerTo<InputValue>>((lv1.inc_or_dec()).get()) == true );


  assert( llvm::isa<InputValue>(v.get()) == true );
  assert( llvm::isa<PointerTo<InputValue>>(p.get()) == true );
  assert( llvm::isa<InputLValue>(&lv1) == true );

  assert( llvm::isa<InputValue>(p->deref().get()) == true );
  assert( llvm::isa<PointerTo<PointerTo<InputValue>>>(p->addrof().get()) == true );
  assert( llvm::isa<PointerTo<PointerTo<InputValue>>>(pp.get()) == true );

  assert( llvm::isa<InputValue>((lv2.add_or_sub()).get()) == true );

  lv2 = std::move(lv1);
  assert( llvm::isa<PointerTo<InputValue>>((lv2.mult_or_div()).get()) == true );
}
