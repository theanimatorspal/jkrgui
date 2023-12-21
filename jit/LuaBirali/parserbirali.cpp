#include "parserbirali.hpp"

#include "astbirali.hpp"
#include "confbirali.hpp"

using namespace Birali;
using namespace Birali::Ast;

Parser::Parser(std::istream &inStream) : Birali::Lexer(inStream) {
  mBinaryOperatorPrecedence['\n'] = 1;
  mBinaryOperatorPrecedence['='] = 2;
  mBinaryOperatorPrecedence['<'] = 10;
  mBinaryOperatorPrecedence['+'] = 20;
  mBinaryOperatorPrecedence['-'] = 20;
  mBinaryOperatorPrecedence['*'] = 40;
  mBinaryOperatorPrecedence['/'] = 40;
}

ccode Parser::GetNextToken() {
  mCurrentToken = (Token)Lexer::GetToken();
  while (IsSpaceExt(mCurrentToken)) {
    mCurrentToken = (Token)Lexer::GetToken();
  }
  return mCurrentToken;
}

int Parser::GetTokenPrecedence() {
  if (not IsPrint(mCurrentToken) and IsSpaceExt(mCurrentToken)) {
    return -1;
  }

  int TokenPrecedence = mBinaryOperatorPrecedence[mCurrentToken];
  if (TokenPrecedence <= 0) return -1;
  return TokenPrecedence;
}

std::unique_ptr<Ast::Number> Parser::ParseNumberExpression() {
  auto Result = mu<Ast::Number>(Type::Double, Lexer::GetTokenizedNumber());
  mSASTree << "NumberExpression : " << Lexer::GetTokenizedNumber() << "\n";
  GetNextToken();
  return mv(Result);
}

std::unique_ptr<Ast::StringLiteral> Parser::ParseStringLiteralExpression() {
  mSASTree << "String Literal:" << '\n';
  s Value = GetTokenizedStringLiteral();
  mSASTree << Value << '\n';
  GetNextToken();
  return mu<Ast::StringLiteral>(Type::String, Value);
}

std::unique_ptr<Ast::Expression> Parser::ParseExpression() {
  mSASTree << "Expression: "
           << "\n";
  auto LHS = ParsePrimary();
  if (not LHS) return nullptr;
  return ParseBinaryOperationRHS(0, std::move(LHS));
}

std::unique_ptr<Ast::Expression> Parser::ParseParentExpression() {
  mSASTree << "ParentExpression:"
           << "\n";
  GetNextToken();
  auto V = ParseExpression();
  if (not V) return nullptr;

  if (mCurrentToken != ')') return LogError("Expected ')'");
  GetNextToken();
  return V;
}

std::unique_ptr<Ast::Expression> Parser::ParseIdentifierExpression() {
  mSASTree << "Identifier :" << Lexer::GetTokenizedIdentifier() << '\n';
  std::string IdName = Lexer::GetTokenizedIdentifier();
  GetNextToken();
  if (mCurrentToken != '(') return mu<Ast::Variable>(Type::Unknown, IdName);
  GetNextToken();
  mSASTree << "-----------------------" << '\n';
  mSASTree << "FunctionCall:" << '\n';
  v<up<Ast::Expression>> Args;
  if (mCurrentToken != ')') {
    while (true) {
      if (auto Arg = ParseExpression()) {
        Args.push_back(mv(Arg));
      } else {
        return nullptr;
      }
      if (mCurrentToken == ')') break;
      if (mCurrentToken != ',')
        return LogError("Expected ')' or ',' in argument list ");
      GetNextToken();
    }
  }
  GetNextToken();
  mSASTree << "-----------------------" << '\n';
  return mu<Ast::Call>(Type::Unknown, IdName, mv(Args));
}

std::unique_ptr<Ast::Expression> Parser::ParsePrimary() {
  mSASTree << "Primary :"
           << "\n";
  switch (mCurrentToken) {
    default:
      return LogError("Unknown Token When Expecting an Expression");
    case Token::Local:
      return ParseLocalExpression();
      break;
    case Token::Identifier:
      return ParseIdentifierExpression();
      break;
    case Token::Number:
      return ParseNumberExpression();
      break;
    case Token::StringLiteral:
      return ParseStringLiteralExpression();
      break;
    case Token::Return:
      return ParseReturnExpression();
      break;
    case '(':
      return ParseParentExpression();
      break;
  }
}

std::unique_ptr<Ast::Expression> Parser::ParseBinaryOperationRHS(
    int inExpressionPrecedence, std::unique_ptr<Ast::Expression> inLHS) {
  while (true) {
    int TokenPrecedence = GetTokenPrecedence();
    if (TokenPrecedence < inExpressionPrecedence) return inLHS;

    int BinaryOperator = mCurrentToken;
    mSASTree << "BinaryRHS:" << (char)mCurrentToken << "\n";
    GetNextToken();
    auto RHS = ParsePrimary();
    if (not RHS) return nullptr;
    int NextPrecedence = GetTokenPrecedence();
    if (TokenPrecedence < NextPrecedence) {
      RHS = ParseBinaryOperationRHS(TokenPrecedence + 1, mv(RHS));
      if (not RHS) return nullptr;
    }
    inLHS = mu<Ast::Binary>(RHS->mType, BinaryOperator, mv(inLHS), mv(RHS));
  }
}

up<Ast::LocalVariable> Birali::Parser::ParseLocalExpression() {
  Type type = Type::Unknown;
  GetNextToken();
  if (mCurrentToken != Token::Identifier)
    return LogError<Ast::LocalVariable>(
        "Identifier Expected After 'local' keyword");
  s Name = GetTokenizedIdentifier();
  mSASTree << "-----------------------" << '\n';
  mSASTree << "Local : " << Name << '\n';
  GetNextToken();
  if (mCurrentToken != '=')
    return LogError<Ast::LocalVariable>("Expected '=' after 'local' keyword");
  GetNextToken();
  auto Exp = ParseExpression();
  mSASTree << "-----------------------" << '\n';
  return mu<Ast::LocalVariable>(Exp->mType, Name, mv(Exp));
}

up<Ast::Prototype> Birali::Parser::ParsePrototype() {
  mSASTree << "<<" << '\n';
  mSASTree << "Function Prototype:" << '\n';
  s FunctionName;
  FunctionName = GetTokenizedIdentifier();
  mSASTree << FunctionName << '\n';
  GetNextToken();
  if (mCurrentToken != '(')
    return LogError<Ast::Prototype>("Expected '(' after function name");
  v<s> ArgumentNames;
  while (true) {
    if (GetNextToken() == Token::Identifier) {
      ArgumentNames.push_back(Lexer::GetTokenizedIdentifier());
      if (GetNextToken() != ',') {
        break;
      }
    } else {
      return LogError<Ast::Prototype>("Expected Argument name");
    }
  }
  if (mCurrentToken != ')')
    return LogError<Ast::Prototype>("Expected ')' in Prototype");

  GetNextToken();
  mSASTree << ">>" << '\n';
  return mu<Ast::Prototype>(Type::Unknown, FunctionName, ArgumentNames);
}

up<Ast::Function> Birali::Parser::ParseFunctionDefinition() {
  mSASTree << "-----------------------" << '\n';
  mSASTree << "Function Definition:" << '\n';
  GetNextToken();
  auto Prototype = ParsePrototype();
  Lexer::SetShouldIgnoreNewLines(false);
  if (not Prototype) return nullptr;
  auto Body = ParseExpression();
  if (mCurrentToken != Token::End) {
    return LogError<Ast::Function>("Expected 'end' after function definition");
  }
  Lexer::SetShouldIgnoreNewLines(true);
  mSASTree << "-----------------------" << '\n';
  return mu<Ast::Function>(Body->mType, mv(Prototype), mv(Body));
}

up<Ast::Return> Birali::Parser::ParseReturnExpression() {
  bool NewLinesIgnored = IsNewLinesIgnored();
  if (not NewLinesIgnored) SetShouldIgnoreNewLines(true);

  mSASTree << "Return" << '\n';
  GetNextToken();
  auto Value = ParseExpression();
  if (not Value) return LogError<Ast::Return>("Expected an Expression");

  SetShouldIgnoreNewLines(NewLinesIgnored);
  return mu<Ast::Return>(Value->mType, mv(Value));
}

up<Ast::Function> Birali::Parser::ParseTopLevelExpression() {
  if (auto E = ParseExpression()) {
    auto Proto = mu<Prototype>(E->mType, mAnonymousNameGenerator.New(),
                               v<s>()); 
    return mu<Ast::Function>(Proto->mType, mv(Proto), mv(E));
  }
  return nullptr;
}
