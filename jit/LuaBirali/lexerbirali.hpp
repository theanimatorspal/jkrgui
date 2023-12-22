#pragma once
#include "confbirali.hpp"

namespace Birali {

class Lexer {
 public:
  Lexer(std::istream &inCodeStream) : mCodeStream(inCodeStream) {}
  GETTER GetLineLexed() const { return mLineLexed; }
  GETTER GetTokenizedNumber() const { return mTokenizedNumber; }
  GETTER GetTokenizedIdentifier() const { return mTokenizedIdentifier; }
  GETTER GetTokenizedStringLiteral() const { return mTokenizedStringLiteral; }
  ccode GetToken();

 private:
  uint32_t mLineLexed = 0;
  std::istream &mCodeStream;
  s mTokenizedIdentifier;
  double mTokenizedNumber;
  s mTokenizedStringLiteral;
  ccode mLastCharacter = ' ';

 public:
  enum Token : ccode {
    Local = INT_MIN,
    Eof,
    End,

    Function,
    StringConcat,
    Return,

    If,
    Then,
    ElseIf,
    Else,

    Do,
    In,
    Repeat,

    Identifier,
    StringLiteral,
    Number,

  };

  umap<s, Token> mKeywordMap = {
      {"local", Token::Local},   {"function", Token::Function},
      {"return", Token::Return}, {"if", Token::If},
      {"then", Token::Then},     {"else", Token::Else},
      {"elseif", Token::ElseIf}, {"do", Token::Do},
      {"in", Token::In},         {"repeat", Token::Repeat},
      {"end", Token::End}};
};

}  // namespace Birali
