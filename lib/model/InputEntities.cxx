//---------------- -*- C++ -*- -------------------------------------------------
// model/InputEntities.cxx
// 
// This file defines the InputEntities which are tracked during AST processing.
//
//------------------------------------------------------------------------------

#include "model/InputEntities.h"

using namespace LintBleed;

const clang::Decl* InputLValue::canonicalDecl() const {
  return m_decl->getCanonicalDecl();
}

// std::unique_ptr<InputEntity> InputEntity::clone() const {
//   return do_clone();
// }
