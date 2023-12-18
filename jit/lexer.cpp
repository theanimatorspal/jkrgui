#include "lexer.hpp"
#include <iostream>

int Lexer::GetToken()
{
    while (std::isspace(mLastCharacter)) {
        mLastCharacter = mCodeStream.get();
    }

    if (std::isalpha(mLastCharacter)) {
        mTokenizedIdentifier = mLastCharacter;

        while (std::isalnum(mLastCharacter = mCodeStream.get())) {
            mTokenizedIdentifier += mLastCharacter;
        }

        if (mTokenizedIdentifier == "function")
            return Token::Function;
        if (mTokenizedIdentifier == "external")
            return Token::External;
        if (mTokenizedIdentifier == "end")
            return Token::End;
        if (mTokenizedIdentifier == "if")
            return Token::If;
        if (mTokenizedIdentifier == "then")
            return Token::Then;
        if (mTokenizedIdentifier == "else")
            return Token::Else;
        if (mTokenizedIdentifier == "for")
            return Token::For;
        if (mTokenizedIdentifier == "do")
            return Token::Do;

        return Token::Identifier;
    }

    if (std::isdigit(mLastCharacter) or mLastCharacter == '.') {
        std::string NumberString;
        do {
            NumberString += mLastCharacter;
            mLastCharacter = mCodeStream.get();
        } while (std::isdigit(mLastCharacter) || mLastCharacter == '.');
        mTokenizedNumber = std::strtod(NumberString.c_str(), 0);
        return Token::Number;
    }

    /* -- is the comment  */
    if (mLastCharacter == '-') {
        auto NewLastCharacter = mCodeStream.peek();
        if (NewLastCharacter == '-') {
            do {
                mLastCharacter = mCodeStream.get();
            } while (not IsEndOrNewLine(mLastCharacter));
            if (mLastCharacter != EOF) {
                return GetToken(); // jaba samma kei return gardaina
            }
        }
    }

    if (mLastCharacter == EOF) {
        return Token::Eof;
    }

    int ThisCharacter = mLastCharacter;
    mLastCharacter = mCodeStream.get();
    return ThisCharacter;
}
