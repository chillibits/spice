// Copyright (c) 2021-2023 ChilliBits. All rights reserved.

#include "OpRuleConversionManager.h"

#include <stdexcept>

#include <ast/ASTNodes.h>
#include <irgenerator/IRGenerator.h>
#include <symboltablebuilder/Scope.h>

namespace spice::compiler {

OpRuleConversionManager::OpRuleConversionManager(GlobalResourceManager &resourceManager, IRGenerator *irGenerator)
    : context(resourceManager.context), builder(resourceManager.builder), irGenerator(irGenerator),
      stdFunctionManager(irGenerator->stdFunctionManager) {}

ExprResult OpRuleConversionManager::getPlusEqualInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                     ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope,
                                                     size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsP = [&]() { return irGenerator->resolveAddress(lhs); };
  const auto rhsP = [&]() { return irGenerator->resolveAddress(rhs); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);

  // Handle operator overloads
  if (callsOverloadedOpFct(node, opIdx))
    return callBinaryOperatorOverloadFct(node, lhsV, rhsV, lhsP, rhsP, opIdx);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = builder.CreateFAdd(lhsV(), rhsV())};
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateAdd(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): // fallthrough
  case COMB(TY_INT, TY_LONG): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateAdd(lhsV(), rhsInt)};
  }
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *rhsShort = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateAdd(lhsV(), rhsShort)};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateAdd(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): {
    llvm::Value *rhsShort = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateAdd(lhsV(), rhsShort)};
  }
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateAdd(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG): // fallthrough
  case COMB(TY_BYTE, TY_BYTE):
    return {.value = builder.CreateAdd(lhsV(), rhsV())};
  case COMB(TY_PTR, TY_INT):   // fallthrough
  case COMB(TY_PTR, TY_SHORT): // fallthrough
  case COMB(TY_PTR, TY_LONG):
    llvm::Type *elementTy = lhsSTy.getContainedTy().toLLVMType(context, accessScope);
    return {.value = builder.CreateGEP(elementTy, lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: +="); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getMinusEqualInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                      ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope,
                                                      size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsP = [&]() { return irGenerator->resolveAddress(lhs); };
  const auto rhsP = [&]() { return irGenerator->resolveAddress(rhs); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);

  // Handle operator overloads
  if (callsOverloadedOpFct(node, opIdx))
    return callBinaryOperatorOverloadFct(node, lhsV, rhsV, lhsP, rhsP, opIdx);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = builder.CreateFSub(lhsV(), rhsV())};
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateSub(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): // fallthrough
  case COMB(TY_INT, TY_LONG): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSub(lhsV(), rhsInt)};
  }
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *rhsShort = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSub(lhsV(), rhsShort)};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateSub(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): {
    llvm::Value *rhsShort = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSub(lhsV(), rhsShort)};
  }
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSub(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG): // fallthrough
  case COMB(TY_BYTE, TY_BYTE):
    return {.value = builder.CreateSub(lhsV(), rhsV())};
  case COMB(TY_PTR, TY_INT):   // fallthrough
  case COMB(TY_PTR, TY_SHORT): // fallthrough
  case COMB(TY_PTR, TY_LONG):
    llvm::Type *elementType = lhsSTy.getContainedTy().toLLVMType(context, accessScope);
    return {.value = builder.CreateGEP(elementType, lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: -="); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getMulEqualInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                    ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsP = [&]() { return irGenerator->resolveAddress(lhs); };
  const auto rhsP = [&]() { return irGenerator->resolveAddress(rhs); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);

  // Handle operator overloads
  if (callsOverloadedOpFct(node, opIdx))
    return callBinaryOperatorOverloadFct(node, lhsV, rhsV, lhsP, rhsP, opIdx);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = builder.CreateFMul(lhsV(), rhsV())};
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateMul(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): // fallthrough
  case COMB(TY_INT, TY_LONG): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateMul(lhsV(), rhsInt)};
  }
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *rhsShort = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateMul(lhsV(), rhsShort)};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateMul(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): {
    llvm::Value *rhsShort = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateMul(lhsV(), rhsShort)};
  }
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateMul(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG): // fallthrough
  case COMB(TY_BYTE, TY_BYTE):
    return {.value = builder.CreateMul(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: *="); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getDivEqualInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                    ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsP = [&]() { return irGenerator->resolveAddress(lhs); };
  const auto rhsP = [&]() { return irGenerator->resolveAddress(rhs); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);

  // Handle operator overloads
  if (callsOverloadedOpFct(node, opIdx))
    return callBinaryOperatorOverloadFct(node, lhsV, rhsV, lhsP, rhsP, opIdx);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = builder.CreateFDiv(lhsV(), rhsV())};
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateSDiv(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): // fallthrough
  case COMB(TY_INT, TY_LONG): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSDiv(lhsV(), rhsInt)};
  }
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *rhsShort = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSDiv(lhsV(), rhsShort)};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateSDiv(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): {
    llvm::Value *rhsShort = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSDiv(lhsV(), rhsShort)};
  }
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSDiv(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG): // fallthrough
  case COMB(TY_BYTE, TY_BYTE):
    return {.value = builder.CreateSDiv(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: /="); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getRemEqualInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                    ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = builder.CreateFRem(lhsV(), rhsV())};
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateSRem(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): // fallthrough
  case COMB(TY_INT, TY_LONG): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSRem(lhsV(), rhsInt)};
  }
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *rhsShort = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSRem(lhsV(), rhsShort)};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateSRem(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): {
    llvm::Value *rhsShort = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSRem(lhsV(), rhsShort)};
  }
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSRem(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG): // fallthrough
  case COMB(TY_BYTE, TY_BYTE):
    return {.value = builder.CreateSRem(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: %="); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getSHLEqualInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                    ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateShl(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): // fallthrough
  case COMB(TY_INT, TY_LONG):  // fallthrough
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateShl(lhsV(), rhsInt)};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateShl(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): // fallthrough
  case COMB(TY_LONG, TY_INT):   // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateShl(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG): // fallthrough
  case COMB(TY_BYTE, TY_BYTE):
    return {.value = builder.CreateShl(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: <<="); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getSHREqualInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                    ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateLShr(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): // fallthrough
  case COMB(TY_INT, TY_LONG):  // fallthrough
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateLShr(lhsV(), rhsInt)};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateLShr(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): // fallthrough
  case COMB(TY_LONG, TY_INT):   // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateLShr(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG): // fallthrough
  case COMB(TY_BYTE, TY_BYTE):
    return {.value = builder.CreateLShr(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: >>="); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getAndEqualInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                    ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateAnd(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): // fallthrough
  case COMB(TY_INT, TY_LONG):  // fallthrough
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateAnd(lhsV(), rhsInt)};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateAnd(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): // fallthrough
  case COMB(TY_LONG, TY_INT):   // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateAnd(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG): // fallthrough
  case COMB(TY_BYTE, TY_BYTE):
    return {.value = builder.CreateAnd(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: &="); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getOrEqualInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                   ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateOr(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): // fallthrough
  case COMB(TY_INT, TY_LONG):  // fallthrough
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateOr(lhsV(), rhsInt)};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateOr(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): // fallthrough
  case COMB(TY_LONG, TY_INT):   // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateOr(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG): // fallthrough
  case COMB(TY_BYTE, TY_BYTE):
    return {.value = builder.CreateOr(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: |="); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getXorEqualInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                    ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateXor(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): // fallthrough
  case COMB(TY_INT, TY_LONG):  // fallthrough
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateXor(lhsV(), rhsInt)};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateXor(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): // fallthrough
  case COMB(TY_LONG, TY_INT):   // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateXor(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG): // fallthrough
  case COMB(TY_BYTE, TY_BYTE): // fallthrough
  case COMB(TY_CHAR, TY_CHAR):
    return {.value = builder.CreateXor(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: ^="); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getBitwiseAndInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                      ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope,
                                                      size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_INT, TY_INT):     // fallthrough
  case COMB(TY_SHORT, TY_SHORT): // fallthrough
  case COMB(TY_LONG, TY_LONG):   // fallthrough
  case COMB(TY_BYTE, TY_BYTE):
    return {.value = builder.CreateAnd(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: &"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getBitwiseOrInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                     ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope,
                                                     size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_INT, TY_INT):     // fallthrough
  case COMB(TY_SHORT, TY_SHORT): // fallthrough
  case COMB(TY_LONG, TY_LONG):   // fallthrough
  case COMB(TY_BYTE, TY_BYTE):
    return {.value = builder.CreateOr(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: |"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getBitwiseXorInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                      ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope,
                                                      size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_INT, TY_INT):     // fallthrough
  case COMB(TY_SHORT, TY_SHORT): // fallthrough
  case COMB(TY_LONG, TY_LONG):   // fallthrough
  case COMB(TY_BYTE, TY_BYTE):
    return {.value = builder.CreateXor(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: ^"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getEqualInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy, ExprResult &rhs,
                                                 const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsP = [&]() { return irGenerator->resolveAddress(lhs); };
  const auto rhsP = [&]() { return irGenerator->resolveAddress(rhs); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);
  const auto rhsT = rhsSTy.toLLVMType(context, accessScope);

  // Handle operator overloads
  if (callsOverloadedOpFct(node, opIdx))
    return callBinaryOperatorOverloadFct(node, lhsV, rhsV, lhsP, rhsP, opIdx);

  // Check if both values are of type pointer
  if (lhsSTy.isPtr() && rhsSTy.isPtr())
    return {.value = builder.CreateICmpEQ(lhsV(), rhsV())};

  // Check if one value is of type pointer and one is of type int
  if (lhsT->isPointerTy() && rhsT->isIntegerTy(32)) {
    llvm::Value *lhsInt = builder.CreatePtrToInt(lhsV(), rhsT);
    return {.value = builder.CreateICmpEQ(lhsInt, rhsV())};
  }

  // Check for primitive type combinations
  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = builder.CreateFCmpOEQ(lhsV(), rhsV())};
  case COMB(TY_DOUBLE, TY_INT):   // fallthrough
  case COMB(TY_DOUBLE, TY_SHORT): // fallthrough
  case COMB(TY_DOUBLE, TY_LONG): {
    llvm::Value *rhsFP = generateIToFpCast(rhsSTy, rhsV(), lhsT);
    return {.value = builder.CreateFCmpOEQ(lhsV(), rhsFP)};
  }
  case COMB(TY_INT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpOEQ(lhsFP, rhsV())};
  }
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateICmpEQ(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpEQ(lhsV(), rhsInt)};
  }
  case COMB(TY_INT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpEQ(lhsLong, rhsV())};
  }
  case COMB(TY_INT, TY_BYTE): // fallthrough
  case COMB(TY_INT, TY_CHAR): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpEQ(lhsV(), rhsInt)};
  }
  case COMB(TY_SHORT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), lhsT);
    return {.value = builder.CreateFCmpOEQ(lhsFP, rhsV())};
  }
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *lhsInt = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpEQ(lhsInt, rhsV())};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateICmpEQ(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpEQ(lhsLong, rhsV())};
  }
  case COMB(TY_SHORT, TY_BYTE): // fallthrough
  case COMB(TY_SHORT, TY_CHAR): {
    llvm::Value *rhsShort = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpEQ(lhsV(), rhsShort)};
  }
  case COMB(TY_LONG, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpOEQ(lhsFP, rhsV())};
  }
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpEQ(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG):
    return {.value = builder.CreateICmpEQ(lhsV(), rhsV())};
  case COMB(TY_LONG, TY_BYTE): // fallthrough
  case COMB(TY_LONG, TY_CHAR): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpEQ(lhsV(), rhsLong)};
  }
  case COMB(TY_BYTE, TY_INT): // fallthrough
  case COMB(TY_CHAR, TY_INT): {
    llvm::Value *lhsInt = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpEQ(lhsInt, rhsV())};
  }
  case COMB(TY_BYTE, TY_SHORT): // fallthrough
  case COMB(TY_CHAR, TY_SHORT): {
    llvm::Value *lhsShort = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpEQ(lhsShort, rhsV())};
  }
  case COMB(TY_BYTE, TY_LONG): // fallthrough
  case COMB(TY_CHAR, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpEQ(lhsLong, rhsV())};
  }
  case COMB(TY_BYTE, TY_BYTE): // fallthrough
  case COMB(TY_CHAR, TY_CHAR):
    return {.value = builder.CreateICmpEQ(lhsV(), rhsV())};
  case COMB(TY_STRING, TY_STRING): {
    // Generate call to the function isRawEqual(string, string) of the string std
    llvm::Function *opFct = stdFunctionManager.getStringIsRawEqualStringStringFct();
    llvm::Value *result = builder.CreateCall(opFct, {lhsV(), rhsV()});
    return {.value = result};
  }
  case COMB(TY_BOOL, TY_BOOL):
    return {.value = builder.CreateICmpEQ(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: =="); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getNotEqualInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                    ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsP = [&]() { return irGenerator->resolveAddress(lhs); };
  const auto rhsP = [&]() { return irGenerator->resolveAddress(rhs); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);
  const auto rhsT = rhsSTy.toLLVMType(context, accessScope);

  // Handle operator overloads
  if (callsOverloadedOpFct(node, opIdx))
    return callBinaryOperatorOverloadFct(node, lhsV, rhsV, lhsP, rhsP, opIdx);

  // Check if both values are of type pointer
  if (lhsSTy.isPtr() && rhsSTy.isPtr())
    return {.value = builder.CreateICmpNE(lhsV(), rhsV())};

  // Check if one value is of type pointer and one is of type int
  if (lhsT->isPointerTy() && rhsT->isIntegerTy(32)) {
    llvm::Value *lhsInt = builder.CreatePtrToInt(lhsV(), rhsT);
    return {.value = builder.CreateICmpNE(lhsInt, rhsV())};
  }

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = builder.CreateFCmpONE(lhsV(), rhsV())};
  case COMB(TY_DOUBLE, TY_INT):   // fallthrough
  case COMB(TY_DOUBLE, TY_SHORT): // fallthrough
  case COMB(TY_DOUBLE, TY_LONG): {
    llvm::Value *rhsFP = generateIToFpCast(rhsSTy, rhsV(), lhsT);
    return {.value = builder.CreateFCmpONE(lhsV(), rhsFP)};
  }
  case COMB(TY_INT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpONE(lhsFP, rhsV())};
  }
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateICmpNE(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpNE(lhsV(), rhsInt)};
  }
  case COMB(TY_INT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpNE(lhsLong, rhsV())};
  }
  case COMB(TY_INT, TY_BYTE): // fallthrough
  case COMB(TY_INT, TY_CHAR): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpNE(lhsV(), rhsInt)};
  }
  case COMB(TY_SHORT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpONE(lhsFP, rhsV())};
  }
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *lhsInt = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpNE(lhsInt, rhsV())};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateICmpNE(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpNE(lhsLong, rhsV())};
  }
  case COMB(TY_SHORT, TY_BYTE): // fallthrough
  case COMB(TY_SHORT, TY_CHAR): {
    llvm::Value *rhsShort = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpNE(lhsV(), rhsShort)};
  }
  case COMB(TY_LONG, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpONE(lhsFP, rhsV())};
  }
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpNE(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG):
    return {.value = builder.CreateICmpNE(lhsV(), rhsV())};
  case COMB(TY_LONG, TY_BYTE): // fallthrough
  case COMB(TY_LONG, TY_CHAR): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpNE(lhsV(), rhsLong)};
  }
  case COMB(TY_BYTE, TY_INT):
  case COMB(TY_CHAR, TY_INT): {
    llvm::Value *lhsInt = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpNE(lhsInt, rhsV())};
  }
  case COMB(TY_BYTE, TY_SHORT): // fallthrough
  case COMB(TY_CHAR, TY_SHORT): {
    llvm::Value *lhsShort = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpNE(lhsShort, rhsV())};
  }
  case COMB(TY_BYTE, TY_LONG): // fallthrough
  case COMB(TY_CHAR, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpNE(lhsLong, rhsV())};
  }
  case COMB(TY_BYTE, TY_BYTE): // fallthrough
  case COMB(TY_CHAR, TY_CHAR):
    return {.value = builder.CreateICmpNE(lhsV(), rhsV())};
  case COMB(TY_STRING, TY_STRING): {
    // Generate call to the function isRawEqual(string, string) of the string std
    llvm::Function *opFct = stdFunctionManager.getStringIsRawEqualStringStringFct();
    llvm::Value *result = builder.CreateCall(opFct, {lhsV(), rhsV()});
    // Negate the result
    return {.value = builder.CreateNot(result)};
  }
  case COMB(TY_BOOL, TY_BOOL):
    return {.value = builder.CreateICmpNE(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: !="); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getLessInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy, ExprResult &rhs,
                                                const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);
  const auto rhsT = rhsSTy.toLLVMType(context, accessScope);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = builder.CreateFCmpOLT(lhsV(), rhsV())};
  case COMB(TY_DOUBLE, TY_INT):   // fallthrough
  case COMB(TY_DOUBLE, TY_SHORT): // fallthrough
  case COMB(TY_DOUBLE, TY_LONG): {
    llvm::Value *rhsFP = generateIToFpCast(rhsSTy, rhsV(), lhsT);
    return {.value = builder.CreateFCmpOLT(lhsV(), rhsFP)};
  }
  case COMB(TY_INT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpOLT(lhsFP, rhsV())};
  }
  case COMB(TY_INT, TY_INT): {
    if (lhsSTy.isSigned() && rhsSTy.isSigned())
      return {.value = builder.CreateICmpSLT(lhsV(), rhsV())};
    else
      return {.value = builder.CreateICmpULT(lhsV(), rhsV())};
  }
  case COMB(TY_INT, TY_SHORT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpSLT(lhsV(), rhsInt)};
  }
  case COMB(TY_INT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpSLT(lhsLong, rhsV())};
  }
  case COMB(TY_SHORT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpOLT(lhsFP, rhsV())};
  }
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *lhsInt = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpSLT(lhsInt, rhsV())};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateICmpSLT(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpSLT(lhsLong, rhsV())};
  }
  case COMB(TY_LONG, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpOLT(lhsFP, rhsV())};
  }
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpSLT(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG): // fallthrough
  case COMB(TY_BYTE, TY_BYTE): // fallthrough
  case COMB(TY_CHAR, TY_CHAR):
    return {.value = builder.CreateICmpSLT(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: <"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getGreaterInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                   ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);
  const auto rhsT = rhsSTy.toLLVMType(context, accessScope);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = builder.CreateFCmpOGT(lhsV(), rhsV())};
  case COMB(TY_DOUBLE, TY_INT):   // fallthrough
  case COMB(TY_DOUBLE, TY_SHORT): // fallthrough
  case COMB(TY_DOUBLE, TY_LONG): {
    llvm::Value *rhsFP = generateIToFpCast(rhsSTy, rhsV(), lhsT);
    return {.value = builder.CreateFCmpOGT(lhsV(), rhsFP)};
  }
  case COMB(TY_INT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpOGT(lhsFP, rhsV())};
  }
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateICmpSGT(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpSGT(lhsV(), rhsInt)};
  }
  case COMB(TY_INT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpSGT(lhsLong, rhsV())};
  }
  case COMB(TY_SHORT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpOGT(lhsFP, rhsV())};
  }
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *lhsInt = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpSGT(lhsInt, rhsV())};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateICmpSGT(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpSGT(lhsLong, rhsV())};
  }
  case COMB(TY_LONG, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpOGT(lhsFP, rhsV())};
  }
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpSGT(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG): // fallthrough
  case COMB(TY_BYTE, TY_BYTE): // fallthrough
  case COMB(TY_CHAR, TY_CHAR):
    return {.value = builder.CreateICmpSGT(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: >"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getLessEqualInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                     ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope,
                                                     size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);
  const auto rhsT = rhsSTy.toLLVMType(context, accessScope);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = builder.CreateFCmpOLE(lhsV(), rhsV())};
  case COMB(TY_DOUBLE, TY_INT):   // fallthrough
  case COMB(TY_DOUBLE, TY_SHORT): // fallthrough
  case COMB(TY_DOUBLE, TY_LONG): {
    llvm::Value *rhsFP = generateIToFpCast(rhsSTy, rhsV(), lhsT);
    return {.value = builder.CreateFCmpOLE(lhsV(), rhsFP)};
  }
  case COMB(TY_INT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpOLE(lhsFP, rhsV())};
  }
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateICmpSLE(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpSLE(lhsV(), rhsInt)};
  }
  case COMB(TY_INT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpSLE(lhsLong, rhsV())};
  }
  case COMB(TY_SHORT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpOLE(lhsFP, rhsV())};
  }
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *lhsInt = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpSLE(lhsInt, rhsV())};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateICmpSLE(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpSLE(lhsLong, rhsV())};
  }
  case COMB(TY_LONG, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpOLE(lhsFP, rhsV())};
  }
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpSLE(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG): // fallthrough
  case COMB(TY_BYTE, TY_BYTE): // fallthrough
  case COMB(TY_CHAR, TY_CHAR):
    return {.value = builder.CreateICmpSLE(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: <="); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getGreaterEqualInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                        ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope,
                                                        size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);
  const auto rhsT = rhsSTy.toLLVMType(context, accessScope);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = builder.CreateFCmpOGE(lhsV(), rhsV())};
  case COMB(TY_DOUBLE, TY_INT):   // fallthrough
  case COMB(TY_DOUBLE, TY_SHORT): // fallthrough
  case COMB(TY_DOUBLE, TY_LONG): {
    llvm::Value *rhsFP = generateIToFpCast(rhsSTy, rhsV(), lhsT);
    return {.value = builder.CreateFCmpOGE(lhsV(), rhsFP)};
  }
  case COMB(TY_INT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpOGE(lhsFP, rhsV())};
  }
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateICmpSGE(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpSGE(lhsV(), rhsInt)};
  }
  case COMB(TY_INT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpSGE(lhsLong, rhsV())};
  }
  case COMB(TY_SHORT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpOGE(lhsFP, rhsV())};
  }
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *lhsInt = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpSGE(lhsInt, rhsV())};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateICmpSGE(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateICmpSGE(lhsLong, rhsV())};
  }
  case COMB(TY_LONG, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFCmpOGE(lhsFP, rhsV())};
  }
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateICmpSGE(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG): // fallthrough
  case COMB(TY_BYTE, TY_BYTE): // fallthrough
  case COMB(TY_CHAR, TY_CHAR):
    return {.value = builder.CreateICmpSGE(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: >="); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getShiftLeftInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                     ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope,
                                                     size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateShl(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): // fallthrough
  case COMB(TY_INT, TY_LONG):  // fallthrough
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateShl(lhsV(), rhsInt)};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateShl(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): // fallthrough
  case COMB(TY_LONG, TY_INT):   // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateShl(lhsV(), rhsInt)};
  }
  case COMB(TY_LONG, TY_LONG):
    return {.value = builder.CreateShl(lhsV(), rhsV())};
  case COMB(TY_BYTE, TY_INT):   // fallthrough
  case COMB(TY_BYTE, TY_SHORT): // fallthrough
  case COMB(TY_BYTE, TY_LONG): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateShl(lhsV(), rhsInt)};
  }
  case COMB(TY_BYTE, TY_BYTE):
    return {.value = builder.CreateShl(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: <<"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getShiftRightInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                      ExprResult &rhs, const SymbolType &rhsSTy, Scope *accessScope,
                                                      size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateLShr(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): // fallthrough
  case COMB(TY_INT, TY_LONG):  // fallthrough
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateLShr(lhsV(), rhsInt)};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateLShr(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): // fallthrough
  case COMB(TY_LONG, TY_INT):   // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateLShr(lhsV(), rhsInt)};
  }
  case COMB(TY_LONG, TY_LONG):
    return {.value = builder.CreateLShr(lhsV(), rhsV())};
  case COMB(TY_BYTE, TY_INT):   // fallthrough
  case COMB(TY_BYTE, TY_SHORT): // fallthrough
  case COMB(TY_BYTE, TY_LONG): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateLShr(lhsV(), rhsInt)};
  }
  case COMB(TY_BYTE, TY_BYTE):
    return {.value = builder.CreateLShr(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: >>"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getPlusInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy, ExprResult &rhs,
                                                const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsP = [&]() { return irGenerator->resolveAddress(lhs); };
  const auto rhsP = [&]() { return irGenerator->resolveAddress(rhs); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);
  const auto rhsT = rhsSTy.toLLVMType(context, accessScope);

  // Handle operator overloads
  if (callsOverloadedOpFct(node, opIdx))
    return callBinaryOperatorOverloadFct(node, lhsV, rhsV, lhsP, rhsP, opIdx);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = builder.CreateFAdd(lhsV(), rhsV())};
  case COMB(TY_DOUBLE, TY_INT):   // fallthrough
  case COMB(TY_DOUBLE, TY_SHORT): // fallthrough
  case COMB(TY_DOUBLE, TY_LONG): {
    llvm::Value *rhsFP = generateIToFpCast(rhsSTy, rhsV(), lhsT);
    return {.value = builder.CreateFAdd(lhsV(), rhsFP)};
  }
  case COMB(TY_INT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFAdd(lhsFP, rhsV())};
  }
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateAdd(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateAdd(lhsV(), rhsInt)};
  }
  case COMB(TY_INT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateAdd(lhsLong, rhsV())};
  }
  case COMB(TY_INT, TY_PTR):
    return {.value = builder.CreateGEP(rhsSTy.getContainedTy().toLLVMType(context, accessScope), rhsV(), lhsV())};
  case COMB(TY_SHORT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFAdd(lhsFP, rhsV())};
  }
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *lhsInt = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateAdd(lhsInt, rhsV())};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateAdd(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateAdd(lhsLong, rhsV())};
  }
  case COMB(TY_SHORT, TY_PTR):
    return {.value = builder.CreateGEP(rhsSTy.getContainedTy().toLLVMType(context, accessScope), rhsV(), lhsV())};
  case COMB(TY_LONG, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFAdd(lhsFP, rhsV())};
  }
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateAdd(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG):
    return {.value = builder.CreateAdd(lhsV(), rhsV())};
  case COMB(TY_LONG, TY_PTR):
    return {.value = builder.CreateGEP(rhsSTy.getContainedTy().toLLVMType(context, accessScope), rhsV(), lhsV())};
  case COMB(TY_BYTE, TY_BYTE): // fallthrough
  case COMB(TY_CHAR, TY_CHAR):
    return {.value = builder.CreateAdd(lhsV(), rhsV())};
  case COMB(TY_PTR, TY_INT):   // fallthrough
  case COMB(TY_PTR, TY_SHORT): // fallthrough
  case COMB(TY_PTR, TY_LONG):
    return {.value = builder.CreateGEP(lhsSTy.getContainedTy().toLLVMType(context, accessScope), lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: +"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getMinusInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy, ExprResult &rhs,
                                                 const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsP = [&]() { return irGenerator->resolveAddress(lhs); };
  const auto rhsP = [&]() { return irGenerator->resolveAddress(rhs); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);
  const auto rhsT = rhsSTy.toLLVMType(context, accessScope);

  // Handle operator overloads
  if (callsOverloadedOpFct(node, opIdx))
    return callBinaryOperatorOverloadFct(node, lhsV, rhsV, lhsP, rhsP, opIdx);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = builder.CreateFSub(lhsV(), rhsV())};
  case COMB(TY_DOUBLE, TY_INT):   // fallthrough
  case COMB(TY_DOUBLE, TY_SHORT): // fallthrough
  case COMB(TY_DOUBLE, TY_LONG): {
    llvm::Value *rhsFP = generateIToFpCast(rhsSTy, rhsV(), lhsT);
    return {.value = builder.CreateFSub(lhsV(), rhsFP)};
  }
  case COMB(TY_INT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFSub(lhsFP, rhsV())};
  }
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateSub(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSub(lhsV(), rhsInt)};
  }
  case COMB(TY_INT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateSub(lhsLong, rhsV())};
  }
  case COMB(TY_INT, TY_PTR):
    return {.value = builder.CreateGEP(rhsSTy.getContainedTy().toLLVMType(context, accessScope), rhsV(), lhsV())};
  case COMB(TY_SHORT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFSub(lhsFP, rhsV())};
  }
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *lhsInt = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateSub(lhsInt, rhsV())};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateSub(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateSub(lhsLong, rhsV())};
  }
  case COMB(TY_SHORT, TY_PTR):
    return {.value = builder.CreateGEP(rhsSTy.getContainedTy().toLLVMType(context, accessScope), rhsV(), lhsV())};
  case COMB(TY_LONG, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFSub(lhsFP, rhsV())};
  }
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSub(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG):
    return {.value = builder.CreateSub(lhsV(), rhsV())};
  case COMB(TY_LONG, TY_PTR):
    return {.value = builder.CreateGEP(rhsSTy.getContainedTy().toLLVMType(context, accessScope), rhsV(), lhsV())};
  case COMB(TY_BYTE, TY_BYTE): // fallthrough
  case COMB(TY_CHAR, TY_CHAR):
    return {.value = builder.CreateSub(lhsV(), rhsV())};
  case COMB(TY_PTR, TY_INT):   // fallthrough
  case COMB(TY_PTR, TY_SHORT): // fallthrough
  case COMB(TY_PTR, TY_LONG):
    return {.value = builder.CreateGEP(lhsSTy.getContainedTy().toLLVMType(context, accessScope), lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: -"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getMulInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy, ExprResult &rhs,
                                               const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsP = [&]() { return irGenerator->resolveAddress(lhs); };
  const auto rhsP = [&]() { return irGenerator->resolveAddress(rhs); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);
  const auto rhsT = rhsSTy.toLLVMType(context, accessScope);

  // Handle operator overloads
  if (callsOverloadedOpFct(node, opIdx))
    return callBinaryOperatorOverloadFct(node, lhsV, rhsV, lhsP, rhsP, opIdx);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = builder.CreateFMul(lhsV(), rhsV())};
  case COMB(TY_DOUBLE, TY_INT):   // fallthrough
  case COMB(TY_DOUBLE, TY_SHORT): // fallthrough
  case COMB(TY_DOUBLE, TY_LONG): {
    llvm::Value *rhsFP = generateIToFpCast(rhsSTy, rhsV(), lhsT);
    return {.value = builder.CreateFMul(lhsV(), rhsFP)};
  }
  case COMB(TY_INT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFMul(lhsFP, rhsV())};
  }
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateMul(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateMul(lhsV(), rhsInt)};
  }
  case COMB(TY_INT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateMul(lhsLong, rhsV())};
  }
  case COMB(TY_SHORT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFMul(lhsFP, rhsV())};
  }
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *lhsInt = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateMul(lhsInt, rhsV())};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateMul(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateMul(lhsLong, rhsV())};
  }
  case COMB(TY_LONG, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFMul(lhsFP, rhsV())};
  }
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateMul(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG): // fallthrough
  case COMB(TY_BYTE, TY_BYTE):
    return {.value = builder.CreateMul(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: *"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getDivInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy, ExprResult &rhs,
                                               const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsP = [&]() { return irGenerator->resolveAddress(lhs); };
  const auto rhsP = [&]() { return irGenerator->resolveAddress(rhs); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);
  const auto rhsT = rhsSTy.toLLVMType(context, accessScope);

  // Handle operator overloads
  if (callsOverloadedOpFct(node, opIdx))
    return callBinaryOperatorOverloadFct(node, lhsV, rhsV, lhsP, rhsP, opIdx);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = builder.CreateFDiv(lhsV(), rhsV())};
  case COMB(TY_DOUBLE, TY_INT):   // fallthrough
  case COMB(TY_DOUBLE, TY_SHORT): // fallthrough
  case COMB(TY_DOUBLE, TY_LONG): {
    llvm::Value *rhsFP = generateIToFpCast(rhsSTy, rhsV(), lhsT);
    return {.value = builder.CreateFDiv(lhsV(), rhsFP)};
  }
  case COMB(TY_INT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFDiv(lhsFP, rhsV())};
  }
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateSDiv(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSDiv(lhsV(), rhsInt)};
  }
  case COMB(TY_INT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateSDiv(lhsLong, rhsV())};
  }
  case COMB(TY_SHORT, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFDiv(lhsFP, rhsV())};
  }
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *lhsInt = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateSDiv(lhsInt, rhsV())};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateSDiv(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateSDiv(lhsLong, rhsV())};
  }
  case COMB(TY_LONG, TY_DOUBLE): {
    llvm::Value *lhsFP = generateIToFpCast(lhsSTy, lhsV(), rhsT);
    return {.value = builder.CreateFDiv(lhsFP, rhsV())};
  }
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSDiv(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG): // fallthrough
  case COMB(TY_BYTE, TY_BYTE): // fallthrough
  case COMB(TY_CHAR, TY_CHAR):
    return {.value = builder.CreateSDiv(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: /"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getRemInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy, ExprResult &rhs,
                                               const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);
  const auto rhsT = rhsSTy.toLLVMType(context, accessScope);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = builder.CreateFRem(lhsV(), rhsV())};
  case COMB(TY_INT, TY_INT):
    return {.value = builder.CreateSRem(lhsV(), rhsV())};
  case COMB(TY_INT, TY_SHORT): {
    llvm::Value *rhsInt = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSRem(lhsV(), rhsInt)};
  }
  case COMB(TY_INT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateSRem(lhsLong, rhsV())};
  }
  case COMB(TY_SHORT, TY_INT): {
    llvm::Value *lhsInt = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateSRem(lhsInt, rhsV())};
  }
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = builder.CreateSRem(lhsV(), rhsV())};
  case COMB(TY_SHORT, TY_LONG): {
    llvm::Value *lhsLong = builder.CreateIntCast(lhsV(), rhsT, true);
    return {.value = builder.CreateSRem(lhsLong, rhsV())};
  }
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT): {
    llvm::Value *rhsLong = builder.CreateIntCast(rhsV(), lhsT, true);
    return {.value = builder.CreateSRem(lhsV(), rhsLong)};
  }
  case COMB(TY_LONG, TY_LONG):
    return {.value = builder.CreateSRem(lhsV(), rhsV())};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: %"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getPrefixMinusInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                       Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };

  switch (lhsSTy.getSuperType()) {
  case TY_DOUBLE:
    return {.value = builder.CreateFMul(lhsV(), llvm::ConstantFP::get(builder.getContext(), llvm::APFloat(double(-1))))};
  case TY_INT:   // fallthrough
  case TY_SHORT: // fallthrough
  case TY_LONG:
    return {.value = builder.CreateNeg(lhsV())};
  default:
    break;
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: -"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getPrefixPlusPlusInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                          Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };

  switch (lhsSTy.getSuperType()) {
  case TY_INT:
    return {.value = builder.CreateAdd(lhsV(), builder.getInt32(1))};
  case TY_SHORT:
    return {.value = builder.CreateAdd(lhsV(), builder.getInt16(1))};
  case TY_LONG:
    return {.value = builder.CreateAdd(lhsV(), builder.getInt64(1))};
  default:
    break;
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: ++ (prefix)"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getPrefixMinusMinusInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                            Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };

  switch (lhsSTy.getSuperType()) {
  case TY_INT:
    return {.value = builder.CreateSub(lhsV(), builder.getInt32(1))};
  case TY_SHORT:
    return {.value = builder.CreateSub(lhsV(), builder.getInt16(1))};
  case TY_LONG:
    return {.value = builder.CreateSub(lhsV(), builder.getInt64(1))};
  default:
    break;
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: -- (prefix)"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getPrefixNotInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                     Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };

  switch (lhsSTy.getSuperType()) {
  case TY_BOOL:
    return {.value = builder.CreateNot(lhsV())};
  default:
    break;
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: !"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getPrefixBitwiseNotInst(const ASTNode *node, ExprResult &lhs, const SymbolType &lhsSTy,
                                                            Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };

  switch (lhsSTy.getSuperType()) {
  case TY_INT:   // fallthrough
  case TY_SHORT: // fallthrough
  case TY_LONG:
    return {.value = builder.CreateNeg(lhsV())};
  default:
    break;
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: ~"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getPostfixPlusPlusInst(const ASTNode *node, ExprResult &lhs, SymbolType lhsSTy,
                                                           Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };

  switch (lhsSTy.getSuperType()) {
  case TY_INT:
    return {.value = builder.CreateAdd(lhsV(), builder.getInt32(1))};
  case TY_SHORT:
    return {.value = builder.CreateAdd(lhsV(), builder.getInt16(1))};
  case TY_LONG:
    return {.value = builder.CreateAdd(lhsV(), builder.getInt64(1))};
  default:
    break;
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: ++ (postfix)"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getPostfixMinusMinusInst(const ASTNode *node, ExprResult &lhs, SymbolType lhsSTy,
                                                             Scope *accessScope, size_t opIdx) {
  const auto lhsV = [&]() { return irGenerator->resolveValue(lhsSTy, lhs, accessScope); };

  switch (lhsSTy.getSuperType()) {
  case TY_INT:
    return {.value = builder.CreateSub(lhsV(), builder.getInt32(1))};
  case TY_SHORT:
    return {.value = builder.CreateSub(lhsV(), builder.getInt16(1))};
  case TY_LONG:
    return {.value = builder.CreateSub(lhsV(), builder.getInt64(1))};
  default:
    break;
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: -- (postfix)"); // GCOV_EXCL_LINE
}

ExprResult OpRuleConversionManager::getCastInst(const ASTNode *node, const SymbolType &lhsSTy, ExprResult &rhs,
                                                const SymbolType &rhsSTy, Scope *accessScope, size_t opIdx) {
  const auto rhsV = [&]() { return irGenerator->resolveValue(rhsSTy, rhs, accessScope); };
  const auto lhsT = lhsSTy.toLLVMType(context, accessScope);

  switch (getTypeCombination(lhsSTy, rhsSTy)) {
  case COMB(TY_DOUBLE, TY_DOUBLE):
    return {.value = rhsV()};
  case COMB(TY_INT, TY_DOUBLE):
    return {.value = builder.CreateFPToSI(rhsV(), lhsT)};
  case COMB(TY_INT, TY_INT):
    return {.value = rhsV()};
  case COMB(TY_INT, TY_SHORT): // fallthrough
  case COMB(TY_INT, TY_LONG):  // fallthrough
  case COMB(TY_INT, TY_BYTE):  // fallthrough
  case COMB(TY_INT, TY_CHAR):
    return {.value = builder.CreateIntCast(rhsV(), lhsT, true)};
  case COMB(TY_SHORT, TY_DOUBLE):
    return {.value = builder.CreateFPToSI(rhsV(), lhsT)};
  case COMB(TY_SHORT, TY_INT):
    return {.value = builder.CreateIntCast(rhsV(), lhsT, true)};
  case COMB(TY_SHORT, TY_SHORT):
    return {.value = rhsV()};
  case COMB(TY_SHORT, TY_LONG):
    return {.value = builder.CreateIntCast(rhsV(), lhsT, true)};
  case COMB(TY_LONG, TY_DOUBLE):
    return {.value = builder.CreateFPToSI(rhsV(), lhsT)};
  case COMB(TY_LONG, TY_INT): // fallthrough
  case COMB(TY_LONG, TY_SHORT):
    return {.value = builder.CreateIntCast(rhsV(), lhsT, true)};
  case COMB(TY_LONG, TY_LONG):
    return {.value = rhsV()};
  case COMB(TY_BYTE, TY_INT):   // fallthrough
  case COMB(TY_BYTE, TY_SHORT): // fallthrough
  case COMB(TY_BYTE, TY_LONG):
    return {.value = builder.CreateIntCast(rhsV(), lhsT, false)};
  case COMB(TY_BYTE, TY_CHAR): // fallthrough
  case COMB(TY_BYTE, TY_BYTE):
    return {.value = rhsV()};
  case COMB(TY_CHAR, TY_INT):   // fallthrough
  case COMB(TY_CHAR, TY_SHORT): // fallthrough
  case COMB(TY_CHAR, TY_LONG):
    return {.value = builder.CreateIntCast(rhsV(), lhsT, false)};
  case COMB(TY_CHAR, TY_BYTE):     // fallthrough
  case COMB(TY_CHAR, TY_CHAR):     // fallthrough
  case COMB(TY_STRING, TY_STRING): // fallthrough
  case COMB(TY_STRING, TY_PTR):    // fallthrough
  case COMB(TY_BOOL, TY_BOOL):     // fallthrough
  case COMB(TY_PTR, TY_STRING):
    return {.value = rhsV()};
  case COMB(TY_PTR, TY_PTR):
    return {.value = lhsSTy.getContainedTy() == rhsSTy.getContainedTy() ? rhsV() : builder.CreatePointerCast(rhsV(), lhsT)};
  }
  throw CompilerError(UNHANDLED_BRANCH, "Operator fallthrough: (cast)"); // GCOV_EXCL_LINE
}

bool OpRuleConversionManager::callsOverloadedOpFct(const ASTNode *node, size_t opIdx) const {
  const std::vector<const Function *> &opFctList = irGenerator->getOpFctPointers(node);
  return opFctList.size() > opIdx && opFctList.at(opIdx) != nullptr;
}

ExprResult OpRuleConversionManager::callBinaryOperatorOverloadFct(const ASTNode *node, auto &lhsV, auto &rhsV, auto &lhsP,
                                                                  auto &rhsP, size_t opIdx) {
  const size_t manIdx = irGenerator->manIdx;
  assert(!node->opFct.empty() && node->opFct.size() > manIdx);
  assert(!node->opFct.at(manIdx).empty() && node->opFct.at(manIdx).size() > opIdx);
  const Function *opFct = node->opFct.at(manIdx).at(opIdx);
  assert(opFct != nullptr);

  const std::string mangledName = opFct->getMangledName();
  Scope *accessScope = opFct->entry->scope;
  assert(accessScope != nullptr);

  // Get arg values
  const std::vector<SymbolType> &paramTypes = opFct->getParamTypes();
  assert(paramTypes.size() == 2);
  llvm::Value *argValues[2];
  argValues[0] = paramTypes.at(0).isRef() ? lhsP() : lhsV();
  argValues[1] = paramTypes.at(1).isRef() ? rhsP() : rhsV();

  // Obtain information about the call
  const bool isImported = accessScope->isImportedBy(irGenerator->rootScope);
  const CodeLoc &callLoc = node->codeLoc;
  const CodeLoc &defLoc = opFct->entry->getDeclCodeLoc();
  const bool isDownCall = defLoc.line > callLoc.line || (defLoc.line == callLoc.line && defLoc.col > callLoc.col);

  // Function is not defined in the current module -> declare it
  // This can happen when:
  // 1) If this is an imported source file
  // 2) This is a down-call to a function, which is defined later in the same file
  if (isImported || isDownCall) {
    // Get returnType
    llvm::Type *returnType = builder.getVoidTy();
    if (!opFct->returnType.is(TY_DYN))
      returnType = opFct->returnType.toLLVMType(context, accessScope);

    // Get arg types
    std::vector<llvm::Type *> argTypes;
    for (const SymbolType &paramType : opFct->getParamTypes())
      argTypes.push_back(paramType.toLLVMType(context, accessScope));

    llvm::FunctionType *fctType = llvm::FunctionType::get(returnType, argTypes, false);
    irGenerator->module->getOrInsertFunction(mangledName, fctType);
  }

  // Get callee function
  llvm::Function *callee = irGenerator->module->getFunction(mangledName);
  assert(callee != nullptr);

  // Generate function call
  llvm::Value *resultValue = builder.CreateCall(callee, argValues);

  if (opFct->isProcedure() || opFct->isMethodProcedure())
    return {};

  return {.value = resultValue};
}

llvm::Value *OpRuleConversionManager::generateIToFpCast(const SymbolType &srcType, llvm::Value *srcValue, llvm::Type *targetType) const {
  if (srcType.isSigned())
    return builder.CreateSIToFP(srcValue, targetType);
  else
    return builder.CreateUIToFP(srcValue, targetType);
}

} // namespace spice::compiler