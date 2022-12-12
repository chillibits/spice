// Copyright (c) 2021-2022 ChilliBits. All rights reserved.

#include "ParallelizableASTVisitor.h"

#include <ast/ASTNodes.h>

std::any ParallelizableASTVisitor::visit(const ASTNode *node) { return node->accept(this); }

std::any ParallelizableASTVisitor::visitChildren(const ASTNode *node) {
  for (const auto &child : node->children)
    child->accept(this);
  return {};
}

std::any ParallelizableASTVisitor::visitEntry(const EntryNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitMainFctDef(const MainFctDefNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitFctDef(const FctDefNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitProcDef(const ProcDefNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitStructDef(const StructDefNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitInterfaceDef(const InterfaceDefNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitEnumDef(const EnumDefNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitGenericTypeDef(const GenericTypeDefNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitGlobalVarDef(const GlobalVarDefNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitExtDecl(const ExtDeclNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitThreadDef(const ThreadDefNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitUnsafeBlockDef(const UnsafeBlockDefNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitForLoop(const ForLoopNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitForeachLoop(const ForeachLoopNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitWhileLoop(const WhileLoopNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitIfStmt(const IfStmtNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitElseStmt(const ElseStmtNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitAnonymousBlockStmt(const AnonymousBlockStmtNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitStmtLst(const StmtLstNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitTypeLst(const TypeLstNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitTypeAltsLst(const TypeAltsLstNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitParamLst(const ParamLstNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitArgLst(const ArgLstNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitEnumItemLst(const EnumItemLstNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitEnumItem(const EnumItemNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitField(const FieldNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitSignature(const SignatureNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitStmt(const StmtNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitDeclStmt(const DeclStmtNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitSpecifierLst(const SpecifierLstNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitSpecifier(const SpecifierNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitImportStmt(const ImportStmtNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitReturnStmt(const ReturnStmtNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitBreakStmt(const BreakStmtNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitContinueStmt(const ContinueStmtNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitAssertStmt(const AssertStmtNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitPrintfCall(const PrintfCallNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitSizeofCall(const SizeofCallNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitLenCall(const LenCallNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitTidCall(const TidCallNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitJoinCall(const JoinCallNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitAssignExpr(const AssignExprNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitTernaryExpr(const TernaryExprNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitLogicalOrExpr(const LogicalOrExprNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitLogicalAndExpr(const LogicalAndExprNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitBitwiseOrExpr(const BitwiseOrExprNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitBitwiseXorExpr(const BitwiseXorExprNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitBitwiseAndExpr(const BitwiseAndExprNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitEqualityExpr(const EqualityExprNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitRelationalExpr(const RelationalExprNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitShiftExpr(const ShiftExprNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitAdditiveExpr(const AdditiveExprNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitMultiplicativeExpr(const MultiplicativeExprNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitCastExpr(const CastExprNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitPrefixUnaryExpr(const PrefixUnaryExprNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitPostfixUnaryExpr(const PostfixUnaryExprNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitAtomicExpr(const AtomicExprNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitValue(const ValueNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitConstant(const ConstantNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitFunctionCall(const FunctionCallNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitArrayInitialization(const ArrayInitializationNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitStructInstantiation(const StructInstantiationNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitDataType(const DataTypeNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitBaseDataType(const BaseDataTypeNode *node) { return visitChildren(node); }

std::any ParallelizableASTVisitor::visitCustomDataType(const CustomDataTypeNode *node) { return visitChildren(node); }