// Copyright (c) 2021-2024 ChilliBits. All rights reserved.

#pragma once

#include <Token.h>
#include <misc/Interval.h>

#include <util/GlobalDefinitions.h>

namespace spice::compiler {

// Forward declarations
class SourceFile;

struct CodeLoc {
public:
  // Constructors
  explicit CodeLoc(const antlr4::Token *token, SourceFile *sourceFile = nullptr)
      : sourceInterval(token->getStartIndex(), token->getStopIndex()), line(token->getLine()),
        col(token->getCharPositionInLine() + 1), sourceFile(sourceFile){};
  CodeLoc(const antlr4::Token *token, size_t startIdx, size_t stopIdx, SourceFile *sourceFile = nullptr)
      : sourceInterval(startIdx, stopIdx), line(token->getLine()), col(token->getCharPositionInLine() + 1),
        sourceFile(sourceFile){};
  CodeLoc(size_t line, size_t col, SourceFile *sourceFile = nullptr) : line(line), col(col), sourceFile(sourceFile) {}

  // Public members
  SourceFile *sourceFile = nullptr;
  const antlr4::misc::Interval sourceInterval;
  size_t line;
  size_t col;

  // Public methods
  [[nodiscard]] std::string toString() const;
  [[nodiscard]] std::string toPrettyString() const;
  [[nodiscard]] std::string toPrettyLine() const;
  [[nodiscard]] std::string toPrettyLineAndColumn() const;

  // Operators
  friend bool operator==(const CodeLoc &a, const CodeLoc &b);
  ALWAYS_INLINE friend bool operator<(const CodeLoc &a, const CodeLoc &b) {
    return a.line == b.line ? a.col < b.col : a.line < b.line;
  }
  ALWAYS_INLINE friend bool operator>(const CodeLoc &a, const CodeLoc &b) {
    return a.line == b.line ? a.col > b.col : a.line > b.line;
  }
};

// Make sure we have no unexpected increases in memory consumption
static_assert(sizeof(CodeLoc) == 40);

} // namespace spice::compiler