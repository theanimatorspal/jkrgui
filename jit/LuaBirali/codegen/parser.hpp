#include "../lexer/lexer.hpp"
#include "ast_primitives.hpp"

class Parser : virtual public Lexer {
    const SemanticInfo& GetNumber()
    {
        return ls.mt.s;
    }
    up<Node> Number() { }
};