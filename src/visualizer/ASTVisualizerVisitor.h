// Copyright (c) 2021-2022 ChilliBits. All rights reserved.

#pragma once

#include <string>
#include <vector>

#include <ast/AbstractAstVisitor.h>
#include <ast/AstNodes.h>

/**
 * Visitor for debug purposes (is only executed in the compiler debug mode)
 *
 * Jobs:
 * - Visualize AST
 */
class ASTVisualizerVisitor : public AbstractAstVisitor {
public:
  // Constructors
  explicit ASTVisualizerVisitor(const AstNode *ast) : ast(ast){};

  // Visitor methods
  std::any visitEntry(const EntryNode *ctx) override { return buildNode(ctx); }
  std::any visitMainFctDef(const MainFctDefNode *ctx) override { return buildNode(ctx); }
  std::any visitFctDef(const FctDefNode *ctx) override { return buildNode(ctx); }
  std::any visitProcDef(const ProcDefNode *ctx) override { return buildNode(ctx); }
  std::any visitStructDef(const StructDefNode *ctx) override { return buildNode(ctx); }
  std::any visitGenericTypeDef(const GenericTypeDefNode *ctx) override { return buildNode(ctx); }
  std::any visitGlobalVarDef(const GlobalVarDefNode *ctx) override { return buildNode(ctx); }
  std::any visitImportStmt(const ImportStmtNode *ctx) override { return buildNode(ctx); }
  std::any visitExtDecl(const ExtDeclNode *ctx) override { return buildNode(ctx); }

private:
  // Members
  const AstNode *ast;
  const std::vector<std::string> nodeNames;
  int currentTabs = 1;
  std::string parentNodeId;

  // Private methods
  template <typename T> std::string buildNode(const T *ctx) {
    // Prepare strings
    std::string codeLoc = ctx->codeLoc.toString();
    std::string nodeName = getNodeName(*ctx);
    std::string nodeId = codeLoc + "_" + nodeName;

    // Build result
    std::string result = nodeId + R"( [color="lightgreen",label=")" + nodeName + "\"];\n";

    // Link parent node with the current one
    std::string parentNodeIdBackup = parentNodeId;
    if (!parentNodeId.empty())
      result += getSpaces() + parentNodeId + " -> " + nodeId + ";\n";
    parentNodeId = nodeId; // Set parentNodeId for children

    // Visit all the children
    for (auto &child : ctx->children) {
      result += getSpaces();
      result += any_cast<std::string>(visit(child));
    }

    // Restore parent node id
    parentNodeId = parentNodeIdBackup;

    return result;
  }

  template <typename T> std::string getNodeName(const T) const {
    if (std::is_same<EntryNode, T>())
      return "Entry";
    if (std::is_same<MainFctDefNode, T>())
      return "MainFctDef";
    if (std::is_same<FctDefNode, T>())
      return "FctDef";
    if (std::is_same<ProcDefNode, T>())
      return "ProcDef";
    return "Unknown";
  }

  [[nodiscard]] std::string getSpaces() const;
};
