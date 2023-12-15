#pragma once
#include "expressions.hpp"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "parser.hpp"

class IrGenerator : public Parser
{
public:
    IrGenerator(std::istream &inStream)
        : Parser(inStream)
    {
        InitializeModule();
    }
    void InitializeModule();
    void HandleDefinition();
    void HandleExtern();
    void HandleTopLevelExpression();
    void MainLoop();

private:
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

public:
    GETTER GetContext() { return *mLLVMContext; }
    GETTER GetIRBuilder() { return *mBuilder; }
    GETTER GetModule() { return *mLLVMModule; }
    std::map<std::string, llvm::Value *> &GetNamedValues() { return mNamedValues; };

    llvm::Value *LogErrorV(const std::string_view inString)
    {
        Parser::LogError(inString);
        return nullptr;
    }
};
