//---------------- -*- C++ -*- -------------------------------------------------
// utils/Matchers.h
// 
// This file defines custom AST matchers for this project.
//
//------------------------------------------------------------------------------
#include <clang/AST/DeclFriend.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/ASTMatchers/ASTMatchersInternal.h>
#include <clang/ASTMatchers/ASTMatchersMacros.h>
#include <llvm/Support/raw_ostream.h>

namespace clang {
namespace ast_matchers {
/// \brief Matches Decl nodes that have the specified canonical declaration.
AST_MATCHER_P(Decl, hasCanonicalDecl, const Decl*, canonicalDecl) {
  llvm::outs() << "hasCanonicalDecl was called!\n";
  assert(canonicalDecl);
  llvm::outs() << "Node.getCanonicalDecl(): " << Node.getCanonicalDecl() << "\n";
  llvm::outs() << "canonicalDecl: " << canonicalDecl << "\n";
  return Node.getCanonicalDecl() == canonicalDecl;
}
} // end namespace ast_matchers
} // end namespace clang
