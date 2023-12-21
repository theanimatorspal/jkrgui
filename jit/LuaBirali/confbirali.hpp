#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/Support/Error.h"
#include "llvm/Target/TargetMachine.h"
#include <llvm/ADT/StringRef.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/ExecutorProcessControl.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/Orc/Shared/ExecutorSymbolDef.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassInstrumentation.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Analysis/TargetTransformInfo.h>
#include <llvm/IR/PassInstrumentation.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <llvm/Support/Error.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <llvm/Transforms/Scalar/Reg2Mem.h>
#ifdef max
undef max
#endif
#ifdef min
    undef min
#endif
#define GETTER inline auto
#define up std::unique_ptr
#define mv std::move
#define mu std::make_unique
#define mp std::make_pair

template<typename T>
using v = std::vector<T>;
using s = std::string;
using sv = std::string_view;
template<typename T, typename U>
using p = std::pair<T, U>;
using sv = std::string_view;
template<typename T, typename M>
using umap = std::unordered_map<T, M>;
template<typename T, typename M>
using map = std::map<T, M>;
using ccode = int;
inline bool IsPrint(ccode c)
{
    return c > 0 and std::isprint((char) c);
}
inline bool IsSpace(ccode c)
{
  return c > 0 and std::isspace((char)c);
}

inline bool IsSpaceAndNotNewLine(ccode c)
{
  return c > 0 and std::isspace((char)c) and c != '\n';
}

inline bool IsAlphabet(ccode c)
{
    return std::isalpha((char) c);
}
inline bool IsAlphaNumeric(ccode c)
{
    return std::isalnum((char) c);
}
inline bool IsDigit(ccode c)
{
    return std::isdigit((char) c);
}
inline bool IsEndOrNewLine(ccode inC)
{
    return inC == EOF or inC == '\r' or inC == '\n';
};
