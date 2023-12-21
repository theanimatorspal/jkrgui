#pragma once
#include "irbirali.hpp"

namespace Birali {
class Jit : public Birali::IrBirali {
 public:
  Jit(std::istream &inStr) : Birali::IrBirali(inStr) { InitializeJit(); }
  void InitializeJit();
  virtual void InitializeModuleAndPassManagers() override;
  virtual void HandleDefinition() override;
  virtual void HandleExtern() override;
  virtual void HandleTopLevelExpression() override;
  virtual void MainLoop() override;

  virtual llvm::FunctionPassManager &GetFunctionPassManager() override {
    return *mFunctionPassManager;
  }
  virtual llvm::FunctionAnalysisManager &GetFunctionAnalysisManager ( ) override {
    return *mFunctionAnalysisManager;
  }

 private:
  up<JitHelper> mJit;
  llvm::ExitOnError mExitOnError;
  up<llvm::FunctionPassManager> mFunctionPassManager;
  up<llvm::LoopAnalysisManager> mLoopAnalysisManager;
  up<llvm::FunctionAnalysisManager> mFunctionAnalysisManager;
  up<llvm::CGSCCAnalysisManager> mCGSCCAnalysisManager;
  up<llvm::ModuleAnalysisManager> mModuleAnalysisManager;
  up<llvm::PassInstrumentationCallbacks> mPassInstrumentationCallbacks;
  up<llvm::StandardInstrumentations> mStandardInstrumentations;
};
}  // namespace Birali
