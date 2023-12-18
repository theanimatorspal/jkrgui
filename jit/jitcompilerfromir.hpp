#include "irgenerator.hpp"

class JitCompiler : public IrGenerator
{
public:
    JitCompiler(std::istream &inStream)
        : IrGenerator(inStream)
    {}
    void HandleTopLevelExpression();
    void MainLoop();
    void HandleDefinition();
    void HandleExtern();
    virtual llvm::Function *GetFunction(const std::string_view inName) override;
};
