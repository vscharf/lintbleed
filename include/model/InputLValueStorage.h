//---------------- -*- C++ -*- -------------------------------------------------
// model/InputLValueStorage.h
// 
// This file defines the InputLValueStorage which stores InputLValues.
//
//------------------------------------------------------------------------------
#ifndef LINTBLEED_INPUTLVALUESTORAGE_H
#define LINTBLEED_INPUTLVALUESTORAGE_H

#include <unordered_set>
#include <utility>

#include "model/InputEntities.h"

namespace std {
template<> struct hash<::LintBleed::InputLValue> {
  size_t operator()(const LintBleed::InputLValue& x) const {
    return hash<const clang::Decl*>()(x.canonicalDecl());
  }
};
}

namespace LintBleed {

class InputLValueStorage {
  typedef std::unordered_set<::LintBleed::InputLValue> container_type;
  container_type m;
public:
  typedef typename container_type::key_type key_type;
  typedef typename container_type::value_type value_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::difference_type difference_type;
  typedef typename container_type::allocator_type allocator_type;
  typedef typename container_type::pointer pointer;
  typedef typename container_type::const_pointer const_pointer;
  typedef typename container_type::reference reference;
  typedef typename container_type::const_reference const_reference;
  typedef typename container_type::iterator iterator;
  typedef typename container_type::const_iterator const_iterator;
  typedef typename container_type::local_iterator local_iterator;
  typedef typename container_type::const_local_iterator const_local_iterator;

  ~InputLValueStorage() = default;
  template<typename ...Args> InputLValueStorage(Args&& ...args) : m {std::forward<Args>(args)...} {}


};
}



#endif
