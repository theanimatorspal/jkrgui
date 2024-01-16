#pragma once
#include "../../LuaBirali/lua.hpp"
#include "../include.hpp"
#include "operators.hpp"
#include "token.hpp"

class Lexer {
public:
    Lexer()
        : mC(mDummyStr)
    {
    }

    Lexer(std::istream& inCodeStream)
        : mC(inCodeStream)
    {
        ls.mz = &mC;
        ls.mt.token = (Token)0;
        ls.mcurrent = (Token)mC.get();
        ls.mlookahead.token = Token::Eos;
        ls.mlinenumber = 1;
        ls.mlastline = 1;
    }

    Token Next()
    {
        ls.mlastline = ls.mlinenumber;
        if (ls.mlookahead != Token::Eos) {
            ls.mt = ls.mlookahead;
            ls.mlookahead = Token::Eos;
        } else {
            ls.mt = ls.Lex(ls.mt.s);
            return ls.mt.token;
        }
    }

    Token LookAhead()
    {
        assert(ls.mlookahead.token == Token::Eos);
        ls.mlookahead = ls.Lex(ls.mlookahead.s);
        return ls.mlookahead.token;
    }

private:
    std::stringstream mDummyStr;
protected:
    LexerState ls;
    std::istream& mC;
};
