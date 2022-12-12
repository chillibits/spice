// Copyright (c) 2021-2022 ChilliBits. All rights reserved.

#pragma once

#include <queue>
#include <stack>
#include <utility>
#include <vector>

#include <ast/ASTVisitor.h>
#include <ast/ParallelizableASTVisitor.h>
#include <model/Function.h>
#include <model/Struct.h>
#include <scope/Scope.h>
#include <util/CodeLoc.h>

/**
 * Saves a constant value for an AST node to realize features like array-out-of-bounds checks
 */
union CompileTimeValue {
  std::double_t doubleValue;
  std::int32_t intValue;
  std::int16_t shortValue;
  std::int64_t longValue;
  std::int8_t byteValue;
  std::int8_t charValue;
  const char *stringValue;
  bool boolValue;
};

// =========================================================== AstNode ===========================================================

class ASTNode {
public:
  // Constructors
  explicit ASTNode(ASTNode *parent, CodeLoc codeLoc) : parent(parent), codeLoc(std::move(codeLoc)) {}

  // Destructors
  virtual ~ASTNode() {
    for (const auto child : children)
      delete child;
  }

  // Friend classes
  friend class ASTBuilder;

  // Virtual methods
  virtual std::any accept(AbstractASTVisitor *visitor) = 0;
  virtual std::any accept(ParallelizableASTVisitor *visitor) const = 0;

  // Public methods
  template <typename T> T *createChild(const CodeLoc &loc) {
    static_assert(std::is_base_of_v<ASTNode, T>, "T must be derived from AstNode");
    T *node = new T(this, loc);
    children.push_back(node);
    return node;
  }

  template <typename T> [[nodiscard]] T *getChild(size_t i = 0) const {
    static_assert(std::is_base_of_v<ASTNode, T>, "T must be derived from AstNode");
    size_t j = 0;
    for (const auto &child : children) {
      if (auto *typedChild = dynamic_cast<T *>(child); typedChild != nullptr) {
        if (j++ == i)
          return typedChild;
      }
    }
    return nullptr;
  }

  template <typename T> [[nodiscard]] std::vector<T *> getChildren() const {
    static_assert(std::is_base_of_v<ASTNode, T>, "T must be derived from AstNode");
    std::vector<T *> nodes;
    for (const auto &child : children) {
      if (auto *typedChild = dynamic_cast<T *>(child); typedChild != nullptr)
        nodes.push_back(typedChild);
    }
    return nodes;
  }

  inline void reserveChildren(size_t numberOfChildren) { children.reserve(numberOfChildren); }

  [[nodiscard]] size_t getSymbolTypeIndex() const { // NOLINT(misc-no-recursion)
    if (symbolTypeIndex == SIZE_MAX) {
      if (parent != nullptr)
        return parent->getSymbolTypeIndex();
      return 0;
    }
    return symbolTypeIndex;
  }

  SymbolType setEvaluatedSymbolType(const SymbolType &symbolType) {
    symbolTypes.insert(symbolTypes.begin() + static_cast<long>(getSymbolTypeIndex()), symbolType);
    return symbolType;
  }

  [[nodiscard]] SymbolType getEvaluatedSymbolType() const { // NOLINT(misc-no-recursion)
    size_t idx = getSymbolTypeIndex();
    if (!symbolTypes.empty() && !symbolTypes[idx].is(TY_INVALID))
      return symbolTypes.at(idx);
    if (children.size() != 1)
      throw std::runtime_error("Cannot deduce evaluated symbol type");
    return children.front()->getEvaluatedSymbolType();
  }

  void reset() { // NOLINT(misc-no-recursion)
    // Reset all children
    for (const auto &child : children)
      child->reset();
    // Reset the symbolTypeIndex counter
    symbolTypeIndex = SIZE_MAX;
  }

  [[nodiscard]] virtual const CompileTimeValue &getCompileTimeValue() const { // NOLINT(misc-no-recursion)
    if (hasDirectCompileTimeValue || children.empty())
      return compileTimeValue;
    return children.front()->getCompileTimeValue();
  }

  [[nodiscard]] bool hasCompileTimeValue() const { // NOLINT(misc-no-recursion)
    if (hasDirectCompileTimeValue)
      return true;
    if (children.size() != 1)
      return false;
    return children.front()->hasCompileTimeValue();
  }

  [[nodiscard]] virtual bool returnsOnAllControlPaths() const {
    return children.size() == 1 && children.front()->returnsOnAllControlPaths();
  }

  [[nodiscard]] virtual std::vector<Function *> *getFctManifestations() {
    assert(false); // Must be called on FctDefNode or ProcDefNode
    return nullptr;
  }

  [[nodiscard]] virtual std::vector<Struct *> *getStructManifestations() {
    assert(false); // Must be called on StructDefNode
    return nullptr;
  }

  // Public members
  ASTNode *parent;
  std::vector<ASTNode *> children;
  const CodeLoc codeLoc;
  std::string errorMessage;
  size_t symbolTypeIndex = SIZE_MAX;
  std::vector<SymbolType> symbolTypes;
  bool unreachable = false;

protected:
  // Private members
  CompileTimeValue compileTimeValue = {};
  std::string compileTimeStringValue;

private:
  bool hasDirectCompileTimeValue = false;
};

// ========================================================== EntryNode ==========================================================

class EntryNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitEntry(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitEntry(this); }

  // Public get methods
  [[nodiscard]] std::vector<ImportStmtNode *> importStmts() const { return getChildren<ImportStmtNode>(); }
};

// ======================================================== MainFctDefNode =======================================================

class MainFctDefNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitMainFctDef(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitMainFctDef(this); }

  // Public get methods
  [[nodiscard]] ParamLstNode *paramLst() const { return getChild<ParamLstNode>(); }
  [[nodiscard]] StmtLstNode *body() const { return getChild<StmtLstNode>(); }

  // Other methods
  [[nodiscard]] std::string getScopeId() const { return "fct:main"; }
  [[nodiscard]] std::string getSignature() const { return takesArgs ? "main()" : "main(int, string[])"; }
  bool returnsOnAllControlPaths() const override;

  // Public members
  SymbolTableEntry *entry = nullptr;
  Scope *fctScope = nullptr;
  bool takesArgs = false;
};

// ========================================================== FctDefNode =========================================================

class FctDefNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitFctDef(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitFctDef(this); }

  // Public get methods
  [[nodiscard]] SpecifierLstNode *specifierLst() const { return getChild<SpecifierLstNode>(); }
  [[nodiscard]] DataTypeNode *returnType() const { return getChild<DataTypeNode>(); }
  [[nodiscard]] TypeLstNode *templateTypeLst() const { return getChild<TypeLstNode>(); }
  [[nodiscard]] ParamLstNode *paramLst() const { return getChild<ParamLstNode>(); }
  [[nodiscard]] StmtLstNode *body() const { return getChild<StmtLstNode>(); }

  // Other methods
  [[nodiscard]] std::string getScopeId() const { return "fct:" + codeLoc.toString(); }
  [[nodiscard]] std::string getSymbolTableEntryName() const { return functionName + ":" + codeLoc.toPrettyLine(); }
  [[nodiscard]] bool returnsOnAllControlPaths() const override;
  std::vector<Function *> *getFctManifestations() override { return &fctManifestations; }

  // Public members
  std::string functionName;
  std::string structName;
  std::string fqFunctionName;
  std::vector<std::string> functionNameFragments;
  bool isMethod = false;
  bool hasTemplateTypes = false;
  bool hasParams = false;
  SymbolTableEntry *entry = nullptr;
  Scope *structScope = nullptr;
  Scope *fctScope = nullptr;
  std::vector<Function *> fctManifestations;
};

// ========================================================== ProcDefNode ========================================================

class ProcDefNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitProcDef(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitProcDef(this); }

  // Public get methods
  [[nodiscard]] SpecifierLstNode *specifierLst() const { return getChild<SpecifierLstNode>(); }
  [[nodiscard]] TypeLstNode *templateTypeLst() const { return getChild<TypeLstNode>(); }
  [[nodiscard]] ParamLstNode *paramLst() const { return getChild<ParamLstNode>(); }
  [[nodiscard]] StmtLstNode *body() const { return getChild<StmtLstNode>(); }

  // Other methods
  [[nodiscard]] std::string getScopeId() const { return "proc:" + codeLoc.toString(); }
  [[nodiscard]] std::string getSymbolTableEntryName() const { return procedureName + ":" + codeLoc.toPrettyLine(); }
  bool returnsOnAllControlPaths() const override;
  std::vector<Function *> *getFctManifestations() override { return &procManifestations; }

  // Public members
  std::string procedureName;
  std::string structName;
  std::string fqProcedureName;
  std::vector<std::string> procedureNameFragments;
  bool isMethod = false;
  bool hasTemplateTypes = false;
  bool hasParams = false;
  SymbolTableEntry *entry = nullptr;
  Scope *structScope = nullptr;
  Scope *procScope = nullptr;
  Function *procBase = nullptr;
  std::vector<Function *> procManifestations;
};

// ========================================================= StructDefNode =======================================================

class StructDefNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitStructDef(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitStructDef(this); }

  // Public get methods
  [[nodiscard]] SpecifierLstNode *specifierLst() const { return getChild<SpecifierLstNode>(); }
  [[nodiscard]] std::vector<FieldNode *> fields() const { return getChildren<FieldNode>(); }
  [[nodiscard]] TypeLstNode *templateTypeLst() const { return getChild<TypeLstNode>(0); }
  [[nodiscard]] TypeLstNode *interfaceTypeLst() const { return getChild<TypeLstNode>(isGeneric ? 1 : 0); }
  std::vector<Struct *> *getStructManifestations() override { return &structManifestations; }

  // Public members
  std::string structName;
  bool isGeneric = false;
  bool hasInterfaces = false;
  SymbolTableEntry *entry = nullptr;
  std::vector<Struct *> structManifestations;
  Scope *structScope = nullptr;
  Struct *spiceStruct = nullptr;
};

// ======================================================= InterfaceDefNode ======================================================

class InterfaceDefNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitInterfaceDef(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitInterfaceDef(this); }

  // Public get methods
  [[nodiscard]] SpecifierLstNode *specifierLst() const { return getChild<SpecifierLstNode>(); }
  [[nodiscard]] std::vector<SignatureNode *> signatures() const { return getChildren<SignatureNode>(); }

  // Public members
  std::string interfaceName;
  SymbolTableEntry *entry = nullptr;
  Scope *interfaceScope = nullptr;
  Interface *spiceInterface = nullptr;
};

// ========================================================== EnumDefNode ========================================================

class EnumDefNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitEnumDef(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitEnumDef(this); }

  // Public get methods
  [[nodiscard]] SpecifierLstNode *specifierLst() const { return getChild<SpecifierLstNode>(); }
  [[nodiscard]] EnumItemLstNode *itemLst() const { return getChild<EnumItemLstNode>(); }

  // Public members
  std::string enumName;
  Scope *enumScope;
  SymbolTableEntry *entry = nullptr;
};

// ====================================================== GenericTypeDefNode =====================================================

class GenericTypeDefNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitGenericTypeDef(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitGenericTypeDef(this); }

  // Public get methods
  [[nodiscard]] TypeAltsLstNode *typeAltsLst() const { return getChild<TypeAltsLstNode>(); }

  // Public members
  std::string typeName;
};

// ======================================================= GlobalVarDefNode ======================================================

class GlobalVarDefNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitGlobalVarDef(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitGlobalVarDef(this); }

  // Public get methods
  [[nodiscard]] SpecifierLstNode *specifierLst() const { return getChild<SpecifierLstNode>(); }
  [[nodiscard]] DataTypeNode *dataType() const { return getChild<DataTypeNode>(); }
  [[nodiscard]] ConstantNode *constant() const { return getChild<ConstantNode>(); }

  // Public members
  std::string varName;
  SymbolTableEntry *entry = nullptr;
};

// ========================================================== ExtDeclNode ========================================================

class ExtDeclNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitExtDecl(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitExtDecl(this); }

  // Public get methods
  [[nodiscard]] DataTypeNode *returnType() const { return getChild<DataTypeNode>(); }
  [[nodiscard]] TypeLstNode *argTypeLst() const { return getChild<TypeLstNode>(); }

  // Public members
  std::string extFunctionName;
  bool hasArgs = false;
  bool isVarArg = false;
  bool isDll = false;
  Scope *fctScope = nullptr;
  SymbolTableEntry *entry = nullptr;
  Function *externalFunction = nullptr;
};

// ========================================================= ThreadDefNode =======================================================

class ThreadDefNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitThreadDef(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitThreadDef(this); }

  // Public get methods
  [[nodiscard]] StmtLstNode *body() const { return getChild<StmtLstNode>(); }

  // Other methods
  [[nodiscard]] std::string getScopeId() const { return "thread:" + codeLoc.toString(); }
  [[nodiscard]] bool returnsOnAllControlPaths() const override;

  // Public members
  Scope *bodyScope = nullptr;
};

// ====================================================== UnsafeBlockDefNode =====================================================

class UnsafeBlockDefNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitUnsafeBlockDef(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitUnsafeBlockDef(this); }

  // Public get methods
  [[nodiscard]] StmtLstNode *body() const { return getChild<StmtLstNode>(); }

  // Other methods
  [[nodiscard]] std::string getScopeId() const { return "unsafe:" + codeLoc.toString(); }

  // Public members
  Scope *bodyScope = nullptr;
};

// ========================================================== ForLoopNode ========================================================

class ForLoopNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitForLoop(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitForLoop(this); }

  // Public get methods
  [[nodiscard]] DeclStmtNode *initDecl() const { return getChild<DeclStmtNode>(); }
  [[nodiscard]] AssignExprNode *condAssign() const { return getChild<AssignExprNode>(0); }
  [[nodiscard]] AssignExprNode *incAssign() const { return getChild<AssignExprNode>(1); }
  [[nodiscard]] StmtLstNode *body() const { return getChild<StmtLstNode>(); }

  // Other methods
  [[nodiscard]] std::string getScopeId() const { return "for:" + codeLoc.toString(); }
  [[nodiscard]] bool returnsOnAllControlPaths() const override;

  // Public members
  Scope *bodyScope = nullptr;
};

// ======================================================== ForeachLoopNode ======================================================

class ForeachLoopNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitForeachLoop(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitForeachLoop(this); }

  // Public get methods
  [[nodiscard]] DeclStmtNode *idxVarDecl() const {
    std::vector<DeclStmtNode *> declStmtNodes = getChildren<DeclStmtNode>();
    return declStmtNodes.size() == 2 ? declStmtNodes.front() : nullptr;
  }
  [[nodiscard]] DeclStmtNode *itemDecl() const { return getChildren<DeclStmtNode>().back(); }
  [[nodiscard]] AssignExprNode *arrayAssign() const { return getChild<AssignExprNode>(); }
  [[nodiscard]] StmtLstNode *body() const { return getChild<StmtLstNode>(); }

  // Other methods
  [[nodiscard]] std::string getScopeId() const { return "foreach:" + codeLoc.toString(); }

  // Public members
  Scope *bodyScope = nullptr;
};

// ========================================================= WhileLoopNode =======================================================

class WhileLoopNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitWhileLoop(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitWhileLoop(this); }

  // Public get methods
  [[nodiscard]] AssignExprNode *condition() const { return getChild<AssignExprNode>(); }
  [[nodiscard]] StmtLstNode *body() const { return getChild<StmtLstNode>(); }

  // Other methods
  [[nodiscard]] std::string getScopeId() const { return "while:" + codeLoc.toString(); }
  [[nodiscard]] bool returnsOnAllControlPaths() const override;

  // Public members
  Scope *bodyScope = nullptr;
};

// ========================================================== IfStmtNode =========================================================

class IfStmtNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitIfStmt(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitIfStmt(this); }

  // Public get methods
  [[nodiscard]] AssignExprNode *condition() const { return getChild<AssignExprNode>(); }
  [[nodiscard]] StmtLstNode *thenBody() const { return getChild<StmtLstNode>(); }
  [[nodiscard]] ElseStmtNode *elseStmt() const { return getChild<ElseStmtNode>(); }

  // Other methods
  [[nodiscard]] std::string getScopeId() const { return "if:" + codeLoc.toString(); }
  [[nodiscard]] bool returnsOnAllControlPaths() const override;

  // Public members
  Scope *thenBodyScope = nullptr;
};

// ========================================================= ElseStmtNode ========================================================

class ElseStmtNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitElseStmt(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitElseStmt(this); }

  // Public get methods
  [[nodiscard]] IfStmtNode *ifStmt() const { return getChild<IfStmtNode>(); }
  [[nodiscard]] StmtLstNode *body() const { return getChild<StmtLstNode>(); }

  // Other methods
  [[nodiscard]] std::string getScopeId() const { return "if:" + codeLoc.toString(); }
  [[nodiscard]] bool returnsOnAllControlPaths() const override;

  // Public members
  bool isElseIf = false;
  Scope *elseBodyScope = nullptr;
};

// ========================================================== ScopeNode ==========================================================

class AnonymousBlockStmtNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitAnonymousBlockStmt(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitAnonymousBlockStmt(this); }

  // Public get methods
  [[nodiscard]] StmtLstNode *body() const { return getChild<StmtLstNode>(); }

  // Other methods
  [[nodiscard]] std::string getScopeId() const { return "anon:" + codeLoc.toString(); }

  // Public members
  Scope *bodyScope = nullptr;
};

// ========================================================= StmtLstNode =========================================================

class StmtLstNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitStmtLst(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitStmtLst(this); }

  // Other methods
  [[nodiscard]] bool returnsOnAllControlPaths() const override;

  // Public members
  size_t complexity = 0;
};

// ========================================================= TypeLstNode =========================================================

class TypeLstNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitTypeLst(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitTypeLst(this); }

  // Public get methods
  [[nodiscard]] std::vector<DataTypeNode *> dataTypes() const { return getChildren<DataTypeNode>(); }

  // Public members
  size_t numberOfTypes = 0;
};

// ======================================================= TypeAltsLstNode =======================================================

class TypeAltsLstNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitTypeAltsLst(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitTypeAltsLst(this); }

  // Public get methods
  [[nodiscard]] std::vector<DataTypeNode *> dataTypes() const { return getChildren<DataTypeNode>(); }

  // Public members
  size_t numberOfAlts = 0;
};

// ======================================================== ParamLstNode =========================================================

class ParamLstNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitParamLst(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitParamLst(this); }

  // Public get methods
  [[nodiscard]] std::vector<DeclStmtNode *> params() const { return getChildren<DeclStmtNode>(); }
};

// ========================================================== ArgLstNode =========================================================

class ArgLstNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitArgLst(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitArgLst(this); }

  // Public get methods
  [[nodiscard]] std::vector<AssignExprNode *> args() const { return getChildren<AssignExprNode>(); }
};

// ======================================================== EnumItemLstNode ======================================================

class EnumItemLstNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitEnumItemLst(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitEnumItemLst(this); }

  // Public get methods
  [[nodiscard]] std::vector<EnumItemNode *> items() const { return getChildren<EnumItemNode>(); }

  // Public members
  EnumDefNode *enumDef = nullptr;
};

// ========================================================= EnumItemNode ========================================================

class EnumItemNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitEnumItem(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitEnumItem(this); }

  // Public get methods

  // Public members
  std::string itemName;
  uint32_t itemValue;
  bool hasValue = false;
  SymbolTableEntry *entry = nullptr;
  EnumDefNode *enumDef = nullptr;
};

// ========================================================== FieldNode ==========================================================

class FieldNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitField(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitField(this); }

  // Public get methods
  [[nodiscard]] SpecifierLstNode *specifierLst() const { return getChild<SpecifierLstNode>(); }
  [[nodiscard]] DataTypeNode *dataType() const { return getChild<DataTypeNode>(); }

  // Public members
  std::string fieldName;
  SymbolTableEntry *entry = nullptr;
};

// ======================================================== SignatureNode ========================================================

class SignatureNode : public ASTNode {
public:
  // Enums
  enum Type {
    TYPE_FUNCTION,
    TYPE_PROCEDURE,
  };

  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitSignature(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitSignature(this); }

  // Public get methods
  [[nodiscard]] SpecifierLstNode *specifierLst() const { return getChild<SpecifierLstNode>(); }
  [[nodiscard]] DataTypeNode *dataType() const { return getChild<DataTypeNode>(); }
  [[nodiscard]] TypeLstNode *paramTypeLst() const { return getChild<TypeLstNode>(); }

  // Public members
  std::string methodName;
  Type signatureType = SignatureNode::TYPE_PROCEDURE;
  bool hasParams = false;
  std::vector<Function *> signatureManifestations;
};

// =========================================================== StmtNode ==========================================================

class StmtNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitStmt(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitStmt(this); }
};

// ========================================================= DeclStmtNode ========================================================

class DeclStmtNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitDeclStmt(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitDeclStmt(this); }

  // Public get methods
  [[nodiscard]] SpecifierLstNode *specifierLst() const { return getChild<SpecifierLstNode>(); }
  [[nodiscard]] DataTypeNode *dataType() const { return getChild<DataTypeNode>(); }
  [[nodiscard]] AssignExprNode *assignExpr() const { return getChild<AssignExprNode>(); }

  // Util methods
  [[nodiscard]] bool isParam() const { return dynamic_cast<ParamLstNode *>(parent); }

  // Public members
  std::string varName;
  bool hasAssignment = false;
  SymbolTableEntry *entry = nullptr;
};

// ======================================================= SpecifierLstNode ======================================================

class SpecifierLstNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitSpecifierLst(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitSpecifierLst(this); }

  // Public get methods
  [[nodiscard]] std::vector<SpecifierNode *> specifiers() const { return getChildren<SpecifierNode>(); }
};

// ========================================================= SpecifierNode =======================================================

class SpecifierNode : public ASTNode {
public:
  // Enums
  enum SpecifierType { TY_CONST, TY_SIGNED, TY_UNSIGNED, TY_INLINE, TY_PUBLIC, TY_HEAP };

  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitSpecifier(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitSpecifier(this); }

  // Public members
  SpecifierType type;
};

// ======================================================== ImportStmtNode =======================================================

class ImportStmtNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitImportStmt(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitImportStmt(this); }

  // Public members
  std::string importPath;
  std::string importName;
};

// ======================================================== ReturnStmtNode =======================================================

class ReturnStmtNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitReturnStmt(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitReturnStmt(this); }

  // Public get methods
  [[nodiscard]] AssignExprNode *assignExpr() const { return getChild<AssignExprNode>(); }

  // Other methods
  [[nodiscard]] bool returnsOnAllControlPaths() const override { return true; }

  // Public members
  bool hasReturnValue = false;
};

// ======================================================== BreakStmtNode ========================================================

class BreakStmtNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitBreakStmt(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitBreakStmt(this); }

  // Public members
  int breakTimes = 1;
};

// ======================================================= ContinueStmtNode ======================================================

class ContinueStmtNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitContinueStmt(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitContinueStmt(this); }

  // Public members
  int continueTimes = 1;
};

// ======================================================== AssertStmtNode =======================================================

class AssertStmtNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitAssertStmt(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitAssertStmt(this); }

  // Public get methods
  [[nodiscard]] AssignExprNode *assignExpr() const { return getChild<AssignExprNode>(); }

  // Public members
  std::string expressionString;
};

// ======================================================== PrintfCallNode =======================================================

class PrintfCallNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitPrintfCall(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitPrintfCall(this); }

  // Public get methods
  [[nodiscard]] std::vector<AssignExprNode *> assignExpr() const { return getChildren<AssignExprNode>(); }

  // Public members
  std::string templatedString;
};

// ======================================================== SizeofCallNode =======================================================

class SizeofCallNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitSizeofCall(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitSizeofCall(this); }

  // Public get methods
  [[nodiscard]] AssignExprNode *assignExpr() const { return getChild<AssignExprNode>(); }
  [[nodiscard]] DataTypeNode *dataType() const { return getChild<DataTypeNode>(); }

  // Public members
  bool isType = false;
};

// ========================================================= LenCallNode =========================================================

class LenCallNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitLenCall(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitLenCall(this); }

  // Public get methods
  [[nodiscard]] AssignExprNode *assignExpr() const { return getChild<AssignExprNode>(); }
};

// ========================================================= TidCallNode =========================================================

class TidCallNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitTidCall(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitTidCall(this); }
};

// ======================================================== JoinCallNode =========================================================

class JoinCallNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitJoinCall(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitJoinCall(this); }

  // Public get methods
  [[nodiscard]] std::vector<AssignExprNode *> assignExpressions() const { return getChildren<AssignExprNode>(); }
};

// ======================================================= AssignExprNode ========================================================

class AssignExprNode : public ASTNode {
public:
  // Enums
  enum AssignOp {
    OP_ASSIGN,
    OP_PLUS_EQUAL,
    OP_MINUS_EQUAL,
    OP_MUL_EQUAL,
    OP_DIV_EQUAL,
    OP_REM_EQUAL,
    OP_SHL_EQUAL,
    OP_SHR_EQUAL,
    OP_AND_EQUAL,
    OP_OR_EQUAL,
    OP_XOR_EQUAL
  };

  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitAssignExpr(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitAssignExpr(this); }

  // Public get methods
  [[nodiscard]] AssignExprNode *rhs() const { return getChild<AssignExprNode>(); }
  [[nodiscard]] PrefixUnaryExprNode *lhs() const { return getChild<PrefixUnaryExprNode>(); }
  [[nodiscard]] TernaryExprNode *ternaryExpr() const { return getChild<TernaryExprNode>(); }
  [[nodiscard]] ThreadDefNode *threadDef() const { return getChild<ThreadDefNode>(); }

  // Other methods
  [[nodiscard]] bool returnsOnAllControlPaths() const override;

  // Public members
  AssignOp op;
  bool hasOperator = false;
};

// ======================================================= TernaryExprNode =======================================================

class TernaryExprNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitTernaryExpr(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitTernaryExpr(this); }

  // Public get methods
  [[nodiscard]] std::vector<LogicalOrExprNode *> operands() const { return getChildren<LogicalOrExprNode>(); }

  // Other methods
  [[nodiscard]] const CompileTimeValue &getCompileTimeValue() const override;

  bool isShortened = false;
};

// ===================================================== LogicalOrExprNode =======================================================

class LogicalOrExprNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitLogicalOrExpr(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitLogicalOrExpr(this); }

  // Public get methods
  [[nodiscard]] std::vector<LogicalAndExprNode *> operands() const { return getChildren<LogicalAndExprNode>(); }

  // Other methods
  [[nodiscard]] const CompileTimeValue &getCompileTimeValue() const override;
};

// ===================================================== LogicalAndExprNode ======================================================

class LogicalAndExprNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitLogicalAndExpr(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitLogicalAndExpr(this); }

  // Public get methods
  [[nodiscard]] std::vector<BitwiseOrExprNode *> operands() const { return getChildren<BitwiseOrExprNode>(); }
};

// ===================================================== BitwiseOrExprNode =======================================================

class BitwiseOrExprNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitBitwiseOrExpr(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitBitwiseOrExpr(this); }

  // Public get methods
  [[nodiscard]] std::vector<BitwiseXorExprNode *> operands() const { return getChildren<BitwiseXorExprNode>(); }
};

// ==================================================== BitwiseXorExprNode =======================================================

class BitwiseXorExprNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitBitwiseXorExpr(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitBitwiseXorExpr(this); }

  // Public get methods
  [[nodiscard]] std::vector<BitwiseAndExprNode *> operands() const { return getChildren<BitwiseAndExprNode>(); }
};

// ==================================================== BitwiseAndExprNode =======================================================

class BitwiseAndExprNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitBitwiseAndExpr(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitBitwiseAndExpr(this); }

  // Public get methods
  [[nodiscard]] std::vector<EqualityExprNode *> operands() const { return getChildren<EqualityExprNode>(); }
};

// ===================================================== EqualityExprNode ========================================================

class EqualityExprNode : public ASTNode {
public:
  // Enums
  enum EqualityOp { OP_EQUAL, OP_NOT_EQUAL };

  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitEqualityExpr(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitEqualityExpr(this); }

  // Public get methods
  [[nodiscard]] std::vector<RelationalExprNode *> operands() const { return getChildren<RelationalExprNode>(); }

  // Public members
  EqualityOp op;
};

// ==================================================== RelationalExprNode =======================================================

class RelationalExprNode : public ASTNode {
public:
  // Enums
  enum RelationalOp { OP_LESS, OP_GREATER, OP_LESS_EQUAL, OP_GREATER_EQUAL };

  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitRelationalExpr(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitRelationalExpr(this); }

  // Public get methods
  [[nodiscard]] std::vector<ShiftExprNode *> operands() const { return getChildren<ShiftExprNode>(); }

  // Public members
  RelationalOp op;
};

// ====================================================== ShiftExprNode ==========================================================

class ShiftExprNode : public ASTNode {
public:
  // Enums
  enum ShiftOp {
    OP_SHIFT_LEFT,
    OP_SHIFT_RIGHT,
  };

  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitShiftExpr(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitShiftExpr(this); }

  // Public get methods
  [[nodiscard]] std::vector<AdditiveExprNode *> operands() const { return getChildren<AdditiveExprNode>(); }

  // Public members
  ShiftOp op;
};

// ==================================================== AdditiveExprNode =========================================================

class AdditiveExprNode : public ASTNode {
public:
  // Enums
  enum AdditiveOp {
    OP_PLUS,
    OP_MINUS,
  };

  // Typedefs
  typedef std::queue<std::pair<AdditiveOp, SymbolType>> OpQueue;

  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitAdditiveExpr(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitAdditiveExpr(this); }

  // Public get methods
  [[nodiscard]] std::vector<MultiplicativeExprNode *> operands() const { return getChildren<MultiplicativeExprNode>(); }

  // Public members
  OpQueue opQueue;
};

// ================================================== MultiplicativeExprNode =====================================================

class MultiplicativeExprNode : public ASTNode {
public:
  // Enums
  enum MultiplicativeOp {
    OP_MUL,
    OP_DIV,
    OP_REM,
  };

  // Typedefs
  typedef std::queue<std::pair<MultiplicativeOp, SymbolType>> OpQueue;

  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitMultiplicativeExpr(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitMultiplicativeExpr(this); }

  // Public get methods
  [[nodiscard]] std::vector<CastExprNode *> operands() const { return getChildren<CastExprNode>(); }

  // Public members
  OpQueue opQueue;
};

// ======================================================= CastExprNode ==========================================================

class CastExprNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitCastExpr(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitCastExpr(this); }

  // Public get methods
  [[nodiscard]] DataTypeNode *dataType() const { return getChild<DataTypeNode>(); }
  [[nodiscard]] PrefixUnaryExprNode *prefixUnaryExpr() const { return getChild<PrefixUnaryExprNode>(); }

  // Public members
  bool isCasted = false;
};

// ==================================================== PrefixUnaryExprNode ======================================================

class PrefixUnaryExprNode : public ASTNode {
public:
  // Enums
  enum PrefixUnaryOp {
    OP_MINUS,
    OP_PLUS_PLUS,
    OP_MINUS_MINUS,
    OP_NOT,
    OP_BITWISE_NOT,
    OP_INDIRECTION,
    OP_ADDRESS_OF,
  };

  // Typedefs
  typedef std::queue<std::pair<PrefixUnaryOp, SymbolType>> OpQueue;

  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitPrefixUnaryExpr(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitPrefixUnaryExpr(this); }

  // Public get methods
  [[nodiscard]] PostfixUnaryExprNode *postfixUnaryExpr() const { return getChild<PostfixUnaryExprNode>(); }

  // Public members
  OpQueue opQueue;
};

// =================================================== PostfixUnaryExprNode ======================================================

class PostfixUnaryExprNode : public ASTNode {
public:
  // Enums
  enum PostfixUnaryOp { OP_SUBSCRIPT, OP_MEMBER_ACCESS, OP_PLUS_PLUS, OP_MINUS_MINUS };

  // Typedefs
  typedef std::queue<std::pair<PostfixUnaryOp, SymbolType>> OpQueue;

  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitPostfixUnaryExpr(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitPostfixUnaryExpr(this); }

  // Public get methods
  [[nodiscard]] AtomicExprNode *atomicExpr() const { return getChild<AtomicExprNode>(); }
  [[nodiscard]] std::vector<AssignExprNode *> assignExpr() const { return getChildren<AssignExprNode>(); }

  // Public members
  OpQueue opQueue;
  std::vector<std::string> identifier; // Only set when operator is member access
};

// ====================================================== AtomicExprNode =========================================================

class AtomicExprNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitAtomicExpr(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitAtomicExpr(this); }

  // Public get methods
  [[nodiscard]] ConstantNode *constant() const { return getChild<ConstantNode>(); }
  [[nodiscard]] ValueNode *value() const { return getChild<ValueNode>(); }
  [[nodiscard]] AssignExprNode *assignExpr() const { return getChild<AssignExprNode>(); }
  [[nodiscard]] PrintfCallNode *printfCall() const { return getChild<PrintfCallNode>(); }
  [[nodiscard]] SizeofCallNode *sizeofCall() const { return getChild<SizeofCallNode>(); }
  [[nodiscard]] LenCallNode *lenCall() const { return getChild<LenCallNode>(); }
  [[nodiscard]] TidCallNode *tidCall() const { return getChild<TidCallNode>(); }
  [[nodiscard]] JoinCallNode *joinCall() const { return getChild<JoinCallNode>(); }

  // Public members
  std::vector<std::string> identifierFragments;
  std::string fqIdentifier;
  SymbolTableEntry *entry = nullptr; // Only set if identifier is set as well
};

// ======================================================== ValueNode ============================================================

class ValueNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitValue(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitValue(this); }

  // Public get methods
  [[nodiscard]] ConstantNode *constant() const { return getChild<ConstantNode>(); }
  [[nodiscard]] FunctionCallNode *functionCall() const { return getChild<FunctionCallNode>(); }
  [[nodiscard]] ArrayInitializationNode *arrayInitialization() const { return getChild<ArrayInitializationNode>(); }
  [[nodiscard]] StructInstantiationNode *structInstantiation() const { return getChild<StructInstantiationNode>(); }
  [[nodiscard]] DataTypeNode *nilType() const { return getChild<DataTypeNode>(); }

  // Public members
  bool isNil = false;
};

// =================================================== PrimitiveValueNode ========================================================

class ConstantNode : public ASTNode {
public:
  // Enum
  enum PrimitiveValueType { TYPE_DOUBLE, TYPE_INT, TYPE_SHORT, TYPE_LONG, TYPE_BYTE, TYPE_CHAR, TYPE_STRING, TYPE_BOOL };

  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitConstant(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitConstant(this); }

  // Public members
  PrimitiveValueType type;
  bool isSigned = true;
};

// ==================================================== FunctionCallNode =========================================================

class FunctionCallNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitFunctionCall(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitFunctionCall(this); }

  // Public get methods
  [[nodiscard]] ArgLstNode *argLst() const { return getChild<ArgLstNode>(); }

  // Public members
  std::string fqFunctionName;
  std::vector<std::string> functionNameFragments;
  bool hasArgs = false;
  bool isConstructorCall = false;
  bool isMethodCall = false;
  SymbolType thisType = SymbolType(TY_DYN); // Is filled if method or ctor call
  std::vector<SymbolType> argTypes;
  Function *calledFunction = nullptr;
};

// ================================================= ArrayInitializationNode =====================================================

class ArrayInitializationNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitArrayInitialization(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitArrayInitialization(this); }

  // Public get methods
  [[nodiscard]] ArgLstNode *itemLst() const { return getChild<ArgLstNode>(); }
};

// ================================================= StructInstantiationNode =====================================================

class StructInstantiationNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitStructInstantiation(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitStructInstantiation(this); }

  // Public get methods
  [[nodiscard]] TypeLstNode *templateTypeLst() const { return getChild<TypeLstNode>(); }
  [[nodiscard]] ArgLstNode *fieldLst() const { return getChild<ArgLstNode>(); }

  // Public members
  std::string fqStructName;
  std::vector<std::string> structNameFragments;
  Struct *instantiatedStruct;
};

// ======================================================= DataTypeNode ==========================================================

class DataTypeNode : public ASTNode {
public:
  // Enums
  enum TypeModifierType { TYPE_PTR, TYPE_REF, TYPE_ARRAY };

  // Structs
  struct TypeModifier {
    TypeModifierType modifierType = TYPE_PTR;
    bool hasSize = false;
    bool isSizeHardcoded = false;
    int hardcodedSize = 0;
  };

  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitDataType(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitDataType(this); }

  // Public get methods
  [[nodiscard]] BaseDataTypeNode *baseDataType() const { return getChild<BaseDataTypeNode>(); }
  [[nodiscard]] std::vector<AssignExprNode *> arraySizeExpr() const { return getChildren<AssignExprNode>(); }

  // Util methods
  [[nodiscard]] bool isParamType() const {
    auto declStmt = dynamic_cast<DeclStmtNode *>(parent);
    return declStmt && declStmt->isParam();
  }

  // Public members
  std::queue<TypeModifier> tmQueue;
};

// ==================================================== BaseDataTypeNode =========================================================

class BaseDataTypeNode : public ASTNode {
public:
  // Enums
  enum Type { TYPE_DOUBLE, TYPE_INT, TYPE_SHORT, TYPE_LONG, TYPE_BYTE, TYPE_CHAR, TYPE_STRING, TYPE_BOOL, TYPE_DYN, TY_CUSTOM };

  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitBaseDataType(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitBaseDataType(this); }

  // Public get methods
  [[nodiscard]] CustomDataTypeNode *customDataType() const { return getChild<CustomDataTypeNode>(); }

  // Public members
  Type type;
};

// ==================================================== CustomDataTypeNode =======================================================

class CustomDataTypeNode : public ASTNode {
public:
  // Constructors
  using ASTNode::ASTNode;

  // Visitor methods
  std::any accept(AbstractASTVisitor *visitor) override { return visitor->visitCustomDataType(this); }
  std::any accept(ParallelizableASTVisitor *visitor) const override { return visitor->visitCustomDataType(this); }

  // Public get methods
  [[nodiscard]] TypeLstNode *templateTypeLst() const { return getChild<TypeLstNode>(); }

  // Public members
  std::string fqTypeName;
  std::vector<std::string> typeNameFragments;
  SymbolTableEntry *customType = nullptr;
};