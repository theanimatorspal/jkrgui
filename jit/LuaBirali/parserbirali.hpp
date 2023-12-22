#pragma once
#include <sstream>

#include "astbirali.hpp"
#include "lexerbirali.hpp"

namespace Birali {
class Parser : public Birali::Lexer {
public:
    Parser(std::istream& inStream)
        : Birali::Lexer(inStream)
    {
        mBinaryOperatorPrecedence['='] = 2;
        mBinaryOperatorPrecedence['<'] = 10;
        mBinaryOperatorPrecedence['+'] = 20;
        mBinaryOperatorPrecedence['-'] = 20;
        mBinaryOperatorPrecedence['*'] = 40;
        mBinaryOperatorPrecedence['/'] = 40;
        GetNextToken();
    }
    ccode GetNextToken()
    {
        mCurrentToken = (Token)Lexer::GetToken();
        while (mCurrentToken >= 0 && std::isspace(mCurrentToken)) {
            mCurrentToken = (Token)Lexer::GetToken();
        }
        return mCurrentToken;
    }
    ccode GetCurrentToken() const { return mCurrentToken; }
    int GetTokenPrecedence();
    std::stringstream mT;
    void PrintAST() { std::cout << mT.rdbuf() << "\n"; }

public:
    template <typename T = Ast::Expression>
    up<T> LogError(const sv inString)
    {
        std::cout << "Error:" << inString << " at ";
        std::cout << "Line:" << GetLineLexed() << '\n';
        return nullptr;
    }

    /* Primary Parsers */
    up<Ast::Expression> ParseNumberExpression();
    up<Ast::Expression> ParseStringLiteralExpression();
    up<Ast::Expression> ParseIdentifierExpression();
    up<Ast::Expression> ParseParentExpression();
    up<Ast::Expression> ParsePrimary()
    {
        switch (mCurrentToken) {
        default:
            return LogError("Unknown Item When Expecting an expression");
        case Token::Number:
            return ParseNumberExpression();
        case StringLiteral:
            return ParseStringLiteralExpression();
        case Token::Identifier:
            return ParseIdentifierExpression();
        case '(':
            return ParseParentExpression();
        }
    }

    /* Expression Parsers */
    void NextExpression()
    {
        mT << "\n";
        GetNextToken();
    }
    up<Ast::Expression> ParseBinaryOperationRHS(int inExpressionPrecedence,
                                                up<Ast::Expression> inLHS);

    up<Ast::Expression> ParseExpression()
    {
        mT << "\nExpression:";
        auto LHS = ParsePrimary();
        if (not LHS) {
            return nullptr;
        }
        return ParseBinaryOperationRHS(0, mv(LHS));
    }

public:
    AnonymousExpressionNameGenerator mAnonymousNameGenerator;

private:
    map<char, int> mBinaryOperatorPrecedence;
    Token mCurrentToken;
};

} // namespace Birali
