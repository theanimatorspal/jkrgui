#pragma once
#include "lexer.hpp"
#include "expressions.hpp"

class Parser : public Lexer
{
public:
    Parser(std::istream &inCodeStream)
        : Lexer(inCodeStream)
    {
        mBinaryOperatorPrecedence['<'] = 10;
        mBinaryOperatorPrecedence['+'] = 20;
        mBinaryOperatorPrecedence['-'] = 20;
        mBinaryOperatorPrecedence['*'] = 40;
        GetNextToken();
    }
    void MainLoop();

private:
    int mCurrentToken;
    inline int GetNextToken() { return mCurrentToken = Lexer::GetToken(); }

private:
    std::map<char, int> mBinaryOperatorPrecedence;
    int GetTokenPrecedence()
    {
        if (not isascii(mCurrentToken)) {
            return -1;
        }

        int TokenPrecedence = mBinaryOperatorPrecedence[mCurrentToken];
        if (TokenPrecedence <= 0)
            return -1;
        return TokenPrecedence;
    }

protected:
    up<Expression> LogError(const sv inString);
    up<Prototype> LogErrorPrototype(const sv inString);
    up<Expression> ParseNumberExpression();
    up<Expression> ParseExpression();
    up<Expression> ParseParentExpression();
    up<Expression> ParseIdentifierExpression();
    up<Expression> ParsePrimary();
    up<Expression> ParseBinaryOperationRHS(int inExpressionPrecedence, up<Expression> inLHS);
    up<Prototype> ParsePrototype();
    up<Function> ParseDefinition();
    up<Function> ParseTopLevelExpression();
    up<Prototype> ParseExtern();
    /* Top Level Parsing */
    void HandleDefinition();
    void HandleExtern();
    void HandleTopLevelExpression();
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
