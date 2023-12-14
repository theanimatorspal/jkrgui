#include "parser.hpp"
#include "lexer.hpp"

void Parser::MainLoop()
{
    bool run = true;
	std::cout << "Ready :: \n";
    while (run) {
        switch (mCurrentToken) {
        case Token::Eof:
            run = false;
            break;
        case ';':
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

std::unique_ptr<Parser::Expression> Parser::LogError(const sv inString)
{
    std::cout << "Error : " << inString << '\n';
    return nullptr;
}

std::unique_ptr<Parser::Prototype> Parser::LogErrorPrototype(const sv inString)
{
    LogError(inString);
    return nullptr;
}

std::unique_ptr<Parser::Expression> Parser::ParseNumberExpression()
{
    auto Result = mu<Number>(
        Lexer::GetTokenizedNumber()); // GetNextToken garnu vanda agaadi garnu parxa kinaki,
    //tei number vetera nai aako hunxa yo function ma chae
    GetNextToken();
    return mv(Result);
}

// expression ::= primary binoperation
std::unique_ptr<Parser::Expression> Parser::ParseExpression()
{
    auto LHS = ParsePrimary();
    if (not LHS)
        return nullptr;
    return ParseBinaryOperationRHS(0, std::move(LHS));
}

std::unique_ptr<Parser::Expression> Parser::ParseParentExpression()
{
    GetNextToken(); // '(' vetera yaa aako hunxa, so tellai khanu paro
    auto V = ParseExpression();
    if (not V)
        return nullptr;

    if (mCurrentToken != ')')
        return LogError("Expected ')'");
    GetNextToken(); // aba ( paxi ) khojera tellai khana paro
    return V;
}

std::unique_ptr<Parser::Expression> Parser::ParseIdentifierExpression()
{
    std::string IdName = Lexer::GetTokenizedIdentifier();
    GetNextToken(); // Identifier lai vetera nai yaa aako hunxa, so tellai khanu paro (xodnu paro)
    if (mCurrentToken != '(')
        return mu<Variable>(IdName); // bracket open vako xaina, so function call hoina vane chae
    // Tara aba function call ho vane chae
    GetNextToken(); // Identifier paxi ko '(' lai khana paro, mathi ko GetNextToken le chae '(' lai Current banaidinxa
    v<up<Expression>> Args;
    if (mCurrentToken != ')') {
        while (true) {
            if (auto Arg = ParseExpression()) {
                Args.push_back(mv(Arg));
            } else {
                return nullptr;
            }
            if (mCurrentToken == ')')
                break;
            if (mCurrentToken
                != ',') // Comma aayo vane pani true vairakhxa, matlab ) naaaunjel samma true nai hunxa
                return LogError("Expected ')' or ',' in argument list ");
            GetNextToken();
        }
    }
    // Aba chae last wala ) lai pani khana paro
    GetNextToken();
    return mu<Call>(IdName, mv(Args));
}

std::unique_ptr<Parser::Expression> Parser::ParsePrimary()
{
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
    }
}

std::unique_ptr<Parser::Expression> Parser::ParseBinaryOperationRHS(
    int inExpressionPrecedence, std::unique_ptr<Expression> inLHS)
{
    while (true) {
        int TokenPrecedence = GetTokenPrecedence();
        if (TokenPrecedence
            < inExpressionPrecedence) // inexpressionprecedecne = 0, so Token precedence negative xa vane
            return inLHS; // esto case ma rhs nai xaina so, lhs j xa tei return garne (yo break condition ho,
        // jaba samma yo hunna taba samma loop chalairakhne

        // aba mathi ko maa kaam vaena vane yo binary operation ho
        int BinaryOperator
            = mCurrentToken; // GetTokenPrecedence() le kaam garyo matlab, currentToken ma BinaryOperator xa
        GetNextToken(); // tyo binary operator lai khana paro, aba RHS xa mCurrentToken ma

        auto RHS = ParsePrimary(); // aba RHS ko ni primary parse (identifier sidentifier garne)
        if (not RHS)
            return nullptr;
        // mathi ko ParsePrimary le aba Token ma Operator gako huna sakxa
        int NextPrecedence = GetTokenPrecedence();
        if (TokenPrecedence < NextPrecedence) {
            RHS = ParseBinaryOperationRHS(TokenPrecedence + 1,
                                          mv(RHS)); // a+ b *c esto vayo vane a+ aba ahile ko RHS lai
            // LHS banaune ani feri esaima pathaune so suru ma b * c le kaam garos
            if (not RHS)
                return nullptr;
        }
        inLHS = mu<Binary>(BinaryOperator, mv(inLHS), mv(RHS));
    }
}

std::unique_ptr<Parser::Prototype> Parser::ParsePrototype()
{
    if (mCurrentToken != Token::Identifier)
        return LogErrorPrototype("Expected Function name in Prototype");

    s FunctionName = GetTokenizedIdentifier();
    GetNextToken();
    if (mCurrentToken != '(')
        return LogErrorPrototype("Expected '(' in Prototype");

    v<s> ArgNames;
    while (GetNextToken() == Token::Identifier) {
        ArgNames.push_back(GetTokenizedIdentifier());
    }
    if (mCurrentToken != ')')
        return LogErrorPrototype("Expected ')' in Prototype");
    GetNextToken();
    return mu<Prototype>(FunctionName, mv(ArgNames));
}

std::unique_ptr<Parser::Function> Parser::ParseDefinition()
{
    GetNextToken(); // "function" keyword lai khane
    auto Proto = ParsePrototype();
    if (not Proto)
        return nullptr;

    if (auto Body = ParseExpression())
        return mu<Function>(mv(Proto), mv(Body));
    return nullptr;
}

std::unique_ptr<Parser::Function> Parser::ParseTopLevelExpression()
{
    if (auto E = ParseExpression()) {
        // aba euta anonymouse prototype banaidine
        auto Proto = mu<Prototype>("__anon_expression__", v<s>()); // kunai argument navako
        return mu<Function>(mv(Proto), mv(E));
    }
    return nullptr;
}

std::unique_ptr<Parser::Prototype> Parser::ParseExtern()
{
    GetNextToken(); // external keyword lai khaidine
    return ParsePrototype();
}

void Parser::HandleDefinition()
{
    if (ParseDefinition()) {
        std::cout << "Parsed a Function Definition\n";
    } else {
        // Definition haina raixa
        GetNextToken();
    }
}

void Parser::HandleExtern()
{
    if (ParseExtern()) {
        std::cout << "Parsed an Extern\n";
    } else {
        GetNextToken();
    }
}

void Parser::HandleTopLevelExpression()
{
    if (ParseTopLevelExpression()) {
        std::cout << "Parsed a top level expression \n";
    } else {
        GetNextToken();
    }
}
