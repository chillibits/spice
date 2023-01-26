// Copyright (c) 2021-2023 ChilliBits. All rights reserved.

#include "OpRuleManager.h"

#include <SourceFile.h>
#include <ast/ASTNodes.h>
#include <global/RuntimeModuleManager.h>
#include <typechecker/TypeChecker.h>

namespace spice::compiler {

SymbolType OpRuleManager::getAssignResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  // Skip type compatibility check if the lhs is of type dyn -> perform type inference
  if (lhs.is(TY_DYN))
    return rhs;
  // Allow pointers, arrays and structs of the same type straight away
  if (lhs.isOneOf({TY_PTR, TY_ARRAY, TY_STRUCT}) && lhs == rhs)
    return rhs;
  // Allow type to ref type of the same contained type straight away
  if (lhs.is(TY_REF) && lhs.getContainedTy() == rhs)
    return lhs;
  // Allow array to pointer
  if (lhs.is(TY_PTR) && rhs.is(TY_ARRAY) && lhs.getContainedTy() == rhs.getContainedTy())
    return lhs;
  // Allow char* = string
  if (lhs.isPtrOf(TY_CHAR) && rhs.is(TY_STRING))
    return lhs;
  // Check primitive type combinations
  return validateBinaryOperation(node, ASSIGN_OP_RULES, arrayLength(ASSIGN_OP_RULES), "=", lhs, rhs);
}

SymbolType OpRuleManager::getPlusEqualResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  // Check if this is an unsafe operation
  if (lhs.isPtr() && rhs.isOneOf({TY_INT, TY_LONG, TY_SHORT})) {
    if (typeChecker->currentScope->doesAllowUnsafeOperations())
      return lhs;
    else
      throw printErrorMessageUnsafe(node, "+=", lhs, rhs);
  }

  return validateBinaryOperation(node, PLUS_EQUAL_OP_RULES, arrayLength(PLUS_EQUAL_OP_RULES), "+=", lhs, rhs);
}

SymbolType OpRuleManager::getMinusEqualResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  if (lhs.isPtr() && rhs.isOneOf({TY_INT, TY_LONG, TY_SHORT})) {
    if (typeChecker->currentScope->doesAllowUnsafeOperations())
      return lhs;
    else
      throw printErrorMessageUnsafe(node, "-=", lhs, rhs);
  }

  return validateBinaryOperation(node, MINUS_EQUAL_OP_RULES, arrayLength(MINUS_EQUAL_OP_RULES), "-=", lhs, rhs);
}

SymbolType OpRuleManager::getMulEqualResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, MUL_EQUAL_OP_RULES, arrayLength(MUL_EQUAL_OP_RULES), "*=", lhs, rhs);
}

SymbolType OpRuleManager::getDivEqualResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, DIV_EQUAL_OP_RULES, arrayLength(DIV_EQUAL_OP_RULES), "/=", lhs, rhs);
}

SymbolType OpRuleManager::getRemEqualResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, REM_EQUAL_OP_RULES, arrayLength(REM_EQUAL_OP_RULES), "%=", lhs, rhs);
}

SymbolType OpRuleManager::getSHLEqualResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, SHL_EQUAL_OP_RULES, arrayLength(SHL_EQUAL_OP_RULES), "<<=", lhs, rhs);
}

SymbolType OpRuleManager::getSHREqualResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, SHR_EQUAL_OP_RULES, arrayLength(SHR_EQUAL_OP_RULES), ">>=", lhs, rhs);
}

SymbolType OpRuleManager::getAndEqualResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, AND_EQUAL_OP_RULES, arrayLength(AND_EQUAL_OP_RULES), "&=", lhs, rhs);
}

SymbolType OpRuleManager::getOrEqualResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, OR_EQUAL_OP_RULES, arrayLength(OR_EQUAL_OP_RULES), "|=", lhs, rhs);
}

SymbolType OpRuleManager::getXorEqualResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, XOR_EQUAL_OP_RULES, arrayLength(XOR_EQUAL_OP_RULES), "^=", lhs, rhs);
}

SymbolType OpRuleManager::getLogicalAndResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, LOGICAL_AND_OP_RULES, arrayLength(LOGICAL_AND_OP_RULES), "&&", lhs, rhs);
}

SymbolType OpRuleManager::getLogicalOrResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, LOGICAL_OR_OP_RULES, arrayLength(LOGICAL_OR_OP_RULES), "||", lhs, rhs);
}

SymbolType OpRuleManager::getBitwiseAndResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, BITWISE_AND_OP_RULES, arrayLength(BITWISE_AND_OP_RULES), "&", lhs, rhs);
}

SymbolType OpRuleManager::getBitwiseOrResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, BITWISE_OR_OP_RULES, arrayLength(BITWISE_OR_OP_RULES), "|", lhs, rhs);
}

SymbolType OpRuleManager::getBitwiseXorResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, BITWISE_XOR_OP_RULES, arrayLength(BITWISE_XOR_OP_RULES), "^", lhs, rhs);
}

SymbolType OpRuleManager::getEqualResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  // Allow 'pointer == pointer' straight away
  if (lhs.isPtr() && rhs.isPtr())
    return SymbolType(TY_BOOL);
  // Allow 'pointer == int' straight away
  if (lhs.isPtr() && rhs.is(TY_INT))
    return SymbolType(TY_BOOL);
  // Check primitive type combinations
  return validateBinaryOperation(node, EQUAL_OP_RULES, arrayLength(EQUAL_OP_RULES), "==", lhs, rhs);
}

SymbolType OpRuleManager::getNotEqualResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  // Allow 'pointer != pointer' straight away
  if (lhs.isPtr() && rhs.isPtr())
    return SymbolType(TY_BOOL);
  // Allow 'pointer != int' straight away
  if (lhs.isPtr() && rhs.is(TY_INT))
    return SymbolType(TY_BOOL);
  // Check primitive type combinations
  return validateBinaryOperation(node, NOT_EQUAL_OP_RULES, arrayLength(NOT_EQUAL_OP_RULES), "!=", lhs, rhs);
}

SymbolType OpRuleManager::getLessResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, LESS_OP_RULES, arrayLength(LESS_OP_RULES), "<", lhs, rhs);
}

SymbolType OpRuleManager::getGreaterResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, GREATER_OP_RULES, arrayLength(GREATER_OP_RULES), ">", lhs, rhs);
}

SymbolType OpRuleManager::getLessEqualResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, LESS_EQUAL_OP_RULES, arrayLength(LESS_EQUAL_OP_RULES), "<=", lhs, rhs);
}

SymbolType OpRuleManager::getGreaterEqualResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, GREATER_EQUAL_OP_RULES, arrayLength(GREATER_EQUAL_OP_RULES), ">=", lhs, rhs);
}

SymbolType OpRuleManager::getShiftLeftResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, SHIFT_LEFT_OP_RULES, arrayLength(SHIFT_LEFT_OP_RULES), "<<", lhs, rhs);
}

SymbolType OpRuleManager::getShiftRightResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, SHIFT_RIGHT_OP_RULES, arrayLength(SHIFT_RIGHT_OP_RULES), ">>", lhs, rhs);
}

SymbolType OpRuleManager::getPlusResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  // Allow any* + <int/long/short>
  if (lhs.isPtr() && rhs.isOneOf({TY_INT, TY_LONG, TY_SHORT})) {
    if (typeChecker->currentScope->doesAllowUnsafeOperations())
      return lhs;
    else
      throw printErrorMessageUnsafe(node, "+", lhs, rhs);
  }
  // Allow <int/long/short> + any*
  if (lhs.isOneOf({TY_INT, TY_LONG, TY_SHORT}) && rhs.isPtr()) {
    if (typeChecker->currentScope->doesAllowUnsafeOperations())
      return rhs;
    else
      throw printErrorMessageUnsafe(node, "+", lhs, rhs);
  }

  return validateBinaryOperation(node, PLUS_OP_RULES, arrayLength(PLUS_OP_RULES), "+", lhs, rhs);
}

SymbolType OpRuleManager::getMinusResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  // Allow any* - <int/long/short>
  if (lhs.isPtr() && rhs.isOneOf({TY_INT, TY_LONG, TY_SHORT})) {
    if (typeChecker->currentScope->doesAllowUnsafeOperations())
      return lhs;
    else
      throw printErrorMessageUnsafe(node, "-", lhs, rhs);
  }
  // Allow <int/long/short> - any*
  if (lhs.isOneOf({TY_INT, TY_LONG, TY_SHORT}) && rhs.isPtr()) {
    if (typeChecker->currentScope->doesAllowUnsafeOperations())
      return rhs;
    else
      throw printErrorMessageUnsafe(node, "-", lhs, rhs);
  }

  return validateBinaryOperation(node, MINUS_OP_RULES, arrayLength(MINUS_OP_RULES), "-", lhs, rhs);
}

SymbolType OpRuleManager::getMulResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, MUL_OP_RULES, arrayLength(MUL_OP_RULES), "*", lhs, rhs);
}

SymbolType OpRuleManager::getDivResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, DIV_OP_RULES, arrayLength(DIV_OP_RULES), "/", lhs, rhs);
}

SymbolType OpRuleManager::getRemResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  return validateBinaryOperation(node, REM_OP_RULES, arrayLength(REM_OP_RULES), "%", lhs, rhs);
}

SymbolType OpRuleManager::getPrefixMinusResultType(const ASTNode *node, SymbolType lhs) {
  lhs = lhs.removeReferenceWrappers();

  return validateUnaryOperation(node, PREFIX_MINUS_OP_RULES, arrayLength(PREFIX_MINUS_OP_RULES), "-", lhs);
}

SymbolType OpRuleManager::getPrefixPlusPlusResultType(const ASTNode *node, SymbolType lhs) {
  lhs = lhs.removeReferenceWrappers();

  return validateUnaryOperation(node, PREFIX_PLUS_PLUS_OP_RULES, arrayLength(PREFIX_PLUS_PLUS_OP_RULES), "++", lhs);
}

SymbolType OpRuleManager::getPrefixMinusMinusResultType(const ASTNode *node, SymbolType lhs) {
  lhs = lhs.removeReferenceWrappers();

  return validateUnaryOperation(node, PREFIX_MINUS_MINUS_OP_RULES, arrayLength(PREFIX_MINUS_MINUS_OP_RULES), "--", lhs);
}

SymbolType OpRuleManager::getPrefixNotResultType(const ASTNode *node, SymbolType lhs) {
  lhs = lhs.removeReferenceWrappers();

  return validateUnaryOperation(node, PREFIX_NOT_OP_RULES, arrayLength(PREFIX_NOT_OP_RULES), "!", lhs);
}

SymbolType OpRuleManager::getPrefixBitwiseNotResultType(const ASTNode *node, SymbolType lhs) {
  lhs = lhs.removeReferenceWrappers();

  return validateUnaryOperation(node, PREFIX_BITWISE_NOT_OP_RULES, arrayLength(PREFIX_BITWISE_NOT_OP_RULES), "~", lhs);
}

SymbolType OpRuleManager::getPrefixMulResultType(const ASTNode *node, SymbolType lhs) {
  lhs = lhs.removeReferenceWrappers();

  if (!lhs.isPtr())
    throw SemanticError(node, OPERATOR_WRONG_DATA_TYPE, "Cannot apply de-referencing operator on type " + lhs.getName(true));
  return lhs.getContainedTy();
}

SymbolType OpRuleManager::getPrefixBitwiseAndResultType(const ASTNode *node, SymbolType lhs) {
  lhs = lhs.removeReferenceWrappers();

  return lhs.toPointer(node);
}

SymbolType OpRuleManager::getPostfixPlusPlusResultType(const ASTNode *node, SymbolType lhs) {
  lhs = lhs.removeReferenceWrappers();

  return validateUnaryOperation(node, POSTFIX_PLUS_PLUS_OP_RULES, arrayLength(POSTFIX_PLUS_PLUS_OP_RULES), "++", lhs);
}

SymbolType OpRuleManager::getPostfixMinusMinusResultType(const ASTNode *node, SymbolType lhs) {
  lhs = lhs.removeReferenceWrappers();

  return validateUnaryOperation(node, POSTFIX_MINUS_MINUS_OP_RULES, arrayLength(POSTFIX_MINUS_MINUS_OP_RULES), "--", lhs);
}

SymbolType OpRuleManager::getCastResultType(const ASTNode *node, SymbolType lhs, SymbolType rhs) {
  lhs = lhs.removeReferenceWrappers();
  rhs = rhs.removeReferenceWrappers();

  // Allow casts string -> char*  and string -> char[]
  if (lhs.isOneOf({TY_PTR, TY_ARRAY}) && lhs.getContainedTy().is(TY_CHAR) && rhs.is(TY_STRING))
    return lhs;
  // Allow casts char* -> string and char[] -> string
  if (lhs.is(TY_STRING) && rhs.isOneOf({TY_PTR, TY_ARRAY}) && rhs.getContainedTy().is(TY_CHAR))
    return lhs;
  // Allow casts any* -> any*
  if (lhs.isPtr() && rhs.isPtr()) {
    if (typeChecker->currentScope->doesAllowUnsafeOperations())
      return lhs;
    else
      throw printErrorMessageUnsafe(node, "(cast)", lhs, rhs);
  }
  // Check primitive type combinations
  return validateBinaryOperation(node, CAST_OP_RULES, arrayLength(CAST_OP_RULES), "(cast)", lhs, rhs);
}

SymbolType OpRuleManager::validateBinaryOperation(const ASTNode *node, const BinaryOpRule opRules[], size_t opRulesSize,
                                                  const char *name, const SymbolType &lhs, const SymbolType &rhs) {
  for (size_t i = 0; i < opRulesSize; i++) {
    const BinaryOpRule &rule = opRules[i];
    if (std::get<0>(rule) == lhs.getSuperType() && std::get<1>(rule) == rhs.getSuperType())
      return SymbolType(SymbolSuperType(std::get<2>(rule)));
  }
  throw printErrorMessageBinary(node, name, lhs, rhs);
}

SymbolType OpRuleManager::validateUnaryOperation(const ASTNode *node, const UnaryOpRule opRules[], size_t opRulesSize,
                                                 const char *name, const SymbolType &lhs) {
  for (size_t i = 0; i < opRulesSize; i++) {
    const UnaryOpRule &rule = opRules[i];
    if (std::get<0>(rule) == lhs.getSuperType())
      return SymbolType(SymbolSuperType(std::get<1>(rule)));
  }
  throw printErrorMessageUnary(node, name, lhs);
}

SemanticError OpRuleManager::printErrorMessageBinary(const ASTNode *node, const char *name, const SymbolType &lhs,
                                                     const SymbolType &rhs) {
  return {node, OPERATOR_WRONG_DATA_TYPE,
          "Cannot apply '" + std::string(name) + "' operator on types " + lhs.getName(true) + " and " + rhs.getName(true)};
}

SemanticError OpRuleManager::printErrorMessageUnary(const ASTNode *node, const char *name, const SymbolType &lhs) {
  return {node, OPERATOR_WRONG_DATA_TYPE, "Cannot apply '" + std::string(name) + "' operator on type " + lhs.getName(true)};
}

SemanticError OpRuleManager::printErrorMessageUnsafe(const ASTNode *node, const char *name, const SymbolType &lhs,
                                                     const SymbolType &rhs) {
  return {node, UNSAFE_OPERATION_IN_SAFE_CONTEXT,
          "Cannot apply '" + std::string(name) + "' operator on types " + lhs.getName(true) + " and " + rhs.getName(true) +
              " as this is an unsafe operation. Please use unsafe blocks if you know what you are doing."};
}

} // namespace spice::compiler