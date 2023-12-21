#include "confbirali.hpp"

class JitHelper {
 public:
  JitHelper(up<llvm::orc::ExecutionSession> inExecutionSession,
      llvm::orc::JITTargetMachineBuilder inJitTargetMachineBuilder,
      llvm::DataLayout inDataLayout)
      : mExecutionSession(mv(inExecutionSession)),
        mDataLayout(mv(inDataLayout)),
        mMangleAndInterner(*this->mExecutionSession, this->mDataLayout),
        mObjectLinkingLayer(*this->mExecutionSession,
                            []() { return mu<llvm::SectionMemoryManager>(); }),
        mCompilerLayer(
            *this->mExecutionSession, mObjectLinkingLayer,
            mu<llvm::orc::ConcurrentIRCompiler>(mv(inJitTargetMachineBuilder))),
        mMainJittedDynamicLibrary(
            this->mExecutionSession->createBareJITDylib("<main>")) {
    mMainJittedDynamicLibrary.addGenerator(llvm::cantFail(
        llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
            mDataLayout.getGlobalPrefix())));

    if (inJitTargetMachineBuilder.getTargetTriple()
            .isOSBinFormatCOFF()) {  // IDK what happens to moved thing here
      mObjectLinkingLayer.setOverrideObjectFlagsWithResponsibilityFlags(true);
      mObjectLinkingLayer.setAutoClaimResponsibilityForObjectSymbols(true);
    }
  }

  ~JitHelper() {
    if (auto Error = mExecutionSession->endSession()) {
      mExecutionSession->reportError(mv(Error));
    }
  }

  static llvm::Expected<up<JitHelper>> Create() {
    auto ExecutorProcessControl =
        llvm::orc::SelfExecutorProcessControl::Create();
    if (not ExecutorProcessControl) {
      return ExecutorProcessControl.takeError();
    }
    up<llvm::orc::ExecutionSession> ExecutionSession =
        mu<llvm::orc::ExecutionSession>(mv(*ExecutorProcessControl));

    llvm::orc::JITTargetMachineBuilder Jtmbuilder(
        ExecutionSession->getTargetTriple());

    auto DataLayout = Jtmbuilder.getDefaultDataLayoutForTarget();
    if (not DataLayout) {
      return DataLayout.takeError();
    }

    return mu<JitHelper>(mv(ExecutionSession), mv(Jtmbuilder), mv(*DataLayout));
  }

  const llvm::DataLayout &GetDataLayout() const { return mDataLayout; }
  llvm::orc::JITDylib &GetMainJitDyLib() const {
    return mMainJittedDynamicLibrary;
  }

  llvm::Error AddModule(llvm::orc::ThreadSafeModule inTsm,
                        llvm::orc::ResourceTrackerSP inResTracker = nullptr) {
    if (not inResTracker) {
      inResTracker = mMainJittedDynamicLibrary.getDefaultResourceTracker();
    }
    return mCompilerLayer.add(mMainJittedDynamicLibrary, mv(inTsm));
  }

  llvm::Expected<llvm::orc::ExecutorSymbolDef> LookUp(llvm::StringRef inName) {
    return mExecutionSession->lookup({&mMainJittedDynamicLibrary},
                                     mMangleAndInterner(inName.str()));
  }

 private:
  up<llvm::orc::ExecutionSession> mExecutionSession;
  llvm::DataLayout mDataLayout;
  llvm::orc::MangleAndInterner mMangleAndInterner;
  llvm::orc::RTDyldObjectLinkingLayer mObjectLinkingLayer;
  llvm::orc::IRCompileLayer mCompilerLayer;
  llvm::orc::JITDylib &mMainJittedDynamicLibrary;
};
