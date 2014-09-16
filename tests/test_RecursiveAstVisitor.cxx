// copied from http://clang.llvm.org/docs/RAVFrontendAction.html and adjusted
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/raw_ostream.h>

using namespace clang;

class UnderstandVisitor
  : public RecursiveASTVisitor<UnderstandVisitor> {
public:
  explicit UnderstandVisitor(ASTContext *Context)
    : Context(Context) {}

  bool VisitStmt(Stmt *S) {
    llvm::errs() << "VisitStmt: " << S->getStmtClassName() << "\n";
    S->dump();
    S->dumpPretty(*Context);
    return true;
  }

private:
  ASTContext *Context;
};

class UnderstandVisitorConsumer : public clang::ASTConsumer {
public:
  explicit UnderstandVisitorConsumer(ASTContext *Context)
    : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  UnderstandVisitor Visitor;
};

class UnderstandVisitorAction : public clang::ASTFrontendAction {
public:
  virtual clang::ASTConsumer *CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return new UnderstandVisitorConsumer(&Compiler.getASTContext());
  }
};

int main(int argc, char **argv) {
  clang::tooling::runToolOnCode(new UnderstandVisitorAction, "void f() { int a; a += 5; }");
}

// TranslationUnitDecl 0x7094a30 <<invalid sloc>> <invalid sloc>
// |- [...]
// `-FunctionDecl 0x7095430 </tmp/vscharf/test.cxx:1:1, line:4:1> line:1:5 main 'int (void)'
//   `-CompoundStmt 0x7095630 <col:12, line:4:1>
//     |-DeclStmt 0x7095598 <line:2:3, col:8>
//     | `-VarDecl 0x7095540 <col:3, col:7> col:7 used a 'int'
//     `-CompoundAssignOperator 0x70955f8 <line:3:3, col:8> 'int' lvalue '+=' ComputeLHSTy='int' ComputeResultTy='int'
//       |-DeclRefExpr 0x70955b0 <col:3> 'int' lvalue Var 0x7095540 'a' 'int'
//       `-IntegerLiteral 0x70955d8 <col:8> 'int' 5

