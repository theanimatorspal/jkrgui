#include "jitcompilerfromir.hpp"
#include "expressions.hpp"
#include <string_view>

void JitCompiler::HandleTopLevelExpression()
{
    // Top level expression euta anonymouse function ma line
    if (auto FnAST = Parser::ParseTopLevelExpression()) {
        if (FnAST->CodeGen(*this)) {
            // Memory is managed by resource tracker for the Jitted memory allocated for our
            // anonymous expression
            auto ResourceTracker = mJit->GetMainJitDyLib().createResourceTracker();
            // naya thread safe module
            auto ThreadSafeModule = llvm::orc::ThreadSafeModule(mv(mLLVMModule), mv(mLLVMContext));
            // This triggers code generations for all code in the module;
            mExitOnError(mJit->AddModule(mv(ThreadSafeModule), ResourceTracker));
            // Yaa naya module banaera Chiraidine ani, tyo module faaliyo, so naya banaune aba
            InitializeModuleAndPassManagers();

            // Once the module has been added to jit it can no longer be modified,
            // so we also open a new module to hold subsequent mode

            // Now searching the JIT symbol AnonymousName
            auto ExprSymbol = mExitOnError(mJit->LookUp(mAnonymousNameGenerator.Get()));

            // Get the symbol's address ani cast garne right type ma
            // double() so we can call the native function
            // matlab ki statement 5+3 garda return hos 8.0f
            double (*functionPointer)() = ExprSymbol.getAddress().toPtr<double (*)()>();
            std::cout << "Evaluated to " << functionPointer() << '\n';

            // now tyo anonymous function lai remove garne
            mExitOnError(ResourceTracker->remove());
        }
    } else {
        // skip
        Parser::GetNextToken();
    }
}

void JitCompiler::MainLoop()
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
            this->HandleDefinition();
            break;
        case Token::External:
            this->HandleExtern();
            break;
        default:
            this->HandleTopLevelExpression();
            break;
        }
    }
}

void JitCompiler::HandleDefinition()
{
    if (up<Expr::Function> FnAST = Parser::ParseDefinition()) {
        if (llvm::Function *FnIR = FnAST->CodeGen(*this)) {
            std::cout << "Read Function Definition:";
            FnIR->print(llvm::errs());
            std::cout << "\n";
            /// aba pratyek definition ko laagi euta thread safe module banaune halne
            auto TsModule = llvm::orc::ThreadSafeModule(mv(IrGenerator::mLLVMModule),
                                                        mv(IrGenerator::mLLVMContext));
            IrGenerator::mExitOnError(IrGenerator::mJit->AddModule(mv(TsModule)));
            InitializeModuleAndPassManagers();
        }
    } else {
        // skip the token for error recovery
        Parser::GetNextToken();
    }
}

void JitCompiler::HandleExtern()
{
    if (auto ProtoAST = Parser::ParseExtern()) {
        if (auto *FnIR = ProtoAST->CodeGen(*this)) {
            std::cout << "Read Extern:";
            FnIR->print(llvm::errs());
            std::cout << '\n';
            // extern vae naya entry haalne
            IrGenerator::GetFunctionPrototypeMap()[ProtoAST->GetName()] = mv(ProtoAST);
        }
    } else {
        Parser::GetNextToken();
    }
}

llvm::Function *JitCompiler::GetFunction(const std::string_view inName)
{
    // pratyek function afnai module ma rahos vanna kaa laagi purano function declaration haru, pratyek naya module we open.
    if (llvm::Function *func = IrGenerator::mLLVMModule->getFunction(inName))
        return func;
    // xaina vane chae prototype ma herne, xa ki kunai function
    auto function = IrGenerator::mFunctionPrototypes.find(std::string(inName));
    if (function != IrGenerator::mFunctionPrototypes.end())
        return function->second->CodeGen(*this);

    // xaina vane return null
    return nullptr;
}
