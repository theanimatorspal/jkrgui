#pragma once
#include "include.hpp"
#include <cinttypes>
#include <climits>
constexpr int8_t first_reserved = INT8_MAX + 1;
enum Token : int8_t {
    And = first_reserved,
    Break,
    Do,
    Else,
    Elseif,
    End,
    False,
    For,
    Function,
    Goto,
    If,
    In,
    Local,
    Nil,
    Not,
    Or,
    Repeat,
    Return,
    Then,
    True,
    Until,
    While,
    /* other terminal symbols */ Idiv,
    Concat,
    Dots,
    Eq,
    Ge,
    Le,
    Ne,
    Shl,
    Shr,
    Dbcolon,
    Eos,
    Flt,
    Int,
    Name,
    String
};

using SemanticInfo = var<Number, Integer, s>;

struct TokenSematic {
    int token;
    SemanticInfo s;
    std::strong_ordering operator<=>(int inToken)
    {
        return inToken <=> token;
    }
    bool operator!=(int inToken)
    {
        return inToken != token;
    }
    void operator=(int inToken)
    {
        token = inToken;
    }
    template <typename T>
    T Get()
    {
        return get<T>(s);
    }
};

static sv const TokenStrings[] = {
    "and", "break", "do", "else", "elseif", "end", "false", "for", "function", "goto",
    "if", "in", "local", "nil", "not", "or", "repeat", "return", "then", "true", "until", "while",
    "//", "..", "...", "==", ">=", "<=", "~=", "<<", ">>", "::", "<eof>", "<number>", "<integer>", "<name>", "<string>"
};

constexpr s TokenToString(int inToken)
{
    if (inToken < first_reserved) {
        if (std::isprint(inToken)) {
            return s().append("'").append(std::to_string((char)inToken)).append("'");
        } else {
            return s().append("'<").append(std::to_string((char)inToken)).append(">'");
        }
    }
    return s(TokenStrings[inToken - first_reserved]);
}

struct gse {
    constexpr gse(s instr)
        : mstr(mv(instr))
    {
    }
    gse& operator()()
    {
        std::cout << mstr;
        return *this;
    }
    gse& operator<<(sv inString)
    {
        std::cout << inString;
        return *this;
    }
    gse& operator<<(char inC)
    {
        std::cout << inC;
        return *this;
    }
    gse& operator++()
    {
        std::cout << '\n';
        return *this;
    }

private:
    s mstr;
};

static gse SyntaxError("SyntaxError:");
static gse LexError("LexError:");

struct BlockState {
};

struct FunctionState {
};

struct LexerState {
    int mcurrent;
    int mlinenumber;
    int mlastline;
    TokenSematic mt;
    TokenSematic mlookahead;
    s mbuff;
    std::istream* mz;

    void next() { mcurrent = mz->get(); }
    void save(int c) { mbuff.push_back(c); }
    void save_and_next()
    {
        save(mcurrent);
        next();
    }

    int check_next(int c)
    {
        if (mcurrent == c) {
            next();
            return 1;
        } else
            return 0;
    }

    int check_next(sv set)
    {
        assert(set[2] == '\0');
        if (mcurrent == set[0] or mcurrent == set[1]) {
            next();
            return 1;
        } else
            return 0;
    }

    int read_numeral(TokenSematic& inseminfo)
    {
        sv expo = "Ee";
        int first = mcurrent;
        assert(std::isdigit(mcurrent));
        save_and_next();
        if (first == 0 and check_next("xX")) {
            expo = "Pp";
        }
        while (true) {
            if (check_next(expo)) {
                check_next("-+");
            } else if (std::isdigit(mcurrent) or mcurrent == '.') {
                save_and_next();
            } else {
                break;
            }
        }
        if (std::isalpha(mcurrent)) {
            save_and_next();
        }
        save('\0');
        Number num = std::strtod(mbuff.c_str(), nullptr);
        Number wholepart;
        Number fractionalpart = std::modf(num, &wholepart);
        if (fractionalpart == 0) // is integer
        {
            inseminfo = (int)wholepart;
            return Token::Int;
        } else {
            inseminfo = num;
            return Token::Flt;
        }
    }

    size_t skip_sep ( )
    {

    }

    int Lex(optref<SemanticInfo> inS)
    {
        while (true) {
            mbuff.clear();
            switch (mcurrent) {
            }
        }
    }
};

struct ParserState {
    v<s> localvars;
    FunctionState* fs;
};
