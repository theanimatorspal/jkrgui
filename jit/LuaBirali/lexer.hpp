#pragma once
#include "include.hpp"
#include "lua.hpp"

class Lexer {
public:
    Lexer(std::istream& inCodeStream)
        : mC(inCodeStream)
    {
        ls.mz = &mC;
    }

    void Next()
    {
        ls.mlastline = ls.mlinenumber;
        if (ls.mlookahead != Token::Eos) {
            ls.mt = ls.mlookahead;
            ls.mlookahead = Token::Eos;
        } else {
            ls.mt = ls.Lex(ls.mlookahead.s);
        }
    }

    int LookAhead()
    {
        assert(ls.mlookahead.token == Token::Eos);
        ls.mlookahead = ls.Lex(ls.mlookahead.s);
        return ls.mlookahead.token;
    }

protected:
    LexerState ls;
    std::istream& mC;
};
