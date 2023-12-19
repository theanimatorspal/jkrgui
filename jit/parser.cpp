#include "parser.hpp"

#include "expressions.hpp"
#include "lexer.hpp"

using namespace Expr;
void Parser::MainLoop() {
  bool run = true;
  std::cout << "Ready :: \n";
  while (run) {
    switch (mCurrentToken) {
      case Token::Eof:
        run = false;
        break;
      case ';':
      case Token::End:
        GetNextToken();
        break;
      case Token::Function:
        HandleDefinition();
        break;
      case Token::External:
        HandleExtern();
        break;
      default:
        HandleTopLevelExpression();
        break;
    }
  }
}

std::unique_ptr<Expression> Parser::LogError(const sv inString) {
  std::cout << "Error : " << inString << '\n';
  return nullptr;
}

std::unique_ptr<Prototype> Parser::LogErrorPrototype(const sv inString) {
  LogError(inString);
  return nullptr;
}

std::unique_ptr<Expression> Parser::ParseNumberExpression() {
  auto Result = mu<Expr::Number>(
      Lexer::GetTokenizedNumber());  // GetNextToken garnu vanda agaadi garnu
                                     // parxa kinaki,
  // tei number vetera nai aako hunxa yo function ma chae
  GetNextToken();
  return mv(Result);
}

// expression ::= primary binoperation
std::unique_ptr<Expression> Parser::ParseExpression() {
  auto LHS = ParseUnary();
  if (not LHS) return nullptr;
  return ParseBinaryOperationRHS(0, std::move(LHS));
}

std::unique_ptr<Expression> Parser::ParseParentExpression() {
  GetNextToken();  // '(' vetera yaa aako hunxa, so tellai khanu paro
  auto V = ParseExpression();
  if (not V) return nullptr;

  if (mCurrentToken != ')') return LogError("Expected ')'");
  GetNextToken();  // aba ( paxi ) khojera tellai khana paro
  return V;
}

std::unique_ptr<Expression> Parser::ParseIdentifierExpression() {
  std::string IdName = Lexer::GetTokenizedIdentifier();
  GetNextToken();  // Identifier lai vetera nai yaa aako hunxa, so tellai khanu
                   // paro (xodnu paro)
  if (mCurrentToken != '(')
    return mu<Variable>(
        IdName);  // bracket open vako xaina, so function call hoina vane chae
  // Tara aba function call ho vane chae
  GetNextToken();  // Identifier paxi ko '(' lai khana paro, mathi ko
                   // GetNextToken le chae '(' lai Current banaidinxa
  v<up<Expression>> Args;
  if (mCurrentToken != ')') {
    while (true) {
      if (auto Arg = ParseExpression()) {
        Args.push_back(mv(Arg));
      } else {
        return nullptr;
      }
      if (mCurrentToken == ')') break;
      if (mCurrentToken != ',')  // Comma aayo vane pani true vairakhxa, matlab
                                 // ) naaaunjel samma true nai hunxa
        return LogError("Expected ')' or ',' in argument list ");
      GetNextToken();
    }
  }
  // Aba chae last wala ) lai pani khana paro
  GetNextToken();
  return mu<Call>(IdName, mv(Args));
}

std::unique_ptr<Expression> Parser::ParsePrimary() {
  switch (mCurrentToken) {
    default:
      return LogError("Unknown Token When Expecting an Expression");
    case Token::Identifier:
      return ParseIdentifierExpression();
      break;
    case Token::Number:
      return ParseNumberExpression();
      break;
    case '(':
      return ParseParentExpression();
      break;
    case Token::If:
      return ParseIfExpression();
      break;
    case Token::For:
      return ParseForExpression();
      break;
  }
}

std::unique_ptr<Expression> Parser::ParseBinaryOperationRHS(
    int inExpressionPrecedence, std::unique_ptr<Expression> inLHS) {
  while (true) {
    int TokenPrecedence = GetTokenPrecedence();
    if (TokenPrecedence <
        inExpressionPrecedence)  // inexpressionprecedecne = 0, so Token
                                 // precedence negative xa vane
      return inLHS;  // esto case ma rhs nai xaina so, lhs j xa tei return garne
                     // (yo break condition ho,
    // jaba samma yo hunna taba samma loop chalairakhne

    // aba mathi ko maa kaam vaena vane yo binary operation ho
    int BinaryOperator =
        mCurrentToken;  // GetTokenPrecedence() le kaam garyo matlab,
                        // currentToken ma BinaryOperator xa
    GetNextToken();     // tyo binary operator lai khana paro, aba RHS xa
                        // mCurrentToken ma

    // binary operator paxi unary parse garne
    auto RHS = ParseUnary();  // parse unary call garne  // aba RHS ko ni
                              // primary parse (identifier
                              //  sidentifier garne)
    if (not RHS) return nullptr;
    // mathi ko ParsePrimary le aba Token ma Operator gako huna sakxa
    int NextPrecedence = GetTokenPrecedence();
    if (TokenPrecedence < NextPrecedence) {
      RHS = ParseBinaryOperationRHS(
          TokenPrecedence + 1,
          mv(RHS));  // a+ b *c esto vayo vane a+ aba ahile ko RHS lai
      // LHS banaune ani feri esaima pathaune so suru ma b * c le kaam garos
      if (not RHS) return nullptr;
    }
    inLHS = mu<Binary>(BinaryOperator, mv(inLHS), mv(RHS));
  }
}

up<Expr::Expression> Parser::ParseUnary() {
  // operator haina vane matra
  bool is_not_print = not(mCurrentToken > 0 and std::isprint(mCurrentToken));
  if (is_not_print or mCurrentToken == '(' or mCurrentToken == ',')
    return ParsePrimary();

  // if unary operator tellai padhne
  int Opc = mCurrentToken;
  GetNextToken();
  if (auto Operand = ParseUnary()) return mu<Expr::Unary>(Opc, mv(Operand));

  return nullptr;
}

std::unique_ptr<Prototype> Parser::ParsePrototype() {
  std::string FunctionName;
  uint32_t KindOfFunction = 0;  // 0 = function, 1 = unary, 2 = binary;
  uint32_t BinaryPrecedence = 30;
  switch (mCurrentToken) {
    default:
      return LogErrorPrototype("Expected a function name in the Prototype");
      break;
    case Token::Identifier:
      FunctionName = GetTokenizedIdentifier();
      KindOfFunction = 0;
      GetNextToken();
      break;
    case Token::BinaryOP: {
      GetNextToken();
      bool is_not_print =
          not(mCurrentToken > 0 and std::isprint(mCurrentToken));
      if (is_not_print) {
        return LogErrorPrototype("Expected binary Operator");
      }
      FunctionName = "binary";
      FunctionName += (char)mCurrentToken;
      KindOfFunction = 2;
      GetNextToken();
      if (mCurrentToken == Token::Number) {
        double NumberValue = GetTokenizedNumber();
        if (NumberValue < 1 or NumberValue > 100)
          return LogErrorPrototype(
              "Invalid precedence, it should be betweeen 1 and 100");
        BinaryPrecedence = (uint32_t)NumberValue;
        GetNextToken();
      }
      break;
    }
    case Token::UnaryOP: {
      GetNextToken();
      bool is_not_print =
          not(mCurrentToken > 0 and std::isprint(mCurrentToken));
      if (is_not_print) return LogErrorPrototype("Expected Unary Operator");
      FunctionName = "unary";
      FunctionName += (char)mCurrentToken;
      KindOfFunction = 1;
      GetNextToken();
      break;
    }
  }
  if (mCurrentToken != '(')
    return LogErrorPrototype("Expected '(' in Prototype");

  std::vector<std::string> ArgumentNames;
  while (GetNextToken() == Token::Identifier)
    ArgumentNames.push_back(GetTokenizedIdentifier());
  if (mCurrentToken != ')')
    return LogErrorPrototype("Expected ')' in Prototype");

  GetNextToken();  // khane ')'

  if (KindOfFunction and ArgumentNames.size() != KindOfFunction)
    return LogErrorPrototype("Invalid number of Operands of Operators");

  return mu<Expr::Prototype>(FunctionName, mv(ArgumentNames),
                             KindOfFunction != 0, BinaryPrecedence);
}

std::unique_ptr<Expr::Function> Parser::ParseDefinition() {
  GetNextToken();  // "function" keyword lai khane
  auto Proto = ParsePrototype();
  if (not Proto) return nullptr;

  if (auto Body = ParseExpression())
    return mu<Expr::Function>(mv(Proto), mv(Body));
  return nullptr;
}

std::unique_ptr<Expr::Function> Parser::ParseTopLevelExpression() {
  if (auto E = ParseExpression()) {
    // aba euta anonymouse prototype banaidine
    auto Proto = mu<Prototype>(mAnonymousNameGenerator.New(),
                               v<s>());  // kunai argument navako
    return mu<Expr::Function>(mv(Proto), mv(E));
  }
  return nullptr;
}

std::unique_ptr<Prototype> Parser::ParseExtern() {
  GetNextToken();  // external keyword lai khaidine
  return ParsePrototype();
}

std::unique_ptr<Expression> Parser::ParseIfExpression() {
  GetNextToken();  // suru ko if khane
  auto Condition = Parser::ParseExpression();
  if (mCurrentToken != Token::Then) {
    LogError("Expected Then");
    return nullptr;
  }

  GetNextToken();  // Then lai khane
  auto Then = Parser::ParseExpression();
  if (not Then) return nullptr;
  if (mCurrentToken != Token::Else) {
    LogError("Expected Else");
    return nullptr;
  }
  GetNextToken();  // Else lai khane
  auto Else = Parser::ParseExpression();
  if (not Else) return nullptr;

  return mu<Expr::If>(mv(Condition), mv(Then), mv(Else));
}

std::unique_ptr<Expression> Parser::ParseForExpression() {
  GetNextToken();  // For key word lai khane
  if (mCurrentToken != Token::Identifier) {
    LogError("Expected Identifier after 'for' keyword");
    return nullptr;
  }
  std::string IdName = GetTokenizedIdentifier();
  GetNextToken();  // aba tyo identifier lai khane

  if (mCurrentToken != '=') {
    LogError("Expected '=' after loop variable");
    return nullptr;
  }
  GetNextToken();  // eat =

  auto Start = Parser::ParseExpression();
  if (not Start) return nullptr;
  if (mCurrentToken != ',') {
    LogError("Expected ',' after Start Value");
    return nullptr;
  }
  GetNextToken();

  auto End = Parser::ParseExpression();
  if (not End) return nullptr;
  // aba yo step chae optional xa

  up<Expression> Step;
  if (mCurrentToken == ',') {
    GetNextToken();  // comma khane
    Step = Parser::ParseExpression();
    if (not Step) return nullptr;
  }

  if (mCurrentToken != Token::Do) {
    LogError("Expected do after this");
    return nullptr;
  }
  GetNextToken();  // do khane

  auto Body = Parser::ParseExpression();
  if (not Body) {
    return nullptr;
  }

  return mu<Expr::For>(IdName, mv(Start), mv(End), mv(Step), mv(Body));
}

void Parser::HandleDefinition() {
  if (ParseDefinition()) {
    std::cout << "Parsed a Function Definition\n";
  } else {
    // Definition haina raixa
    GetNextToken();
  }
}

void Parser::HandleExtern() {
  if (ParseExtern()) {
    std::cout << "Parsed an Extern\n";
  } else {
    GetNextToken();
  }
}

void Parser::HandleTopLevelExpression() {
  if (ParseTopLevelExpression()) {
    std::cout << "Parsed a top level expression \n";
  } else {
    GetNextToken();
  }
}
