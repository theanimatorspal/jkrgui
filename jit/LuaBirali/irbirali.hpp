#pragma once
#include "jithelper.hpp"
#include "parserbirali.hpp"

namespace Birali {
class IrBirali : public Birali::Parser {
public:
    IrBirali(std::istream& inStream)
        : Parser(inStream)
    {
    }
    llvm::Function* GetFunction(const std::string_view inName);

public:
    template <typename T = llvm::Value>
    T* LogError(const sv inString)
    {
        std::cout << "Error:" << inString << " at ";
        std::cout << "Line:" << GetLineLexed() << '\n';
        return nullptr;
    }
    virtual void InitializeModuleAndPassManagers() = 0;
    virtual void HandleDefinition() = 0;
    virtual void HandleExtern() = 0;
    virtual void HandleTopLevelExpression() = 0;
    virtual void MainLoop() = 0;
    virtual llvm::FunctionPassManager& GetFunctionPassManager() = 0;
    virtual llvm::FunctionAnalysisManager& GetFunctionAnalysisManager() = 0;

protected:
    up<llvm::LLVMContext> mLLVMContext;
    up<llvm::IRBuilder<>> mBuilder;
    up<llvm::Module> mLLVMModule;
    std::map<std::string, llvm::AllocaInst*> mNamedValues;
    std::map<std::string, up<Ast::Prototype>> mFunctionPrototypes;

public:
    GETTER& GetContext() { return *mLLVMContext; }
    GETTER& GetIRBuilder() { return *mBuilder; }
    GETTER& GetModule() { return *mLLVMModule; }
    GETTER& GetNamedValues() { return mNamedValues; };
    GETTER& GetFunctionPrototypeMap() { return mFunctionPrototypes; }

    llvm::Value* LogErrorV(const std::string_view inString)
    {
        Parser::LogError(inString);
        return nullptr;
    }

    llvm::AllocaInst* CreateEntryBlockAlloca(llvm::Function* inFunction,
        const std::string_view inName)
    {
        llvm::IRBuilder<> TemporaryBuilder(&inFunction->getEntryBlock(),
            inFunction->getEntryBlock().begin());
        return TemporaryBuilder.CreateAlloca(
            llvm::Type::getDoubleTy(this->GetContext()), nullptr, inName);
    }

public:
    /* Utility functions for Code generation */
    llvm::Value* c_Constant(double inValue)
    {
        return llvm::ConstantFP::get(*mLLVMContext, llvm::APFloat(inValue));
    }

    llvm::Type* c_Type(Ast::Type inType)
    {
        switch (inType) {
        default:
        case Ast::Type::Double:
            return llvm::Type::getDoubleTy(*mLLVMContext);
            break;
        }
    }

    llvm::Value* c_ConstantNull(Ast::Type inType)
    {
	    switch (inType)
	    {
        default:
        case Ast::Type::Double:
            return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(*mLLVMContext));
	   }
    }
};
} // namespace Birali
