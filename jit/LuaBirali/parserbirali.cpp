#include "parserbirali.hpp"

#include "astbirali.hpp"
#include "confbirali.hpp"

int Birali::Parser::GetTokenPrecedence()
{
    if (mCurrentToken >= 0 and not std::isprint(mCurrentToken)) {
        return -1;
    }

    int TokenPrecedence = mBinaryOperatorPrecedence[mCurrentToken];
    if (TokenPrecedence <= 0)
        return -1;
    return TokenPrecedence;
}

std::unique_ptr<Birali::Ast::Expression> Birali::Parser::ParseNumberExpression()
{
    auto Value = mu<Ast::Number>(Ast::Type::Double, GetTokenizedNumber());
    mT << "Number: " << GetTokenizedNumber() << ", ";
    GetNextToken();
    return mv(Value);
}

std::unique_ptr<Birali::Ast::Expression> Birali::Parser::ParseStringLiteralExpression()
{
    auto Value = mu<Ast::StringLiteral>(Ast::Type::String, GetTokenizedStringLiteral());
    mT << "StringLiteral: " << GetTokenizedStringLiteral() << ", ";
    GetNextToken();
    return mv(Value);
}

std::unique_ptr<Birali::Ast::Expression> Birali::Parser::ParseIdentifierExpression()
{
    s Name = Lexer::GetTokenizedIdentifier();
    GetNextToken();
    if (mCurrentToken != '(') {
        mT << "Identifier: " << GetTokenizedIdentifier() << ", ";
        return mu<Ast::Variable>(Ast::Type::Unknown, Name);
    }
    GetNextToken();
    v<up<Ast::Expression>> Args;
    if (mCurrentToken != ')') {
        while (true) {
            if (auto Arg = ParseExpression()) {
                Args.push_back(mv(Arg));
            } else {
                return nullptr;
            }
            if (mCurrentToken == ')')
                break;
            if (mCurrentToken != ',')
                return LogError("Expected ')' or ',' in argument list ");
            GetNextToken();
        }
    }
    GetNextToken();
    mT << "Call: " << GetTokenizedIdentifier() << ", ";
    return mu<Ast::Call>(Ast::Type::Unknown, Name, mv(Args));
}

std::unique_ptr<Birali::Ast::Expression> Birali::Parser::ParseParentExpression()
{
    GetNextToken();
    auto V = ParseExpression();
    if (not V) {
        return nullptr;
    }

    if (mCurrentToken != ')') {
        return LogError("Expected ')'");
    }
    GetNextToken();
    return V;
}

std::unique_ptr<Birali::Ast::Expression> Birali::Parser::ParseBinaryOperationRHS(
    int inExpressionPrecedence, std::unique_ptr<Ast::Expression> inLHS)
{
    while (true) {
        int TokenPrecedence = GetTokenPrecedence();
        if (TokenPrecedence < inExpressionPrecedence) {
            return inLHS;
        }

        int BinaryOperator = mCurrentToken;
        GetNextToken();
        mT << "BinaryOperator: " << (char) BinaryOperator << ", ";
        auto RHS = ParsePrimary();
        if (not RHS) {
            return nullptr;
        }
        int NextPrecedence = GetTokenPrecedence();
        if (TokenPrecedence < NextPrecedence) {
            RHS = ParseBinaryOperationRHS(TokenPrecedence + 1, mv(RHS));
            if (not RHS) {
                return nullptr;
            }
        }
        inLHS = mu<Ast::Binary>(inLHS->mType, BinaryOperator, mv(inLHS), mv(RHS));
    }
}
