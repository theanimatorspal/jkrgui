#include "irbirali.hpp"

llvm::Function* Birali::IrBirali::GetFunction(const std::string_view inName) {
  if (llvm::Function* func = mLLVMModule->getFunction(inName)) return func;
  auto function = mFunctionPrototypes.find(std::string(inName));
  if (function != mFunctionPrototypes.end())
      return function->second->CodeGen(*this);

  return nullptr;
}
