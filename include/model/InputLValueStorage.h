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

#include <boost/signals2.hpp>

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
  typedef std::function<void(const value_type&)> signal_type;

private:
  boost::signals2::signal<void(const value_type&)> sig_insert;

public:
  ~InputLValueStorage() = default;
  InputLValueStorage(InputLValueStorage&&) = default;
  InputLValueStorage& operator=(InputLValueStorage&&) = default;

  // copy is not allowed (signals cannot be copied)
  // provide both overloads other the templated version will be a better match sometimes
  InputLValueStorage(InputLValueStorage&) = delete;
  InputLValueStorage(const InputLValueStorage&) = delete;
  InputLValueStorage& operator=(const InputLValueStorage&) = delete;

  // allow invocation as-if this class was of container_type
  template<typename ...Args> InputLValueStorage(Args&& ...args) : m{std::forward<Args>(args)...} {}

  /// connect to the signal emitted directly before a new item is added to InputLValueStorage
  boost::signals2::connection connect_add(const signal_type& s) {
    return sig_insert.connect(s);
  }

  //============================================================================
  // implementation of the underlying container interface
  // (add signals to insert / emplace)
  //============================================================================
  bool empty() const noexcept { return m.empty(); }
  size_type size() const noexcept { return m.size(); }
  size_type max_size() const noexcept { return m.max_size(); }
  
  iterator begin() noexcept { return m.begin(); }
  const_iterator begin() const noexcept { return m.begin(); }
  local_iterator begin(size_type n) noexcept { return m.begin(n); }
  const_local_iterator begin(size_type n) const noexcept { return m.begin(n); }

  iterator end() noexcept { return m.end(); }
  const_iterator end() const noexcept { return m.end(); }
  local_iterator end(size_type n) noexcept { return m.end(n); }
  const_local_iterator end(size_type n) const noexcept { return m.end(n); }

  const_iterator cbegin() const noexcept { return m.cbegin(); }
  const_local_iterator cbegin(size_type n) const noexcept { return m.cbegin(n); }
  const_iterator cend() const noexcept { return m.cend(); }
  const_local_iterator cend(size_type n) const noexcept { return m.cend(n); }

  iterator find(const key_type& k) { return m.find(k); }
  const_iterator find(const key_type& k) const { return m.find(k); }
  size_type count(const key_type& k) { return m.count(k); }

  std::pair<iterator,iterator> equal_range(const key_type& k) { return m.equal_range(k); }
  std::pair<const_iterator,const_iterator> equal_range(const key_type& k) const { return m.equal_range(k); }

  std::pair<iterator,bool> insert(const value_type& val) {
    sig_insert(val);
    return m.insert(val);
  }
  std::pair<iterator,bool> insert(value_type&& val) {
    sig_insert(val);
    return m.insert(std::move(val));
  }
  iterator insert(const_iterator hint, const value_type& val) {
    sig_insert(val);
    return m.insert(hint, val);
  }
  iterator insert(const_iterator hint, value_type&& val) {
    sig_insert(val);
    return m.insert(hint, std::move(val));
  }
  template <class InputIterator> void insert(InputIterator first, InputIterator last) {
    while(first != last) sig_insert(*(first++));
    m.insert(first, last);
  }
  // not available
  // void insert(std::initializer_list<value_type> il) {
  //   m.insert(il);
  // }
};
}



#endif
