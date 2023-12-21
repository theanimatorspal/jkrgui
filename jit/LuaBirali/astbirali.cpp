#include "astbirali.hpp"
#include "irbirali.hpp"

using namespace Birali;

llvm::Value *Ast::Number::CodeGen(IrBirali &inG) {
  return llvm::ConstantFP::get(inG.GetContext(), llvm::APFloat(mNumberValue));
}

llvm::Value *Birali::Ast::Variable::CodeGen(IrBirali &inG) {
  llvm::AllocaInst *A = inG.GetNamedValues()[mName];
  if (not A)
    inG.LogErrorV("Unknown Variable Name");
  return inG.GetIRBuilder().CreateLoad(A->getAllocatedType(), A, mName.c_str());
}

llvm::Value *Birali::Ast::Binary::CodeGen(IrBirali &inG) {
  if (mOp == '=') {
    Ast::Variable *LHSE = static_cast<Ast::Variable *>(mLHS.get());
    if (not LHSE)
      return inG.LogErrorV("destination of '=' must be a variable");

    llvm::Value *Val = mRHS->CodeGen(inG);
    if (not Val)
      return nullptr;

    llvm::Value *Variable = inG.GetNamedValues()[std::string(LHSE->GetName())];
    if (not Variable)
      return inG.LogErrorV("Unknown Variable Name");

    inG.GetIRBuilder().CreateStore(Val, Variable);
  }
  llvm::Value *L = mLHS->CodeGen(inG);
  llvm::Value *R = mRHS->CodeGen(inG);
  if (not L or not R)
    return nullptr;

  switch (mOp) {
  case '+':
    return inG.GetIRBuilder().CreateFAdd(L, R, "addtmp");
  case '-':
    return inG.GetIRBuilder().CreateFSub(L, R, "subtmp");
  case '*':
    return inG.GetIRBuilder().CreateFMul(L, R, "multmp");
  case '/':
    return inG.GetIRBuilder().CreateFDiv(L, R, "divtmp");
  default:
    return inG.LogError("Unknown Operand");
  }
}

llvm::Value *Birali::Ast::Call::CodeGen(IrBirali &inG) {
  llvm::Function *CalleeF = inG.GetFunction(mCallee);
  if (not CalleeF)
    return inG.LogError("Unknown Function Referenced\n");
  if (CalleeF->arg_size() != mArgs.size())
    return inG.LogErrorV("Incorrect Number Of arguments passed\n");
  v<llvm::Value *> ArgsV;
  for (uint32_t i = 0, e = mArgs.size(); i != e; ++i) {
    ArgsV.push_back(mArgs[i]->CodeGen(inG));
    if (not ArgsV.back())
      return nullptr;
  }
  return inG.GetIRBuilder().CreateCall(CalleeF, ArgsV, "calltmp");
}

llvm::Function *Birali::Ast::Prototype::CodeGen(IrBirali &inG) {
  v<llvm::Type *> Doubles(mArgs.size(),
                          llvm::Type::getDoubleTy(inG.GetContext()));
  llvm::FunctionType *FT = llvm::FunctionType::get(
      llvm::Type::getDoubleTy(inG.GetContext()), Doubles, false);
  llvm::Function *F = llvm::Function::Create(
      FT, llvm::GlobalValue::ExternalLinkage, mName, inG.GetModule());
  uint32_t Idx = 0;
  for (auto &Arg : F->args())
    Arg.setName(mArgs[Idx++]);
  return F;
}

llvm::Function *Birali::Ast::Function::CodeGen(IrBirali &inG) {
  auto Itrfuncpos = inG.GetFunctionPrototypeMap().find(mPrototype->GetName());
  bool IsFunctionAlreadyPresent =
      Itrfuncpos != inG.GetFunctionPrototypeMap().end();
  bool IsAnonymous =
      inG.Parser::mAnonymousNameGenerator.Is(mPrototype->GetName());

  if (IsFunctionAlreadyPresent) {
    if (not IsAnonymous)
      return (llvm::Function *)inG.LogErrorV(
          "Redefinition of function is illegal");
  }

  auto &P = *mPrototype;
  inG.GetFunctionPrototypeMap()[mPrototype->GetName()] = mv(mPrototype);
  llvm::Function *TheFunction = inG.GetFunction(P.GetName());
  if (not TheFunction)
    return nullptr;

  llvm::BasicBlock *BB =
      llvm::BasicBlock::Create(inG.GetContext(), "entry", TheFunction);
  inG.GetIRBuilder().SetInsertPoint(BB);
  inG.GetNamedValues().clear();
  for (auto &Arg : TheFunction->args()) {
    llvm::AllocaInst *Alloca =
        inG.CreateEntryBlockAlloca(TheFunction, std::string(Arg.getName()));
    inG.GetIRBuilder().CreateStore(&Arg, Alloca);
    inG.GetNamedValues()[std::string(Arg.getName())] = Alloca;
  }

  if (llvm::Value *RetVal = mBody->CodeGen(inG)) {
    inG.GetIRBuilder().CreateRet(RetVal);
    llvm::verifyFunction(*TheFunction);
    inG.GetFunctionPassManager().run(*TheFunction,
                                     inG.GetFunctionAnalysisManager());
    return TheFunction;
  }

  TheFunction->eraseFromParent();
  return nullptr;
}

llvm::Value *Birali::Ast::StringLiteral::CodeGen(IrBirali &inG) {
  return nullptr;
}

llvm::Value *Birali::Ast::LocalVariable::CodeGen(IrBirali &inG) {
  return nullptr;
}

llvm::Value *Birali::Ast::Return::CodeGen(IrBirali &inG) { return nullptr; }

llvm::Value *Birali::Ast::If::CodeGen(IrBirali &inG) { return nullptr; }
