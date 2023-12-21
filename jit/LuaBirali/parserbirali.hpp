#pragma once
#include <sstream>

#include "astbirali.hpp"
#include "lexerbirali.hpp"

namespace Birali {
class Parser : public Birali::Lexer {
 public:
  Parser(std::istream &inStream);
  ccode GetNextToken();
  ccode GetCurrentToken() const { return mCurrentToken; }
  int GetTokenPrecedence();
  std::stringstream mSASTree;
  void PrintAST() { std::cout << mSASTree.rdbuf() << "\n"; }

 public:
  template <typename T = Ast::Expression>
  up<T> LogError(const sv inString) {
    std::cout << "Error:" << inString << " at ";
    std::cout << "Line:" << GetLineLexed() << '\n';
    return nullptr;
  }

  up<Ast::Number> ParseNumberExpression();
  up<Ast::StringLiteral> ParseStringLiteralExpression();
  up<Ast::Expression> ParseExpression();
  up<Ast::Expression> ParseParentExpression();
  up<Ast::Expression> ParseIdentifierExpression();
  up<Ast::Expression> ParsePrimary();
  up<Ast::Expression> ParseBinaryOperationRHS(int inExpressionPrecedence,
                                              up<Ast::Expression> inLHS);
  up<Ast::LocalVariable> ParseLocalExpression();
  up<Ast::Prototype> ParsePrototype();
  up<Ast::Function> ParseFunctionDefinition();
  up<Ast::Return> ParseReturnExpression();
  up<Ast::Function> ParseTopLevelExpression();

 public:
  AnonymousExpressionNameGenerator mAnonymousNameGenerator;

 private:
  map<char, int> mBinaryOperatorPrecedence;
  Token mCurrentToken;
};

}  // namespace Birali
