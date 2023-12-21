#include <sstream>
#include "parserbirali.hpp"

int main() {
  std::stringstream str;
  str << R"""( 

function definition ( inX ,
 inY ) 

	local x = x + 5

	local y = y + 7


	return Result 

end
 4 + 5

)""";
  Birali::Parser Parser(str);
  Parser.GetNextToken();
  Parser.ParseFunctionDefinition();
  Parser.PrintAST();
}
