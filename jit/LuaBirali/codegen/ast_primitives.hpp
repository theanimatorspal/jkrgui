#include "ir_base.hpp"

class Node {
public:
    virtual ~Node() = default;
    virtual llvm::Value* codegen(Ir_base& ir) = 0;
};

class NumberAST : public Node {
public:
    NumberAST(ExpDesc inD)
        : mDesc(inD)
    {
    }

private:
    ExpDesc mDesc;
    virtual llvm::Value* codegen(Ir_base& ir);
};

class VariableAST : public Node {
    VariableAST(sv inName, VarKind inVarKind)
    {
        mName = inName;
        mVarDesc = VarDesc { .kind = inVarKind, .name = s(inName) };
    }

private:
    VarDesc mVarDesc;
    s mName;
    virtual llvm::Value* codegen(Ir_base& ir) override;
};
