#pragma once
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include "expressions.hpp"
#include "jit.hpp"
#include "parser.hpp"
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassInstrumentation.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <map>
#define GETTER inline auto
#define mu std::make_unique
#define up std::unique_ptr
#define mv std::move

class IrGenerator : public Parser
{
public:
    IrGenerator(std::istream &inStream)
        : Parser(inStream)
    {
        InitializeJit();
        InitializeModuleAndPassManagers();
    }
    void InitializeJit();
    void InitializeModuleAndPassManagers();
    void HandleDefinition();
    void HandleExtern();
    void HandleTopLevelExpression();
    void MainLoop();
    virtual llvm::Function *GetFunction(const std::string_view inName) = 0;

protected:
    // Context is an opaque object that owns a lot of core LLVM data structures, such as type and constant value tables
    up<llvm::LLVMContext> mLLVMContext;
    // Helper object that makes it easy to generate LLVM instructions,
    // Instances of the IRBuilder class template keep track of the current place to insert instructions and has methods to
    // create new instructions.
    up<llvm::IRBuilder<>> mBuilder;
    // Module is LLVM construct that constains functions and global variables. In many ways it is a top level structure
    // that the LLVM IR uses to contain code. It will own the memory of all of the IR we generate, yei karan le
    // codegen() le Value*  return garxa not up<Value>
    up<llvm::Module> mLLVMModule;
    // This map keeps track of which values are defined in current scope and what their LLVM representation is. (In other words,
    // it is a symbol table for the code).
    std::map<std::string, llvm::Value *> mNamedValues;
    std::map<std::string, up<Expr::Prototype>> mFunctionPrototypes;

    /*
     * This is a functor for taking an llvm::Expected<> stuff and handle it, to be used as a tool
     * */
    llvm::ExitOnError mExitOnError;

    /* Now per function optimizations hamilai k chainxa vane dekhi euta function pass manager */
    up<Jit> mJit;

    /*  
     *  The four analysis managers allows us to basically run across four levels of the IR hirearchy  
     *  PassInstrumentationCallbacks and StandardInstrumentations are required for the pass instrumentation
     *  framework, which allows developers to customize what happens between passes.
     *  */

    /* Funciton Pass Manager for per function opt ko laagi hola */
    // Convenience typedef for a pass manager over functions.
    up<llvm::FunctionPassManager> mFunctionPassManager;
    /* For managing per loop analysis
     * */
    up<llvm::LoopAnalysisManager> mLoopAnalysisManager;
    /* Analysis managers are basically containers for analyses that lazily runs the and caches their results */
    up<llvm::FunctionAnalysisManager> mFunctionAnalysisManager;
    /* Call graph strongly connected component */
    /* Call graph le chae functions le अरु functionslai कसरी call गरेको छ भन्ने कुरा निश्चय गर्दछ । */
    up<llvm::CGSCCAnalysisManager> mCGSCCAnalysisManager;
    up<llvm::ModuleAnalysisManager> mModuleAnalysisManager;
    /* Allows users to hook into optimization pipeline before, after and during the programs' execution */
    up<llvm::PassInstrumentationCallbacks> mPassInstrumentationCallbacks;
    /* predefined instrumentation passes and utilities provided by llvm framework
     * Commonly used for profiling, performance and debugging purposes
	 */
    up<llvm::StandardInstrumentations> mStandardInstrumentations;

public:
    GETTER &GetContext() { return *mLLVMContext; }
    GETTER &GetIRBuilder() { return *mBuilder; }
    GETTER &GetModule() { return *mLLVMModule; }
    std::map<std::string, llvm::Value *> &GetNamedValues() { return mNamedValues; };
    GETTER &GetFunctionPassManager() { return *mFunctionPassManager; }
    GETTER &GetFunctionAnalysisManager() { return *mFunctionAnalysisManager; }
    GETTER &GetFunctionPrototypeMap() { return mFunctionPrototypes; }

    llvm::Value *LogErrorV(const std::string_view inString)
    {
        Parser::LogError(inString);
        return nullptr;
    }
};
