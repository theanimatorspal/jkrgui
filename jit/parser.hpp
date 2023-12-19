#pragma once
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include "expressions.hpp"
#include "lexer.hpp"
#include <iostream>
#include <map>
#ifdef WIN32
#include <Windows.h>
#ifdef max
#undef max
#endif
#endif

class Parser : public Lexer
{
public:
    Parser(std::istream &inCodeStream)
        : Lexer(inCodeStream)
        , mAnonymousNameGenerator()
    {
        mBinaryOperatorPrecedence['<'] = 10;
        mBinaryOperatorPrecedence['+'] = 20;
        mBinaryOperatorPrecedence['-'] = 20;
        mBinaryOperatorPrecedence['*'] = 40;
        mBinaryOperatorPrecedence['/'] = 40;
        GetNextToken();
    }
    void MainLoop();

    inline int GetNextToken() {
        mCurrentToken = Lexer::GetToken(); 
        while (mCurrentToken >= 0 && std::isspace (mCurrentToken))
        {
            mCurrentToken = Lexer::GetToken();
        }
        return mCurrentToken;
    }
    GETTER GetCurretToken() const { return mCurrentToken; }

private:
    int mCurrentToken;

private:
    std::map<char, int> mBinaryOperatorPrecedence;
    int GetTokenPrecedence()
    {
        if (mCurrentToken >= 0 and not std::isprint(mCurrentToken)) {
            return -1;
        }

        int TokenPrecedence = mBinaryOperatorPrecedence[mCurrentToken];
        if (TokenPrecedence <= 0)
            return -1;
        return TokenPrecedence;
    }

	public:
    GETTER& GetBinaryOperatorPrecedenceMap() {
        return mBinaryOperatorPrecedence;
    }

protected:
    up<Expr::Expression> LogError(const sv inString);
    up<Expr::Prototype> LogErrorPrototype(const sv inString);
    up<Expr::Expression> ParseNumberExpression();
    up<Expr::Expression> ParseExpression();
    up<Expr::Expression> ParseParentExpression();
    up<Expr::Expression> ParseIdentifierExpression();
    up<Expr::Expression> ParsePrimary();
    up<Expr::Expression> ParseBinaryOperationRHS(int inExpressionPrecedence,
                                                 up<Expr::Expression> inLHS);
    up<Expr::Expression> ParseUnary();
    up<Expr::Prototype> ParsePrototype();
    up<Expr::Function> ParseDefinition();
    up<Expr::Function> ParseTopLevelExpression();
    up<Expr::Prototype> ParseExtern();
    up<Expr::Expression> ParseIfExpression();
    up<Expr::Expression> ParseForExpression();
    /* Top Level Parsing */
    void HandleDefinition();
    void HandleExtern();
    void HandleTopLevelExpression();

public:
    AnonymousExpressionNameGenerator mAnonymousNameGenerator;
};

template<class T>
struct ExecAllocator
{
    typedef T value_type;
    ExecAllocator() = default;

    template<class U>
    constexpr ExecAllocator(const ExecAllocator<U> &) noexcept
    {}

    [[nodiscard]] T *allocate(std::size_t n)
    {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
            throw std::bad_array_new_length();

        /* TODO: This is to be made cross-platform */
        auto p = static_cast<T *>(
            VirtualAlloc(nullptr, n * sizeof(T), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
        if (p) {
            report(p, n);
            return p;
        }
        throw std::bad_alloc();
    }

    void deallocate(T *p, std::size_t n) noexcept
    {
        report(p, n, false);

        /* TODO: This is to be made cross-platform */
        VirtualFree(p, n, MEM_RELEASE);
    }

private:
    void report(T *p, std::size_t n, bool alloc = true) const
    {
        std::cout << (alloc ? "Alloc: " : "Dealloc : ") << sizeof(T) * n << " bytes at " << std::hex
                  << std::showbase << reinterpret_cast<void *>(p) << std::dec << '\n';
    }
};
