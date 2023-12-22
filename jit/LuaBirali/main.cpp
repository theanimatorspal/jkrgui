#include "parserbirali.hpp"
#include <fstream>
#include <sstream>

int main() {
    std::ifstream file("main.lb");
    std::stringstream str;
    str << file.rdbuf();
    Birali::Parser Parser(str);
    bool run = true;

    while (run) {
        switch (Parser.GetCurrentToken()) {
        case Birali::Parser::Token::Eof:
            run = false;
            break;
        case Birali::Parser::Token::SemiColon:
            Parser.NextExpression();
            break;
        default:
            Parser.ParseExpression();
            break;
        }
    }

    Parser.PrintAST();
}
