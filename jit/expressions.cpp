#include "expressions.hpp"
#include "irgenerator.hpp"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
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
    llvm::AllocaInst *A = inG.GetNamedValues()[mName];
    if (not A)
        inG.LogErrorV("Unknown Variable Name");
    return inG.GetIRBuilder().CreateLoad(A->getAllocatedType(), A, mName.c_str());
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
		// user defined
        break;
    }
	
	llvm::Function *F = inG.GetFunction(std::string("binary") + mOp);
    assert(F and "Binary Operator not Found");
        llvm::Value *Operators[2] = {L, R};
    return inG.GetIRBuilder().CreateCall(F, Operators, "binop");

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
    auto Itrfuncpos = inG.GetFunctionPrototypeMap().find(mPrototype->GetName());
    bool IsFunctionAlreadyPresent = Itrfuncpos != inG.GetFunctionPrototypeMap().end();
    bool IsAnonymous = inG.Parser::mAnonymousNameGenerator.Is(mPrototype->GetName());

    if (IsFunctionAlreadyPresent) {
        if (not IsAnonymous)
            return (llvm::Function *) inG.LogErrorV("Redefinition of function is illegal");
    }

    // Kunai external declaration bata existing function herne xa ki xaina "external use garera
    auto &P = *mPrototype;
    inG.GetFunctionPrototypeMap()[mPrototype->GetName()] = mv(
        mPrototype); // Save garne prototype map ma
    llvm::Function *TheFunction = inG.GetFunction(P.GetName());
    if (not TheFunction)
        return nullptr;

	// If an operator installing it
    if (P.IsBinaryOperator())
        inG.GetBinaryOperatorPrecedenceMap()[P.GetOperatorName()] =
            P.GetBinaryPrecedence();


    // aba funciton vitra body chirauna parne vayo
    llvm::BasicBlock* BB = llvm::BasicBlock::Create(inG.GetContext(), "entry", TheFunction);
    inG.GetIRBuilder().SetInsertPoint(BB);
    // record the function arguments in NamedValues map
    inG.GetNamedValues().clear();
    // aba variable : Expression lai accessible hunxa, 
    for (auto &Arg : TheFunction->args()) {
        llvm::AllocaInst *Alloca =
            inG.CreateEntryBlockAlloca(TheFunction, std::string(Arg.getName()));
		inG.GetIRBuilder().CreateStore(&Arg, Alloca);
        inG.GetNamedValues()[std::string(Arg.getName())] = Alloca;
    }

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

/*

define i32 @max(i32 %a, i32 %b) {
entry:
  %0 = icmp sgt i32 %a, %b
  br i1 %0, label %btrue, label %bfalse

btrue:                                      ; preds = %2
  br label %end // yo vaneko chae if basic block

bfalse:                                     ; preds = %2
  br label %end // yo vaneko else basic block

end:                                     ; preds = %btrue, %bfalse
  %retval = phi i32 [%a, %btrue], [%b, %bfalse] // Yo vaneko chae mergebasic block
  ret i32 %retval
}

 */

llvm::Value *Expr::If::CodeGen(IrGenerator &inG)
{
    llvm::Value *ConditionValue = mCondition->CodeGen(inG);
    if (not ConditionValue)
        return nullptr;
    // Condition nai xaina vane k kaam vanne kuro

    // aba condition bata aako value double xa so tellai bool ma convert garnu paro
    // Compare the Condition Value with Zero, to get whether it is Equal or not to 1 bit value, true OR false
    ConditionValue = inG.GetIRBuilder().CreateFCmpONE(ConditionValue,
                                                      llvm::ConstantFP::get(inG.GetContext(),
                                                                            llvm::APFloat(0.0)),
                                                      "ifcond"); // LHS == RHS return garxa

    // Gets the current function object that is being built
    // gets it by asking the builder about the current basic block
    // and asking the block for its parent
    llvm::Function *func = inG.GetIRBuilder().GetInsertBlock()->getParent();
    // Now create blocks for then and else clauses. Tnserting the then block at the end of the function
    // End of the function
    // ConditionBr create garna create garne ho eneru

    //yaa then block ma chae func vanera hamile constructor mai pass garim
    //elle chae yo function ko taltira aafai naya block banaidinxa
    // aru diuta blocks pani create vaisako tara yo function maa chirako xaina
    llvm::BasicBlock *ThenBasicBlock = llvm::BasicBlock::Create(inG.GetContext(), "then", func);
    llvm::BasicBlock *ElseBasicBlock = llvm::BasicBlock::Create(inG.GetContext(), "else");
    llvm::BasicBlock *MergeBasicBlock = llvm::BasicBlock::Create(inG.GetContext(), "ifcont");

    // aba conditional branching banaiyo duita maddhe euta choose garne wala
    inG.GetIRBuilder().CreateCondBr(ConditionValue, ThenBasicBlock, ElseBasicBlock);

    //aba condition brach insert gariyo, aba then block ma value insert garna laagim
    inG.GetIRBuilder().SetInsertPoint(ThenBasicBlock); // Yo vaneko start mai xa vanne ho

    // Thenko value
    llvm::Value *ThenV = mThen->CodeGen(inG);
    if (not ThenV)
        return nullptr;
    // Then block sakkina, uncondtional branch garnu paryo MergeBasicBlock ma
    // kinaki LLVM requires all basic blocks to be "terminated" with a control flow instruction such as return or branch
    // yo merge block ma chae phi node create hunxa
    inG.GetIRBuilder().CreateBr(MergeBasicBlock);
    // Aba Then ko code Generate Vayesi telle cchae Current block lai Change garna sakxa so, Then Block update garne
    // Phi function ko laagi (Phi chooses the appropiate from the branches)
    ThenBasicBlock = inG.GetIRBuilder().GetInsertBlock();
    // Aba Else block emit garne
    func->insert(func->end(), ElseBasicBlock);
    // function ma else block haalne, suru ma constructor maa haleko thim, tesma then vanekai function
    // vako thyo, tara else block insert vako thena
    inG.GetIRBuilder().SetInsertPoint(ElseBasicBlock);

    llvm::Value *ElseV = mElse->CodeGen(inG);
    if (not ElseV)
        return nullptr;

    inG.GetIRBuilder().CreateBr(MergeBasicBlock);
    ElseBasicBlock = inG.GetIRBuilder().GetInsertBlock();

    func->insert(func->end(), MergeBasicBlock);
    inG.GetIRBuilder().SetInsertPoint(MergeBasicBlock);
    llvm::PHINode *PhiNode = inG.GetIRBuilder().CreatePHI(llvm::Type::getDoubleTy(inG.GetContext()),
                                                          2,
                                                          "iftmp");
    PhiNode->addIncoming(ThenV, ThenBasicBlock);
    PhiNode->addIncoming(ElseV, ElseBasicBlock);
    return PhiNode;
}

llvm::Value *Expr::For::CodeGen(IrGenerator &inG)
{
	// Function is like the scope in llvm
    llvm::Function *parentFunction = inG.GetIRBuilder().GetInsertBlock()->getParent();

	llvm::AllocaInst *Alloca =
        inG.CreateEntryBlockAlloca(parentFunction, mVariableName);
    // Suru ma Start ko code "start" wala code emit garne
    llvm::Value *StartV = mStart->CodeGen(inG);
    if (not StartV) return nullptr;

	// Store the value into the alloca 
	inG.GetIRBuilder().CreateStore(StartV, Alloca);
    // if else ma jastai parent function line.
    // recursive approach so that parent block chae aaos haat ma
    // aba kaa haalne ho ta tyo line for ---- do wala preheader basic block ho
    llvm::BasicBlock *LoopBasicBlock = llvm::BasicBlock::Create(inG.GetContext(),
                                                                "loop",
                                                                parentFunction);
    // aba euta explicit fall through insert garne Current block bata loopbasicblock ma
    inG.GetIRBuilder().CreateBr(LoopBasicBlock);
    inG.GetIRBuilder().SetInsertPoint(LoopBasicBlock);

	llvm::AllocaInst *OldValue = inG.GetNamedValues()[mVariableName];
    inG.GetNamedValues()[mVariableName] = Alloca; // symbol table ma rakhne i = 1 haalda

    // aba loop ko body lai emit garne. Elle chae aru expresssion jasari nai basic block lai change
    // garna sakxa, Body le nikaleko value lai hami ignore garxam.
    if (not mBody->CodeGen(inG))
        return nullptr;

    // Aba step value lai emit garne
    llvm::Value *StepValue = nullptr;
    if (mStep) {
        StepValue = mStep->CodeGen(inG);
        if (not StepValue)
            return nullptr;
    } else {
        StepValue = llvm::ConstantFP::get(inG.GetContext(), llvm::APFloat(1.0));
    }

    // End condition Compute Garne
    llvm::Value *EndCondition = mEnd->CodeGen(inG);
    if (not EndCondition)
        return nullptr;

    // aba addition lai bool ma convert garne
    // Equal xaina vane true
    llvm::Value *CurrentVariable = inG.GetIRBuilder().CreateLoad(
        Alloca->getAllocatedType(), Alloca, mVariableName.c_str());
    llvm::Value *NextVar =
        inG.GetIRBuilder().CreateFAdd(CurrentVariable, StepValue, "nextvar");
    inG.GetIRBuilder().CreateStore(NextVar, Alloca);

    EndCondition = inG.GetIRBuilder().CreateFCmpONE(EndCondition,
                                                    llvm::ConstantFP::get(inG.GetContext(),
                                                                          llvm::APFloat(0.0)),
                                                    "loopcond");

    llvm::BasicBlock *AfterBasicBlock = llvm::BasicBlock::Create(inG.GetContext(),
                                                                 "afterloop",
                                                                 parentFunction);

    inG.GetIRBuilder().CreateCondBr(EndCondition, LoopBasicBlock, AfterBasicBlock);

    // Specifies that the created instructions should be appended to the end of the basic block
    inG.GetIRBuilder().SetInsertPoint(AfterBasicBlock);

    // restore garne tyo maathi ko shadow vako variable lai
    if (OldValue)
        inG.GetNamedValues()[mVariableName] = OldValue;
    else
        inG.GetNamedValues().erase(mVariableName);

    return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(inG.GetContext()));
}

llvm::Value *Expr::Unary::CodeGen(IrGenerator &inG) {
    llvm::Value *OperandV = mOperand->CodeGen(inG);
    if (not OperandV)
		return nullptr; 

	llvm::Function *theFunction = inG.GetFunction(std::string("unary") + mOp);
    if (not theFunction) return inG.LogErrorV("Unknown unary operator");

	return inG.GetIRBuilder().CreateCall(theFunction, OperandV, "unop");
}
