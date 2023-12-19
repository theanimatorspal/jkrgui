#pragma once
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <iostream>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

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
        Number = -6,
        /* conditionals */
        If = -7,
        Then = -8,
        Else = -9,
        /* loop */
        For = -10,
        Do = -11,
        /* Opertor types  */
		BinaryOP = -12,
		UnaryOP = -13
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
