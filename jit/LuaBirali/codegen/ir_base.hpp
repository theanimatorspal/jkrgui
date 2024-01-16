#pragma once
#include "../lexer/lexer.hpp"
#include "jit_helper.hpp"
#include "lua.hpp"


class Ir_base : public Lexer {
public:
    Ir_base() = default;
    Ir_base(std::istream& inS)
        : Lexer(inS)
    {
        InitializeJit();
        InitializeModulePassAndMessengers();
    }

    getter& getbuilder() { return *mb; } 
    getter& getnamedvalues() { return mNamedValues; }


private:
    up<llvm::LLVMContext> mirc;
    up<llvm::IRBuilder<>> mb;
    up<llvm::Module> mmod;

    llvm::ExitOnError mExitOnError;
    up<Jit> mJit;
    up<llvm::FunctionPassManager> mFunctionPassManager;
    up<llvm::LoopAnalysisManager> mLoopAnalysisManager;
    up<llvm::FunctionAnalysisManager> mFunctionAnalysisManager;
    up<llvm::CGSCCAnalysisManager> mCGSCCAnalysisManager;
    up<llvm::ModuleAnalysisManager> mModuleAnalysisManager;
    up<llvm::PassInstrumentationCallbacks> mPassInstrumentationCallbacks;
    up<llvm::StandardInstrumentations> mStandardInstrumentations;

    void InitializeJit()
    {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();
        mJit = mExitOnError(Jit::Create());
    }
    void InitializeModulePassAndMessengers()
    {
        mirc = mu<llvm::LLVMContext>();
        mmod = mu<llvm::Module>("LuaBiraliJit", *mirc);
        mmod->setDataLayout(mJit->GetDataLayout());
        mb = mu<llvm::IRBuilder<>>(*mirc);
        mFunctionPassManager = mu<llvm::FunctionPassManager>();
        mLoopAnalysisManager = mu<llvm::LoopAnalysisManager>();
        mFunctionAnalysisManager = mu<llvm::FunctionAnalysisManager>();
        mCGSCCAnalysisManager = mu<llvm::CGSCCAnalysisManager>();
        mModuleAnalysisManager = mu<llvm::ModuleAnalysisManager>();
        mPassInstrumentationCallbacks = mu<llvm::PassInstrumentationCallbacks>();
        mStandardInstrumentations = mu<llvm::StandardInstrumentations>(*mirc,
            /* debug logging */
            true);
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

protected:
    map<VarDesc, llvm::AllocaInst*> mNamedValues;
    Dyndata mData;
    GlobalData mGlobalData;
};