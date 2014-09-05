//---------------- -*- C++ -*- -------------------------------------------------
// model/InputEntities.cxx
// 
// This file defines the InputEntities which are tracked during AST processing.
//
//------------------------------------------------------------------------------

#include "model/InputEntities.h"

#include "clang/AST/Decl.h"

using namespace LintBleed;

const clang::Decl* InputLValue::canonicalDecl() const {
  return m_decl->getCanonicalDecl();
}
