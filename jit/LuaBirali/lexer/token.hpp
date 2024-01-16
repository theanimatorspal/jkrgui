#pragma once
#include "../../LuaBirali/include.hpp"
#include <cinttypes>
#include <climits>
constexpr int8_t first_reserved = INT8_MAX + 1;
constexpr uint32_t UTF8BUFFZ = 8;
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
    Token token;
    SemanticInfo s;
    std::strong_ordering operator<=>(int inToken)
    {
        return inToken <=> token;
    }
    std::strong_ordering operator<=>(Token inToken)
    {
        return inToken <=> token;
    }
    bool operator!=(int inToken)
    {
        return inToken != token;
    }
    void operator=(int inToken)
    {
        token = (Token)inToken;
    }
    template <typename T>
    T Get()
    {
        return get<T>(s);
    }
};

static const v<sv> TokenStrings = {
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
