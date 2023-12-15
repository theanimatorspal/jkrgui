#include "irgenerator.hpp"

void IrGenerator::InitializeModule()
{
    mLLVMContext = mu<llvm::LLVMContext>();
    mLLVMModule = mu<llvm::Module>("JIT", *mLLVMContext);
    mBuilder = mu<llvm::IRBuilder<>>(*mLLVMContext);
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
            std::cout << "Read Top Level Expressions :";
            FnIR->print(llvm::errs());
            std::cout << '\n';

            // now tyo anonymous function lai remove garne
            FnIR->removeFromParent();
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
