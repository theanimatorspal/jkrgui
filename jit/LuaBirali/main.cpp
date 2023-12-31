
/*
UNITY BUILD, lets try one, single CPP with multiple hpps
*/

#include "ir.hpp"
#include "lexer.hpp"
#include "lua.hpp"

int main()
{
    std::stringstream s;
    s << R"""(
   x = 2 * 5 + 3
)""";
    Ir ir(s);
    ir.Next();
    ir.statementlist();
}