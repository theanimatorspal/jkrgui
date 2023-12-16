#include "expressions.hpp"
#include "irgenerator.hpp"
#include "llvm/IR/Function.h"
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Constants.h>

llvm::Value *Expr::Number::CodeGen(IrGenerator &inG)
{
    // constant floating point, get arbritray floating point
    return llvm::ConstantFP::get(inG.GetContext(), llvm::APFloat(mValue));
}

llvm::Value *Expr::Variable::CodeGen(IrGenerator &inG)
{
    // look this variable up in the function
    // Assume garne ki variable pahile nai katai emit vaisako
    llvm::Value *V = inG.GetNamedValues()[mName];
    if (not V)
        inG.LogErrorV("Unknown Variable Name");
    return V;
}

llvm::Value *Expr::Binary::CodeGen(IrGenerator &inG)
{
    llvm::Value* L = mLHS->CodeGen(inG);
    llvm::Value* R = mRHS->CodeGen(inG);
    if (not L or not R)
        return nullptr;

    switch (mOp)
    {
    case '+':
        return inG.GetIRBuilder().CreateFAdd(L, R, "addtmp");
    case '-':
        return inG.GetIRBuilder().CreateFSub(L, R, "subtmp");
    case '*':
        return inG.GetIRBuilder().CreateFMul(L, R, "multmp");
    case '/':
        return inG.GetIRBuilder().CreateFDiv(L, R, "divtmp");
    case '<':
        // elle one bit integer ULT = unsigned less than
        L = inG.GetIRBuilder().CreateFCmpULT(L, R, "cmptmp");
        // convert garne aba tyo one bit integer lai float ma, Create Unsigned integer to floating point
        return inG.GetIRBuilder().CreateUIToFP(L, llvm::Type::getDoubleTy(inG.GetContext()), "booltmp");
    default:
        return inG.LogErrorV("invalid binary operator");
    }
}

llvm::Value* Expr::Call::CodeGen(IrGenerator& inG)
{
    // Function call ko lagi suru ma module vitra function lai khojne
    //  LLVM module is the container that holds function that we are JITing
    //    llvm::Function* CalleeF = inG.GetModule().getFunction(mCallee);
    // looking the name in the global module table
    llvm::Function *CalleeF = inG.GetFunction(mCallee);
    if (not CalleeF)
        return inG.LogErrorV("Unknown Function Referenced\n");

    // aba argument mismatch vayo vane
    if (CalleeF->arg_size() != mArgs.size())
        return inG.LogErrorV("Incorrect # arguments passed\n");
    std::vector<llvm::Value*> ArgsV;
    for (uint32_t i = 0 , e = mArgs.size ( ); i != e; ++i)
    {
        ArgsV.push_back(mArgs[i]->CodeGen(inG));
        if (not ArgsV.back()) // last wala check gar
            return nullptr;
    }
    return inG.GetIRBuilder().CreateCall(CalleeF, ArgsV, "calltmp");
}

llvm::Function* Expr::Prototype::CodeGen(IrGenerator& inG)
{
    // function prototype ko type chae Double(Double, Double) banaune
    std::vector<llvm::Type*> Doubles(mArgs.size(), llvm::Type::getDoubleTy(inG.GetContext()));
    // All things are uniqued in llvm so, we actually "get" it instead of newing it 
    llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(inG.GetContext()), Doubles, false);
    // The function returns llvm::Function because prototype vaneko external interface for a function matra ho
    llvm::Function* F = llvm::Function::Create(FT, llvm::GlobalValue::ExternalLinkage, mName, inG.GetModule());

    // Set names for all arguments
    // llvm le afai pani dinxa tara yo garo vane herna sajjilo hune vayo
    uint32_t Idx = 0;
    for (auto& Arg : F->args())
        Arg.setName(mArgs[Idx++]);

    return F;
}

llvm::Function* Expr::Function::CodeGen(IrGenerator& inG)
{
    bool IsFunctionAlreadyPresent = inG.GetFunctionPrototypeMap().find(mPrototype->GetName())
                                    == inG.GetFunctionPrototypeMap().end();
    if (not IsFunctionAlreadyPresent)
        return (llvm::Function *) inG.LogErrorV("Redefinition of function is illegal");

    // Kunai external declaration bata existing function herne xa ki xaina "external use garera
    auto &P = *mPrototype;
    inG.GetFunctionPrototypeMap()[mPrototype->GetName()] = mv(
        mPrototype); // Save garne prototype map ma
    llvm::Function *TheFunction = inG.GetFunction(P.GetName());
    if (not TheFunction)
        return nullptr;

    // if (not TheFunction->empty())
    //     return (llvm::Function*)inG.LogErrorV("Function Cannot be redefined");
    // if (not inG.GetModule().getFunction(mPrototype->GetName()))
    //     return (llvm::Function*)inG.LogErrorV("External Function Already Exist");

    // aba funciton vitra body chirauna parne vayo
    llvm::BasicBlock* BB = llvm::BasicBlock::Create(inG.GetContext(), "entry", TheFunction);
    inG.GetIRBuilder().SetInsertPoint(BB);
    // record the function arguments in NamedValues map
    inG.GetNamedValues().clear();
    // aba variable : Expression lai accessible hunxa, 
    for (auto& Arg : TheFunction->args())
        inG.GetNamedValues()[std::string(Arg.getName())] = &Arg;

    // aba body bata code gen garne
    if (llvm::Value *RetVal = mBody->CodeGen (inG))
    {
        inG.GetIRBuilder().CreateRet(RetVal);
        // ani llvm "ret" function banaune
        llvm::verifyFunction(*TheFunction);

        inG.GetFunctionPassManager().run(*TheFunction, inG.GetFunctionAnalysisManager());
        /* Now Optimization of the function is to be done */
        return TheFunction;
    }

    // body bata vaena
    // Incorrect typing vako thyo vane kaam laagxa, hatauna laai, natra symbol table ma rai rahanxa
    TheFunction->eraseFromParent();
    return nullptr;
}
