#pragma once
#include "confbirali.hpp"

namespace Birali {
class IrBirali;
class AnonymousExpressionNameGenerator {
public:
  std::string New() {
    return std::string(anon_name).append(std::to_string(mCurrent++));
  }
  std::string Get() {
    return std::string(anon_name).append(std::to_string(mCurrent - 1));
  }
  bool Is(const std::string_view inView) {
    return inView.substr(0, anon_name.size()) == anon_name;
  }

private:
  static constexpr std::string_view anon_name = "__anon_expression__";
  int mCurrent = 0;
};

/* ABSTRACT SYNTAX TREE */

namespace Ast {

enum class Type {
  Integer,
  UnsignedInteger,
  Float,
  Double,
  String,
  Void,
  Unknown
};


struct Expression {
  Expression(Type inType) : mType(inType) {}
  virtual ~Expression() = default;
  virtual llvm::Value *CodeGen(IrBirali &inG) = 0;
  Type mType;
};

struct Number : public Expression {
  Number(Type inType, double inValue)
      : Expression(inType), mNumberValue(inValue) {}
  virtual llvm::Value *CodeGen(IrBirali &inG) override;

private:
  [[maybe_unused]] double mNumberValue;
};

struct StringLiteral : public Expression {
  StringLiteral(Type inType, s inValue) : Expression(inType), mValue(inValue) {}
  virtual llvm::Value *CodeGen(IrBirali &inG) override;

private:
  [[maybe_unused]] s mValue;
};

struct Variable : public Expression {
  Variable(Type inType, const sv inName) : Expression(inType), mName(inName) {}
  sv GetName() const { return mName; }
  virtual llvm::Value *CodeGen(IrBirali &inG) override;

private:
  s mName;
};

struct Binary : public Expression {
  Binary(Type inType, char inOp, up<Expression> inLHS, up<Expression> inRHS)
      : Expression(inType), mOp(inOp), mLHS(mv(inLHS)), mRHS(mv(inRHS)) {}

  virtual llvm::Value *CodeGen(IrBirali &inG) override;

private:
  char mOp;
  up<Expression> mLHS, mRHS;
};

struct LocalVariable : public Expression {
  LocalVariable(Type inType, v<p<s, up<Expression>>> inVarNames, up<Expression> inBody)
      : Expression(inType), mVarNames(mv(inVarNames)), mBody(mv(inBody)) {}
  virtual llvm::Value *CodeGen(IrBirali &inG) override;

private:
  v<p<s, up<Expression>>> mVarNames;
  up<Expression> mBody;
};

struct Call : Expression {
  Call(Type inType, const sv inCallee, v<up<Expression>> Args)
      : Expression(inType), mCallee(inCallee), mArgs(mv(Args)) {}
  virtual llvm::Value *CodeGen(IrBirali &inG) override;

private:
  s mCallee;
  v<up<Expression>> mArgs;
};

struct Prototype : Expression {
  Prototype(Type inReturnType, const sv inName, v<s> inArgs)
      : Expression(inReturnType), mName(inName), mArgs(mv(inArgs)) {}
  GETTER GetName() const { return mName; }
  virtual llvm::Function *CodeGen(IrBirali &inG) override;

private:
  s mName;
  v<s> mArgs;
};

struct Return : Expression {
  Return(Type inType, up<Expression> inValue)
      : Expression(inType), mValue(mv(inValue)) {}

  virtual llvm::Value *CodeGen(IrBirali &inG) override;

private:
  up<Expression> mValue;
};

struct Function : Expression {
  Function(Type inReturnType, up<Prototype> inPrototype, up<Expression> inBody)
      : Expression(inReturnType), mPrototype(mv(inPrototype)),
        mBody(mv(inBody)) {}
  virtual llvm::Function *CodeGen(IrBirali &inG) override;

private:
  up<Prototype> mPrototype;
  up<Expression> mBody;
};

struct If : Expression {
public:
  If(up<Expression> inCondition, up<Expression> inThen, up<Expression> inElse)
      : Expression(Type::Unknown), mCondition(mv(inCondition)),
        mThen(mv(inThen)), mElse(mv(inElse)) {}

  virtual llvm::Value *CodeGen(IrBirali &inG) override;

private:
  up<Expression> mCondition, mThen, mElse;
};

} // namespace Ast
} // namespace Birali
