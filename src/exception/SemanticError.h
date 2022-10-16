// Copyright (c) 2021-2022 ChilliBits. All rights reserved.

#pragma once

#include <exception>
#include <string>

#include <Token.h>

// Forward declarations
struct AstNode;
struct CodeLoc;

enum SemanticErrorType {
  REFERENCED_UNDEFINED_FUNCTION,
  REFERENCED_UNDEFINED_VARIABLE,
  REFERENCED_UNDEFINED_STRUCT,
  FUNCTION_AMBIGUITY,
  STRUCT_AMBIGUITY,
  VARIABLE_DECLARED_TWICE,
  FUNCTION_DECLARED_TWICE,
  GENERIC_TYPE_DECLARED_TWICE,
  STRUCT_DECLARED_TWICE,
  INTERFACE_DECLARED_TWICE,
  ENUM_DECLARED_TWICE,
  DUPLICATE_ENUM_ITEM_NAME,
  DUPLICATE_ENUM_ITEM_VALUE,
  GLOBAL_OF_TYPE_DYN,
  GLOBAL_OF_INVALID_TYPE,
  GLOBAL_CONST_WITHOUT_VALUE,
  FUNCTION_WITHOUT_RETURN_STMT,
  INVALID_PARAM_ORDER,
  DTOR_MUST_BE_PROCEDURE,
  DTOR_WITH_PARAMS,
  OPERATOR_WRONG_DATA_TYPE,
  UNEXPECTED_DYN_TYPE_SA,
  REASSIGN_CONST_VARIABLE,
  CONDITION_MUST_BE_BOOL,
  MISSING_MAIN_FUNCTION,
  FCT_PARAM_IS_TYPE_DYN,
  INVALID_BREAK_NUMBER,
  INVALID_CONTINUE_NUMBER,
  PRINTF_TYPE_ERROR,
  PRINTF_ARG_COUNT_ERROR,
  STD_NOT_FOUND,
  DUPLICATE_IMPORT_NAME,
  IMPORTED_FILE_NOT_EXISTING,
  CIRCULAR_DEPENDENCY,
  MEMBER_ACCESS_ONLY_STRUCTS,
  SCOPE_ACCESS_ONLY_IMPORTS,
  UNKNOWN_DATATYPE,
  NUMBER_OF_FIELDS_NOT_MATCHING,
  FIELD_TYPE_NOT_MATCHING,
  ARRAY_SIZE_INVALID,
  ARRAY_INDEX_NOT_INT_OR_LONG,
  ARRAY_ITEM_TYPE_NOT_MATCHING,
  EXPECTED_ARRAY_TYPE,
  SIZEOF_DYNAMIC_SIZED_ARRAY,
  RETURN_WITHOUT_VALUE_RESULT,
  RETURN_WITH_VALUE_IN_PROCEDURE,
  DYN_POINTERS_NOT_ALLOWED,
  DYN_ARRAYS_NOT_ALLOWED,
  GENERIC_TYPE_NOT_IN_TEMPLATE,
  SPECIFIER_AT_ILLEGAL_CONTEXT,
  INSUFFICIENT_VISIBILITY,
  TID_INVALID,
  JOIN_ARG_MUST_BE_TID,
  EXPECTED_GENERIC_TYPE,
  EXPECTED_VALUE,
  EXPECTED_TYPE,
  UNSAFE_OPERATION_IN_SAFE_CONTEXT,
  ASSERTION_CONDITION_BOOL,
  ARRAY_INDEX_OUT_OF_BOUNDS,
  RESERVED_KEYWORD,
  COMING_SOON_SA
};

/**
 * Custom exception for errors, occurring in the semantic analysis phase
 */
class SemanticError : public std::exception {
public:
  // Constructors
  explicit SemanticError(const AstNode *node, const SemanticErrorType &type, const std::string &message);

  // Public methods
  [[nodiscard]] const char *what() const noexcept override;

private:
  // Members
  std::string errorMessage;

  // Private methods
  [[nodiscard]] static std::string getMessagePrefix(SemanticErrorType errorType);
};