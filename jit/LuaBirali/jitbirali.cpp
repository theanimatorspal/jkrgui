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

void Birali::Jit::HandleDefinition() {
    if (up<Ast::Function> FnAST = Parser::ParseFunctionDefinition()) {
      if (llvm::Function *FnIR = FnAST->CodeGen(*this)) {
        std::cout << "Read Function Definition:";
        FnIR->print(llvm::errs());
        std::cout << "\n";
        auto TsModule = llvm::orc::ThreadSafeModule(
            mv(mLLVMModule), mv(mLLVMContext));
        mExitOnError(mJit->AddModule(mv(TsModule)));
        InitializeModuleAndPassManagers();
      }
    } else {
      Parser::GetNextToken();
    }
}

void Birali::Jit::HandleExtern() {}

void Birali::Jit::HandleTopLevelExpression() {
    if (auto FnAST = Parser::ParseTopLevelExpression()) {
      if (FnAST->CodeGen(*this)) {
        auto ResourceTracker = mJit->GetMainJitDyLib().createResourceTracker();
        auto ThreadSafeModule =
            llvm::orc::ThreadSafeModule(mv(mLLVMModule), mv(mLLVMContext));
        mExitOnError(mJit->AddModule(mv(ThreadSafeModule), ResourceTracker));
        InitializeModuleAndPassManagers();
        auto ExprSymbol =
            mExitOnError(mJit->LookUp(mAnonymousNameGenerator.Get()));

        double (*functionPointer)() =
            ExprSymbol.getAddress().toPtr<double (*)()>();
        std::cout << "Evaluated to " << functionPointer() << '\n';

        mExitOnError(ResourceTracker->remove());
      }
    } else {
      Parser::GetNextToken();
    }
}

void Birali::Jit::MainLoop() {
    Parser::SetShouldIgnoreNewLines(false);
    while (true) {
      switch (Parser::GetCurrentToken()) {
      case Token::Eof:
        return;
        break;
      case '\n':
      case Token::End:
        GetNextToken();
        break;
      case Token::Function:
        HandleDefinition();
        break;
      default:
        this->HandleTopLevelExpression();
        break;
      }
    }
}
