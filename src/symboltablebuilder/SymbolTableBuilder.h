// Copyright (c) 2021-2023 ChilliBits. All rights reserved.

#pragma once

#include <CompilerPass.h>
#include <ast/ASTVisitor.h>

namespace spice::compiler {

const char *const MAIN_FUNCTION_NAME = "main";
const char *const RETURN_VARIABLE_NAME = "result";
const char *const THIS_VARIABLE_NAME = "this";
const char *const FOREACH_DEFAULT_IDX_VARIABLE_NAME = "idx";
const char *const CTOR_FUNCTION_NAME = "ctor";
const char *const DTOR_FUNCTION_NAME = "dtor";
const char *const ALIAS_CONTAINER_SUFFIX = ".container";
const char *const STRUCT_SCOPE_PREFIX = "struct:";
const char *const INTERFACE_SCOPE_PREFIX = "interface:";
const char *const ENUM_SCOPE_PREFIX = "enum:";
const char UNUSED_VARIABLE_NAME = '_';

/**
 * Jobs:
 * - Build symbol table
 */
class SymbolTableBuilder : private CompilerPass, public ASTVisitor {
public:
  // Constructors
  SymbolTableBuilder(GlobalResourceManager &resourceManager, SourceFile *sourceFile);

  // Public methods
  std::any visitEntry(EntryNode *node) override;
  std::any visitMainFctDef(MainFctDefNode *node) override;
  std::any visitFctDef(FctDefNode *node) override;
  std::any visitProcDef(ProcDefNode *node) override;
  std::any visitStructDef(StructDefNode *node) override;
  std::any visitInterfaceDef(InterfaceDefNode *node) override;
  std::any visitEnumDef(EnumDefNode *node) override;
  std::any visitGenericTypeDef(GenericTypeDefNode *node) override;
  std::any visitAliasDef(AliasDefNode *node) override;
  std::any visitGlobalVarDef(GlobalVarDefNode *node) override;
  std::any visitExtDecl(ExtDeclNode *node) override;
  std::any visitThreadDef(ThreadDefNode *node) override;
  std::any visitUnsafeBlockDef(UnsafeBlockDefNode *node) override;
  std::any visitForLoop(ForLoopNode *node) override;
  std::any visitForeachLoop(ForeachLoopNode *node) override;
  std::any visitWhileLoop(WhileLoopNode *node) override;
  std::any visitDoWhileLoop(DoWhileLoopNode *node) override;
  std::any visitIfStmt(IfStmtNode *node) override;
  std::any visitElseStmt(ElseStmtNode *node) override;
  std::any visitAnonymousBlockStmt(AnonymousBlockStmtNode *node) override;
  std::any visitEnumItem(EnumItemNode *node) override;
  std::any visitField(FieldNode *node) override;
  std::any visitSignature(SignatureNode *node) override;
  std::any visitDeclStmt(DeclStmtNode *node) override;

private:
  // Private members
  Scope *rootScope;
  Scope *currentScope = nullptr;
  bool hasMainFunction = false;
};

} // namespace spice::compiler