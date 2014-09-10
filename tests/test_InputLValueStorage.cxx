#include "model/InputLValueStorage.h"

#include <string>
#include <utility>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Tooling/Tooling.h>

#include "utils/make_unique.h"

using namespace LintBleed;
using namespace clang;
using namespace clang::ast_matchers;

// test code containting two vardecls
const static std::string testCode = "int decl1; float decl2;";

int main() {
  InputLValueStorage lvs;
  assert(lvs.empty());
  assert(lvs.size() == 0);
  assert(lvs.begin() == lvs.end());

  const InputLValueStorage& clvs(lvs);
  assert(clvs.begin() == clvs.end());

  std::unordered_set<::LintBleed::InputLValue> s(3);
  InputLValueStorage llvs(std::move(s));
  InputLValueStorage lllvs(std::move(llvs));

  int listener_calls {0};
  boost::signals2::scoped_connection conn = 
    lvs.connect_add([&listener_calls](const InputLValueStorage::value_type&){ ++listener_calls; });

  DeclarationMatcher declMatcher = varDecl().bind("d");
  class VarDeclAdder : public MatchFinder::MatchCallback {
    InputLValueStorage& m_lvs;
  public:
    VarDeclAdder(InputLValueStorage& lvs) : m_lvs(lvs) {}
    void run(const MatchFinder::MatchResult& Result) override {
      const VarDecl *vd = Result.Nodes.getNodeAs<clang::VarDecl>("d");
      assert(vd);
      m_lvs.insert(InputLValue(make_unique<InputValue>(), vd));
    }
  } varDeclAdder {lvs};
  
  MatchFinder finder;
  finder.addMatcher(declMatcher, &varDeclAdder);

  // will add two objects to lvs
  clang::tooling::runToolOnCode(clang::tooling::newFrontendActionFactory(&finder)->create(), testCode);

  assert(!lvs.empty());
  assert(lvs.size() == 2);
  assert(listener_calls == 2);

  for(auto i : lvs) {
    --listener_calls;
  }
  assert(!listener_calls);
}
