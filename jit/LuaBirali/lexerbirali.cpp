#include "lexerbirali.hpp"

using namespace Birali;

ccode Lexer::GetToken() {
  while (IsSpaceExt(mLastCharacter)) {
    mLastCharacter = mCodeStream.get();
  }

  while (IsAlphabet(mLastCharacter)) {
    mTokenizedIdentifier = mLastCharacter;

    while (IsAlphaNumeric(mLastCharacter = mCodeStream.get())) {
      mTokenizedIdentifier += mLastCharacter;
    }

    if (mKeywordMap.contains(mTokenizedIdentifier)) {
      return mKeywordMap[mTokenizedIdentifier];
    } else {
      return Token::Identifier; 
    }
  }


  while (IsDigit (mLastCharacter))
  {
      s NumberString;
      do {
          NumberString += mLastCharacter;
          mLastCharacter = mCodeStream.get();
      } while (IsDigit(mLastCharacter) || mLastCharacter == '.');
    mTokenizedNumber = std::strtod(NumberString.c_str(), 0);
    return Token::Number;
  }

  if (mLastCharacter == '-') {
    auto NewLastCharacter = mCodeStream.peek();
    if (NewLastCharacter == '-') {
      do {
        mLastCharacter = mCodeStream.get();
      } while (not IsEndOrNewLine(mLastCharacter));
      if (mLastCharacter != EOF) {
        return GetToken();  // jaba samma kei return gardaina
      }
    }
  }

  if (mLastCharacter == '"') {
    mTokenizedStringLiteral = "";
    while (IsPrint(mLastCharacter = mCodeStream.get()) and mLastCharacter != '"') {
      mTokenizedStringLiteral += mLastCharacter;
    }
    mLastCharacter = mCodeStream.get();
     return Token::StringLiteral;
  }

  if (mLastCharacter == EOF) return Token::Eof;

  if (mLastCharacter == '\n')
      mLineLexed++;

  int ThisCharacter = mLastCharacter;
  mLastCharacter = mCodeStream.get();

  return (Token)ThisCharacter;
           
}
