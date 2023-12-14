#pragma once
#include "lexer.hpp"

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
    }
    void MainLoop();

private:
    struct Expression
    {
        virtual ~Expression() = default;
    };

    struct Number : Expression
    {
        Number(double inValue)
            : mValue(inValue)
        {}
        double mValue;
    };

    struct Variable : Expression
    {
        Variable(const sv inName)
            : mName(inName)
        {}
        s mName;
    };

    struct Binary : Expression
    {
        Binary(char inOp, up<Expression> inLHS, up<Expression> inRHS)
            : mOp(inOp)
            , mLHS(mv(inLHS))
            , mRHS(mv(inRHS))
        {}
        char mOp;
        up<Expression> mLHS, mRHS;
    };

    struct Call : Expression
    {
        Call(const sv inCallee, v<up<Expression>> Args)
            : mCallee(inCallee)
            , mArgs(mv(Args))
        {}
        s mCallee;
        v<up<Expression>> mArgs;
    };

    struct Prototype : Expression
    {
        Prototype(const sv inName, v<s> inArgs)
            : mName(inName)
            , mArgs(mv(inArgs))
        {}
        GETTER GetName() const { return mName; }
        s mName;
        v<s> mArgs;
    };

    struct Function : Expression
    {
        Function(up<Prototype> inPrototype, up<Expression> inBody)
            : mPrototype(mv(inPrototype))
            , mBody(mv(inBody))
        {}
        up<Prototype> mPrototype;
        up<Expression> mBody;
    };

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
