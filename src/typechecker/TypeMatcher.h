// Copyright (c) 2021-2023 ChilliBits. All rights reserved.

#pragma once

#include <model/GenericType.h>

namespace spice::compiler {

// Forward declarations
class SymbolType;

/**
 * Helper class for FunctionManager and StructManager to match generic types.
 * This is a tricky task, because the types can be indirect or/and nested.
 */
class TypeMatcher {
public:
  // Type defs
  using ResolverFct = std::function<const GenericType *(const std::string &)>;

  // Public methods
  static bool matchRequestedToCandidateTypes(const std::vector<SymbolType> &candidateType,
                                             const std::vector<SymbolType> &requestedType, TypeMapping &typeMapping,
                                             ResolverFct &resolverFct, bool strictSpecifiers);
  static bool matchRequestedToCandidateType(SymbolType candidateType, SymbolType requestedType, TypeMapping &typeMapping,
                                            ResolverFct &resolverFct, bool strictSpecifiers);
  static void substantiateTypeWithTypeMapping(SymbolType &symbolType, const TypeMapping &typeMapping);
};

} // namespace spice::compiler