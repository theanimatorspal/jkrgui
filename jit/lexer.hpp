#pragma once
#include <iostream>
#include <istream>
#include <map>
#include <string>
#include <vector>
#define GETTER inline auto
#define mu std::make_unique
#define up std::unique_ptr
#define mv std::move
template<typename T>
using v = std::vector<T>;
using s = std::string;
using sv = std::string_view;

class Lexer
{
public:
    Lexer(std::istream &inCodeStream)
        : mCodeStream(inCodeStream)
    {}
    int GetToken();

private:
    std::istream &mCodeStream;

protected:
    enum class Type {
        integer,
        floating,
        string_,
    };
    enum Token : int {
        Eof = -1,
        End = -2,
        /*Commands*/
        Function = -3,
        External = -4,
        /*Primary*/
        Identifier = -5,
        Number = -6
    };

protected:
    GETTER GetTokenizedNumber() const { return mTokenizedNumber; }
    GETTER GetTokenizedIdentifier() const { return mTokenizedIdentifier; }

private:
    s mTokenizedIdentifier;
    double mTokenizedNumber;
    char mLastCharacter = ' ';
    inline bool IsEndOrNewLine(char inC) { return inC == EOF or inC == '\r' or inC == '\n'; };
};
