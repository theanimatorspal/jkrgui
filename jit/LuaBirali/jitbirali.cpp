#include "jitbirali.hpp"

using namespace Birali;

void Birali::Jit::InitializeJit() {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  mJit = mExitOnError(JitHelper::Create());
}

void Jit::InitializeModuleAndPassManagers() {
    mLLVMContext = mu<llvm::LLVMContext>();
    mLLVMModule = mu<llvm::Module>("JIT", *mLLVMContext);
    mLLVMModule->setDataLayout(mJit->GetDataLayout());
    mBuilder = mu<llvm::IRBuilder<>>(*mLLVMContext);
    mFunctionPassManager = mu<llvm::FunctionPassManager>();
    mLoopAnalysisManager = mu<llvm::LoopAnalysisManager>();
    mFunctionAnalysisManager = mu<llvm::FunctionAnalysisManager>();
    mCGSCCAnalysisManager = mu<llvm::CGSCCAnalysisManager>();
    mModuleAnalysisManager = mu<llvm::ModuleAnalysisManager>();
    mPassInstrumentationCallbacks = mu<llvm::PassInstrumentationCallbacks>();
    mStandardInstrumentations = mu<llvm::StandardInstrumentations>(*mLLVMContext, true);
    mStandardInstrumentations->registerCallbacks(*mPassInstrumentationCallbacks,
                                                 mModuleAnalysisManager.get());

    mFunctionPassManager->addPass(llvm::RegToMemPass());
    mFunctionPassManager->addPass(llvm::InstCombinePass());
    mFunctionPassManager->addPass(llvm::ReassociatePass());
    mFunctionPassManager->addPass(llvm::GVNPass());
    mFunctionPassManager->addPass(llvm::SimplifyCFGPass());
    llvm::PassBuilder pb;
    pb.registerModuleAnalyses(*mModuleAnalysisManager);
    pb.registerFunctionAnalyses(*mFunctionAnalysisManager);
    pb.crossRegisterProxies(*mLoopAnalysisManager,
                            *mFunctionAnalysisManager,
                            *mCGSCCAnalysisManager,
                            *mModuleAnalysisManager);
}

