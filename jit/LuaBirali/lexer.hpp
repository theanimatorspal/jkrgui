#pragma once
#include "include.hpp"
#include "lua.hpp"

class Lexer {
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

protected:
    LexerState ls;
    std::istream& mC;
};
