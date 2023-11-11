// Copyright (c) 2021-2023 ChilliBits. All rights reserved.

#pragma once

#include <filesystem>
#include <utility>

#include <BaseErrorListener.h>

namespace spice::compiler {

enum class ThrowingErrorListenerMode {
  LEXER,
  PARSER,
};

class AntlrThrowingErrorListener : public antlr4::BaseErrorListener {
public:
  // Constructors
  AntlrThrowingErrorListener(ThrowingErrorListenerMode mode, std::filesystem::path filePath)
      : mode(mode), filePath(std::move(filePath)){};

  // Public methods
  void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol, size_t line, size_t charPositionInLine,
                   const std::string &msg, std::exception_ptr e) override;

private:
  // Private members
  ThrowingErrorListenerMode mode;
  std::filesystem::path filePath;
};

} // namespace spice::compiler