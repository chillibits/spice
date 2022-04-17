// Copyright (c) 2021-2022 ChilliBits. All rights reserved.

#include "GenericType.h"

GenericType::GenericType(const std::string &name, const SymbolType &type) {
  this->typeChain = type.getTypeChain();
}

GenericType::GenericType(const std::string &name, const std::vector<SymbolType> &typeConditions) {
  this->typeChain.push({TY_GENERIC, name});
  this->typeConditions = typeConditions;
}

GenericType::GenericType(const std::string &name) {
  this->typeChain.push({TY_GENERIC, name});
}

/**
 * Checks if the given symbol type matches all conditions to get a manifestation of the current generic type
 *
 * @return True or false
 */
bool GenericType::meetsConditions(const SymbolType &symbolType) const { return meetsTypeConditions(symbolType); }

/**
 * Checks if the given symbol type matches all type conditions to get a manifestation of the current generic type
 *
 * @return True or false
 */
bool GenericType::meetsTypeConditions(const SymbolType &symbolType) const {
  // Check type conditions
  for (const auto &typeCondition : typeConditions) {
    if (typeCondition.is(TY_DYN) || typeCondition == symbolType)
      return true;
  }
  return false;
}