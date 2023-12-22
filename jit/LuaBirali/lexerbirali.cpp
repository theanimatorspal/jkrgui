#include "lexerbirali.hpp"

using namespace Birali;

ccode Lexer::GetToken() {

  /* Handle Spaces */
  {
    while (IsSpace(mLastCharacter)) {
      mLastCharacter = mCodeStream.get();
    }
  }

  /* Handle Identifier or keyword*/
  {
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
  }

  /* Handle Numbers */
  {
    while (IsDigit(mLastCharacter)) {
      s NumberString;
      do {
        NumberString += mLastCharacter;
        mLastCharacter = mCodeStream.get();
      } while (IsDigit(mLastCharacter) || mLastCharacter == '.');
      mTokenizedNumber = std::strtod(NumberString.c_str(), 0);
      return Token::Number;
    }
  }

  /* Handle Comments */
  {
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
  }

  /* Handle String Literal */
  {
    if (mLastCharacter == '"') {
      mTokenizedStringLiteral = "";
      while (IsPrint(mLastCharacter = mCodeStream.get()) and
             mLastCharacter != '"') {
        mTokenizedStringLiteral += mLastCharacter;
      }
      mLastCharacter = mCodeStream.get();
      return Token::StringLiteral;
    }
  }

  /* Handle EOF / new line */
  {
    if (mLastCharacter == EOF)
      return Token::Eof;

    if (mLastCharacter == '\n')
      mLineLexed++;
  }

  int ThisCharacter = mLastCharacter;
  mLastCharacter = mCodeStream.get();

  return (Token)ThisCharacter;
}
