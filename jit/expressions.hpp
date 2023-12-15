#pragma once
#include "irgenerator.hpp"
#include "llvm/IR/Value.h"
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#ifndef GETTER
#define GETTER inline auto
#endif
#define mu std::make_unique
#define up std::unique_ptr
#define mv std::move
template<typename T>
using v = std::vector<T>;
using s = std::string;
using sv = std::string_view;
namespace l = llvm;

class IrGenerator;
namespace Expr {

struct Expression
{
	virtual ~Expression() = default;
    // this method says to emit IR for that AST node along with all the things it depend on, and they all return
    // an llvm value object
    // This is the class that is used to represent Static Single Assignment (SSA)
    // SSA = requires each variable to be assigned exactly once and defined before using it
    // There is no way to change a SSA value
    virtual llvm::Value *CodeGen(IrGenerator &inG) = 0;
};

struct Number : Expression
{
	Number(double inValue)
		: mValue(inValue)
	{}

	virtual llvm::Value* CodeGen(IrGenerator& inG) override;

private:
    double mValue;
};

struct Variable : Expression
{
	Variable(const sv inName)
		: mName(inName)
	{}

	virtual llvm::Value* CodeGen(IrGenerator& inG) override;

private:
    s mName;
};

struct Binary : Expression
{
	Binary(char inOp, up<Expression> inLHS, up<Expression> inRHS)
		: mOp(inOp)
		, mLHS(mv(inLHS))
		, mRHS(mv(inRHS))
	{}
    virtual llvm::Value *CodeGen(IrGenerator &inG) override;

private:
    char mOp;
    up<Expression> mLHS, mRHS;
};

struct Call : Expression
{
	Call(const sv inCallee, v<up<Expression>> Args)
		: mCallee(inCallee)
		, mArgs(mv(Args))
	{}
    virtual llvm::Value *CodeGen(IrGenerator &inG) override;

private:
	s mCallee;
	v<up<Expression>> mArgs;
};

struct Prototype : Expression
{
	Prototype(const sv inName, v<s> inArgs)
		: mName(inName)
		, mArgs(mv(inArgs))
	{}
    virtual llvm::Function *CodeGen(IrGenerator &inG) override;

	GETTER GetName() const { return mName; }
private:
	s mName;
	v<s> mArgs;
};

struct Function : Expression
{
	Function(up<Prototype> inPrototype, up<Expression> inBody)
		: mPrototype(mv(inPrototype))
		, mBody(mv(inBody))
	{}
    virtual llvm::Function *CodeGen(IrGenerator &inG) override;

private:
	up<Prototype> mPrototype;
	up<Expression> mBody;
};

} // namespace Expr
