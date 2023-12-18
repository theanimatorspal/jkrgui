#include "irgenerator.hpp"
#include "expressions.hpp"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/Support/TargetSelect.h"
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/IR/PassInstrumentation.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <llvm/Support/Error.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>

void IrGenerator::InitializeJit()
{
    /* This is FOR JIT */
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    //    mJit = mExitOnError(Jit::Create());
    mJit = mExitOnError(Jit::Create());
}

void IrGenerator::InitializeModuleAndPassManagers()
{
    mLLVMContext = mu<llvm::LLVMContext>();
    mLLVMModule = mu<llvm::Module>("JIT", *mLLVMContext);
    mLLVMModule->setDataLayout(mJit->GetDataLayout());

    mBuilder = mu<llvm::IRBuilder<>>(*mLLVMContext);

    /*
     * Aba नयाँ pass बनाउने अनि नयाँ analysis managers पनि बनाउने हो । 
     * */

    mFunctionPassManager = mu<llvm::FunctionPassManager>();
    mLoopAnalysisManager = mu<llvm::LoopAnalysisManager>();
    mFunctionAnalysisManager = mu<llvm::FunctionAnalysisManager>();
    mCGSCCAnalysisManager = mu<llvm::CGSCCAnalysisManager>();
    mModuleAnalysisManager = mu<llvm::ModuleAnalysisManager>();
    mPassInstrumentationCallbacks = mu<llvm::PassInstrumentationCallbacks>();
    mStandardInstrumentations = mu<llvm::StandardInstrumentations>(*mLLVMContext,
                                                                   /* debug logging */
                                                                   true);
    mStandardInstrumentations->registerCallbacks(*mPassInstrumentationCallbacks,
                                                 mModuleAnalysisManager.get());

    /* *
	     * अब यो भएसी के गर्ने भन्ने कुरो हो भने देखि addPass हरू गरेर transform pass हरू दिने 
     */
    /*
     * llvm::PassInfoMixin<InstCombinePass> vanne ho yo
     * Instruction Combine pass,
     * Constant Folding,
     * Algebraic Simplifications
     * Strength Redunction
     * Elimination of Redundant  instructions
     * Instruction recombination
     * */
    mFunctionPassManager->addPass(llvm::InstCombinePass());

    /* Rearranging */
    mFunctionPassManager->addPass(llvm::ReassociatePass());

    /* Global Value Numbering, replacing equivalent computations already done. */
    mFunctionPassManager->addPass(llvm::GVNPass());
    /* Transforming the control flow graph
     * Loop Wala
	*/
    mFunctionPassManager->addPass(llvm::SimplifyCFGPass());

    /* Register analysis passes used in these transform passes */
    /*
     * Four optimization passes
	 */
    llvm::PassBuilder pb;
    pb.registerModuleAnalyses(*mModuleAnalysisManager);
    pb.registerFunctionAnalyses(*mFunctionAnalysisManager);
    pb.crossRegisterProxies(*mLoopAnalysisManager,
                            *mFunctionAnalysisManager,
                            *mCGSCCAnalysisManager,
                            *mModuleAnalysisManager);
    /* अब पास म्यानेजर बनो  use गरौँ */
}

void IrGenerator::HandleDefinition()
{
    if (up<Expr::Function> FnAST = Parser::ParseDefinition()) {
        if (llvm::Function* FnIR = FnAST->CodeGen(*this)) {
            std::cout << "Read Function Definition:";
            FnIR->print(llvm::errs());
            std::cout << "\n";
        }
    } else {
        // skip the token for error recovery
        Parser::GetNextToken();
    }
}

void IrGenerator::HandleExtern()
{
    if (auto ProtoAST = Parser::ParseExtern ( ))
    {
        if (auto *FnIR = ProtoAST->CodeGen (*this)) {
            std::cout << "Read Extern:";
            FnIR->print(llvm::errs());
            std::cout << '\n';
        }
    } else {
        Parser::GetNextToken();
	}
}

void IrGenerator::HandleTopLevelExpression()
{
    // Top level expression euta anonymouse function ma line
    if (auto FnAST = Parser::ParseTopLevelExpression ( ))
    {
        if (auto *FnIR = FnAST->CodeGen(*this))
        {
            // naya thread safe module

            std::cout << "Read Top Level Expressions :";
            FnIR->print(llvm::errs());
            std::cout << '\n';

            // now tyo anonymous function lai remove garne
            FnIR->eraseFromParent();
        }
    } else {
        // skip 
        Parser::GetNextToken();
    }
}

void IrGenerator::MainLoop()
{
	std::cout << "Ready :: \n";
    while (true) {
        switch (Parser::GetCurretToken()) {
        case Token::Eof:
            return;
            break;
        case ';':
        case Token::End:
            IrGenerator::GetNextToken();
            break;
        case Token::Function:
            IrGenerator::HandleDefinition();
            break;
        case Token::External:
            IrGenerator::HandleExtern();
            break;
        default:
            IrGenerator::HandleTopLevelExpression();
            break;
        }
    }
}
