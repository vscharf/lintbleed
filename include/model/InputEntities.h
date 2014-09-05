//---------------- -*- C++ -*- -------------------------------------------------
// model/InputEntities.h
// 
// This file defines the InputEntities which are tracked during AST processing.
//
//------------------------------------------------------------------------------
#ifndef LINTBLEED_INPUTENTITIES_H
#define LINTBLEED_INPUTENTITIES_H

#include <cassert> // for assert
#include <memory> // for std::unique_ptr
#include <utility> // for std::swap

#include "utils/make_unique.h"

// forward decls
namespace clang {
class Decl;
class ValueDecl;
}

namespace LintBleed {

///
/// Encodes a value that refers to user-controlled input.
class InputEntity {
public:
  virtual ~InputEntity() = default;

  std::unique_ptr<InputEntity> clone() const { return do_clone(); }
  
  std::unique_ptr<InputEntity> add_or_sub() {return do_add_or_sub(); }
  std::unique_ptr<InputEntity> mult_or_div() { return do_mult_or_div(); }
  std::unique_ptr<InputEntity> inc_or_dec() { return do_inc_or_dec(); }

  std::unique_ptr<InputEntity> deref() const { return do_deref(); }
  std::unique_ptr<InputEntity> subscript() const { return do_subscript(); }
  std::unique_ptr<InputEntity> addrof() const { return do_addrof(); }

private:
  virtual std::unique_ptr<InputEntity> do_clone() const = 0;

  virtual std::unique_ptr<InputEntity> do_add_or_sub() = 0;
  virtual std::unique_ptr<InputEntity> do_mult_or_div() = 0;
  virtual std::unique_ptr<InputEntity> do_inc_or_dec() = 0;

  virtual std::unique_ptr<InputEntity> do_deref() const = 0;
  virtual std::unique_ptr<InputEntity> do_subscript() const = 0;
  virtual std::unique_ptr<InputEntity> do_addrof() const = 0;
};

template<class T>
class PointerTo : public InputEntity {
private:
  std::unique_ptr<InputEntity> do_clone() const override { return make_unique<PointerTo<T>>(); }
  std::unique_ptr<InputEntity> do_add_or_sub() override { return clone(); }
  std::unique_ptr<InputEntity> do_mult_or_div() override { return clone(); }
  std::unique_ptr<InputEntity> do_inc_or_dec() override { return clone(); }
  std::unique_ptr<InputEntity> do_deref() const override { return make_unique<T>(); }
  std::unique_ptr<InputEntity> do_subscript() const override { return make_unique<T>(); }
  std::unique_ptr<InputEntity> do_addrof() const override {
    return make_unique<PointerTo<PointerTo<T>>>();
  }
};

class InputValue : public InputEntity {
private:
  std::unique_ptr<InputEntity> do_clone() const override { return make_unique<InputValue>(); }
  std::unique_ptr<InputEntity> do_add_or_sub() override { return clone(); }
  std::unique_ptr<InputEntity> do_mult_or_div() override { return clone(); }
  std::unique_ptr<InputEntity> do_inc_or_dec() override { return clone(); }
  std::unique_ptr<InputEntity> do_deref() const override {
    assert(false && "Can't dereference a non-pointer type!"); return 0;
  }
  std::unique_ptr<InputEntity> do_subscript() const override {
    assert(false && "Can't use subscript on a non-pointer type!"); return 0;
  }
  std::unique_ptr<InputEntity> do_addrof() const override {
    return make_unique<PointerTo<InputValue>>();
  }
};

// template specialization to break template recursion
// TODO: eventually use boost's preprocessor library to make 
// the maximum level of recursions compile-time configurable
template<class T>
class PointerTo<PointerTo<PointerTo<PointerTo<PointerTo<T>>>>> : public InputEntity
{
  using my_type = PointerTo<PointerTo<PointerTo<PointerTo<PointerTo<T>>>>>;
  std::unique_ptr<InputEntity> do_clone() const override { return make_unique<my_type>(); }
  std::unique_ptr<InputEntity> do_add_or_sub() override { return clone(); }
  std::unique_ptr<InputEntity> do_mult_or_div() override { return clone(); }
  std::unique_ptr<InputEntity> do_inc_or_dec() override { return clone(); }
  std::unique_ptr<InputEntity> do_deref() const override { return make_unique<T>(); }
  std::unique_ptr<InputEntity> do_subscript() const override { return make_unique<T>(); }
  std::unique_ptr<InputEntity> do_addrof() const override { assert(false && "Too many levels of pointer indirection!"); return 0; }
};

/// Encodes an LValue InputEntity
class InputLValue : public InputEntity {
private:
  /// underlying value
  std::unique_ptr<InputEntity> m_value;
  /// the declaration
  clang::ValueDecl* m_decl;
public:
  InputLValue(std::unique_ptr<InputEntity>&& value, clang::ValueDecl *decl) 
    : m_value(std::move(value)), m_decl(decl) {}

  // default methods
  InputLValue(InputLValue&&) = default;
  ~InputLValue() = default; // we don't own m_decl

  // custom copy/assignemnt
  InputLValue(const InputLValue& other) : m_value(other.m_value->clone()), m_decl(other.m_decl) {}
  InputLValue& operator=(InputLValue other) {
    swap(*this, other);
    return *this;
  }

  friend void swap(InputLValue& first, InputLValue& second) noexcept(true) {
    using std::swap;
    swap(first.m_value, second.m_value);
    swap(first.m_decl, second.m_decl);
  }

  const clang::ValueDecl* decl() const { return m_decl; }
  const clang::Decl* canonicalDecl() const;

private:
  std::unique_ptr<InputEntity> do_clone() const override { return make_unique<InputLValue>(*this); }
  std::unique_ptr<InputEntity> do_add_or_sub() override { return m_value->add_or_sub(); }
  std::unique_ptr<InputEntity> do_mult_or_div() override { return m_value->mult_or_div(); }
  std::unique_ptr<InputEntity> do_inc_or_dec() override { return m_value->inc_or_dec(); }
  std::unique_ptr<InputEntity> do_deref() const override { return m_value->deref(); }
  std::unique_ptr<InputEntity> do_subscript() const override { return m_value->subscript(); }
  std::unique_ptr<InputEntity> do_addrof() const override { return m_value->addrof(); }
    
};

inline bool operator==(const InputLValue& lhs, const InputLValue& rhs) {
  return lhs.canonicalDecl() == rhs.canonicalDecl();
}

}

#endif
