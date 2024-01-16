#include "ast_primitives.hpp"

llvm::Value* VariableAST::codegen(Ir_base& ir)
{
    llvm::AllocaInst* A;
    if (not A)
    {
        (SyntaxError << "Unknown Variable Name")++;
    }
    return ir.getbuilder().CreateLoad(A->getAllocatedType(), A, mVarDesc.name.c_str());
}

llvm::Value* NumberAST::codegen(Ir_base& ir)
{
    return nullptr;
}
