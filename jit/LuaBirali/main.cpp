#include <sstream>
#include "jitbirali.hpp"

int main() {
  std::stringstream str;
  str << R"""( 
	local x = 5;
)""";
  Birali::Jit BJit(str);
  BJit.InitializeModuleAndPassManagers();
  BJit.MainLoop();
}
