#pragma once
#include "../jit/LuaBirali/lua.hpp"

enum BinOpr {
    /* arithmetic operators */
    ADD,
    SUB,
    MUL,
    MOD,
    POW,
    DIV,
    IDIV,
    /* bitwise operators */
    BAND,
    BOR,
    BXOR,
    SHL,
    SHR,
    /* string operator */
    CONCAT,
    /* comparison operators */
    EQ,
    LT,
    LE,
    NE,
    GT,
    GE,
    /* logical operators */
    AND,
    OR,
    NOBINOPR
};

/*
** Priority table for binary operators.
*/
constexpr struct {
    int left; /* left priority for each binary operator */
    int right; /* right priority */
} priority[] = {
    /* ORDER OPR */
    { 10, 10 }, { 10, 10 }, /* '+' '-' */
    { 11, 11 }, { 11, 11 }, /* '*' '%' */
    { 14, 13 }, /* '^' (right associative) */
    { 11, 11 }, { 11, 11 }, /* '/' '//' */
    { 6, 6 }, { 4, 4 }, { 5, 5 }, /* '&' '|' '~' */
    { 7, 7 }, { 7, 7 }, /* '<<' '>>' */
    { 9, 8 }, /* '..' (right associative) */
    { 3, 3 }, { 3, 3 }, { 3, 3 }, /* ==, <, <= */
    { 3, 3 }, { 3, 3 }, { 3, 3 }, /* ~=, >, >= */
    { 2, 2 }, { 1, 1 } /* and, or */
};

static BinOpr getbinopr(int op)
{
    switch (op) {
    case '+':
        return BinOpr::ADD;
    case '-':
        return BinOpr::SUB;
    case '*':
        return BinOpr::MUL;
    case '%':
        return BinOpr::MOD;
    case '^':
        return BinOpr::POW;
    case '/':
        return BinOpr::DIV;
    case Token::Idiv:
        return BinOpr::IDIV;
    case '&':
        return BinOpr::BAND;
    case '|':
        return BinOpr::BOR;
    case '~':
        return BinOpr::BXOR;
    case Token::Shl:
        return BinOpr::SHL;
    case Token::Shr:
        return BinOpr::SHR;
    case Token::Concat:
        return BinOpr::CONCAT;
    case Token::Ne:
        return BinOpr::NE;
    case Token::Eq:
        return BinOpr::EQ;
    case '<':
        return BinOpr::LT;
    case Token::Le:
        return BinOpr::LE;
    case '>':
        return BinOpr::GT;
    case Token::Ge:
        return BinOpr::GE;
    case Token::And:
        return BinOpr::AND;
    case Token::Or:
        return BinOpr::OR;
    default:
        return BinOpr::NOBINOPR;
    }
}

enum UnOpr {
    OPR_MINUS,
    OPR_BNOT,
    OPR_NOT,
    OPR_LEN,
    OPR_NOUNOPR
};

constexpr int UNARY_PRIORITY = 12;

static UnOpr getunopr(int op)
{
    switch (op) {
    case Token::Not:
        return OPR_NOT;
    case '-':
        return OPR_MINUS;
    case '~':
        return OPR_BNOT;
    case '#':
        return OPR_LEN;
    default:
        return OPR_NOUNOPR;
    }
}

struct LHS_assign {
    LHS_assign* previous;
    ExpDesc v;
};
