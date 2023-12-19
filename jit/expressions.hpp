#pragma once
#include <algorithm>
#include <type_traits>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
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
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>

#include <memory>
#include <string>
#include <vector>
#ifndef GETTER
#define GETTER inline auto
#endif
#define mu std::make_unique
#define up std::unique_ptr
#define mv std::move

class IrGenerator;

template <typename T>
using v = std::vector<T>;

using s = std::string;
using sv = std::string_view;
namespace l = llvm;

class AnonymousExpressionNameGenerator {
 public:
  std::string New() {
    return std::string(ANONYMOUS_EXPRESSION_NAME)
        .append(std::to_string(mCurrent++));
  }
  std::string Get() {
    return std::string(ANONYMOUS_EXPRESSION_NAME)
        .append(std::to_string(mCurrent - 1));
  }
  bool Is(const std::string_view inView) {
    return inView.substr(0, ANONYMOUS_EXPRESSION_NAME.size()) ==
           ANONYMOUS_EXPRESSION_NAME;
  }

 private:
  static constexpr std::string_view ANONYMOUS_EXPRESSION_NAME =
      "__anon_expression__";
  int mCurrent = 0;
};

namespace Expr {

struct Expression {
  virtual ~Expression() = default;
  // this method says to emit IR for that AST node along with all the things it
  // depend on, and they all return an llvm value object This is the class that
  // is used to represent Static Single Assignment (SSA) SSA = requires each
  // variable to be assigned exactly once and defined before using it There is
  // no way to change a SSA value
  virtual llvm::Value *CodeGen(IrGenerator &inG) = 0;
};

struct Number : Expression {
  Number(double inValue) : mValue(inValue) {}

  virtual llvm::Value *CodeGen(IrGenerator &inG) override;

 private:
  double mValue;
};

struct Variable : Expression {
  Variable(const sv inName) : mName(inName) {}

  virtual llvm::Value *CodeGen(IrGenerator &inG) override;

 private:
  s mName;
};

struct Binary : Expression {
  Binary(char inOp, up<Expression> inLHS, up<Expression> inRHS)
      : mOp(inOp), mLHS(mv(inLHS)), mRHS(mv(inRHS)) {}
  virtual llvm::Value *CodeGen(IrGenerator &inG) override;

 private:
  char mOp;
  up<Expression> mLHS, mRHS;
};

struct Unary : Expression {
  Unary(char inOp, up<Expr::Expression> inOperand)
      : mOp(inOp), mOperand(mv(inOperand)) {}
  virtual llvm::Value *CodeGen(IrGenerator &inG) override;

 private:
  char mOp;
  up<Expr::Expression> mOperand;
};

struct Call : Expression {
  Call(const sv inCallee, v<up<Expression>> Args)
      : mCallee(inCallee), mArgs(mv(Args)) {}
  virtual llvm::Value *CodeGen(IrGenerator &inG) override;

 private:
  s mCallee;
  v<up<Expression>> mArgs;
};

struct Prototype : Expression {
  Prototype(const sv inName, v<s> inArgs, bool inIsOperator = false,
            uint32_t inPrecedence = 0)
      : mName(inName),
        mArgs(mv(inArgs)),
        mIsOperator(inIsOperator),
        mPrecedence(inPrecedence) {}
  GETTER GetName() const { return mName; }
  virtual llvm::Function *CodeGen(IrGenerator &inG) override;
  GETTER IsBinaryOperator() const { return mIsOperator and mArgs.size() == 2; }
  GETTER IsUnaryOperator() const { return mIsOperator and mArgs.size() == 1; }
  char GetOperatorName() const {
    assert(IsUnaryOperator() or IsBinaryOperator());
    return mName.back();
  }
  GETTER GetBinaryPrecedence() const { return mPrecedence; }

 private:
  s mName;
  v<s> mArgs;
  bool mIsOperator = false;
  uint32_t mPrecedence = 0;
};

struct Function : Expression {
  Function(up<Prototype> inPrototype, up<Expression> inBody)
      : mPrototype(mv(inPrototype)), mBody(mv(inBody)) {}
  virtual llvm::Function *CodeGen(IrGenerator &inG) override;

 private:
  up<Prototype> mPrototype;
  up<Expression> mBody;
};

struct If : Expression {
 public:
  If(up<Expression> inCondition, up<Expression> inThen, up<Expression> inElse)
      : mCondition(mv(inCondition)), mThen(mv(inThen)), mElse(mv(inElse)) {}

 private:
  up<Expression> mCondition, mThen, mElse;
  virtual llvm::Value *CodeGen(IrGenerator &inG) override;
};

struct For : Expression {
 public:
  For(const std::string_view inVariableName, up<Expression> inStart,
      up<Expression> inEnd, up<Expression> inStep, up<Expression> inBody)
      : mVariableName(inVariableName),
        mStart(mv(inStart)),
        mEnd(mv(inEnd)),
        mStep(mv(inStep)),
        mBody(mv(inBody)) {}

  virtual llvm::Value *CodeGen(IrGenerator &inG) override;

 private:
  std::string mVariableName;
  up<Expression> mStart, mEnd, mStep, mBody;
};

}  // namespace Expr
