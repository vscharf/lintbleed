#include "utils/Matchers.h"

#include <cstdlib>

#include <clang/AST/ASTConsumer.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>

using namespace clang;
using namespace clang::ast_matchers;

// test code containting two vardecls
const static std::string testCode = "void f() { int a; a += 5; }";
// will emit the following ast:
// TranslationUnitDecl 0x7094a30 <<invalid sloc>> <invalid sloc>
// |- [...]
// `-FunctionDecl 0x7095430 </tmp/vscharf/test.cxx:1:1, line:4:1> line:1:5 main 'int (void)'
//   `-CompoundStmt 0x7095630 <col:12, line:4:1>
//     |-DeclStmt 0x7095598 <line:2:3, col:8>
//     | `-VarDecl 0x7095540 <col:3, col:7> col:7 used a 'int'
//     `-CompoundAssignOperator 0x70955f8 <line:3:3, col:8> 'int' lvalue '+=' ComputeLHSTy='int' ComputeResultTy='int'
//       |-DeclRefExpr 0x70955b0 <col:3> 'int' lvalue Var 0x7095540 'a' 'int'
//       `-IntegerLiteral 0x70955d8 <col:8> 'int' 5

int main() {
  class TestMatchersConsumer : public ASTConsumer {
  public:
    void HandleTranslationUnit(ASTContext &Context) override {
      DeclarationMatcher declMatcher = varDecl().bind("d");
      struct VarDeclMatcher : public MatchFinder::MatchCallback {
        const VarDecl* MatchedDecl = 0;
        void run(const MatchFinder::MatchResult& Result) override {
          MatchedDecl = Result.Nodes.getNodeAs<clang::VarDecl>("d");
        }
      } varDeclMatcher;
      
      // StatementMatcher stmtMatcher = declRefExpr(to(decl())).bind("d");
      struct RefExprMatcher : public MatchFinder::MatchCallback {
        void run(const MatchFinder::MatchResult& Result) override {
          const DeclRefExpr* dre = Result.Nodes.getNodeAs<clang::DeclRefExpr>("d");
          assert(dre);
          exit(0);
        }
      } refExprMatcher;

      MatchFinder declFinder;
      declFinder.addMatcher(declMatcher, &varDeclMatcher);
      declFinder.matchAST(Context);

      assert(varDeclMatcher.MatchedDecl);

      StatementMatcher stmtMatcher = declRefExpr(
                                                 to(
                                                    decl(
                                                         hasCanonicalDecl(
                                                                          varDeclMatcher.MatchedDecl->getCanonicalDecl()
                                                                          )))).bind("d");
      MatchFinder stmtFinder;
      stmtFinder.addMatcher(stmtMatcher, &refExprMatcher);
      stmtFinder.matchAST(Context);
      assert(false);
    }
  } ;

  class TestAction : public ASTFrontendAction {
  public:
    clang::ASTConsumer* CreateASTConsumer(CompilerInstance &Compiler, llvm::StringRef InFile) override {
      return new TestMatchersConsumer;
    }
  };
  
  clang::tooling::runToolOnCode(clang::tooling::newFrontendActionFactory<TestAction>()->create(), testCode);

  assert(false);
}
